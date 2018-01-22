//#define SERIAL_DEDUG
#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include "DateTime.h"
#include "tools.h"
#include "BrowserServer.h" 
#include "Scales.h"
#include "Task.h"
#include "version.h"

/*
 * This example serves a "hello world" on a WLAN and a SoftAP at the same time.
 * The SoftAP allow you to configure WLAN parameters at run time. They are not setup in the sketch but saved on EEPROM.
 * 
 * Connect your computer or cell phone to wifi network ESP_ap with password 12345678. A popup may appear and it allow you to go to WLAN config. If it does not then navigate to http://192.168.4.1/wifi and config it there.
 * Then wait for the module to connect to your wifi and take note of the WLAN IP it got. Then you can disconnect from ESP_ap and return to your regular WLAN.
 * 
 * Now the ESP8266 is in your network. You can reach it through http://192.168.x.x/ (the IP you took note of) or maybe at http://esp8266.local too.
 * 
 * This is a captive portal because through the softAP it will redirect any http request to http://192.168.4.1/
 */
void onStationModeConnected(const WiFiEventStationModeConnected& evt);
void onStationModeDisconnected(const WiFiEventStationModeDisconnected& evt);
void takeBlink();
void takeBattery();
void powerSwitchInterrupt();
void connectWifi();
//
TaskController taskController = TaskController();		/*  */
Task taskBlink(takeBlink, 500);							/*  */
Task taskBattery(takeBattery, 20000);					/* 20 Обновляем заряд батареи */
Task taskPower(powerOff, 1200000);						/* 10 минут бездействия и выключаем */
Task taskConnectWiFi(connectWifi, 60000);				/* Пытаемся соедениться с точкой доступа каждые 60 секунд */
WiFiEventHandler stationModeConnectedHandler;
WiFiEventHandler stationModeDisconnectedHandler;

unsigned int COUNT_FLASH = 500;
unsigned int COUNT_BLINK = 500;
//
void connectWifi();
//
long lastConnectTry = 0;

/** Current WLAN status */
int status = WL_IDLE_STATUS;

void setup() {
	pinMode(EN_NCP, OUTPUT);
	digitalWrite(EN_NCP, HIGH);
	pinMode(LED, OUTPUT);
	//digitalWrite(LED, HIGH);
	pinMode(PWR_SW, INPUT);
	digitalWrite(LED, HIGH);
	while (digitalRead(PWR_SW) == HIGH){
		delay(100);
	};
	
	takeBattery();
	Serial.begin(9600);	
	taskController.add(&taskBlink);
	taskController.add(&taskBattery);
	taskController.add(&taskConnectWiFi);
	taskConnectWiFi.pause();
	taskController.add(&taskPower);	
	
	SPIFFS.begin(); // Not really needed, checked inside library and started if needed	
	SCALES.begin();	
	
	stationModeConnectedHandler = WiFi.onStationModeConnected(&onStationModeConnected);	
	stationModeDisconnectedHandler = WiFi.onStationModeDisconnected(&onStationModeDisconnected);
	
	/* You can remove the password parameter if you want the AP to be open. */
	WiFi.hostname(myHostname);
	WiFi.softAPConfig(apIP, apIP, netMsk);
	WiFi.softAP(softAP_ssid, softAP_password);
	delay(500); // Without delay I've seen the IP address blank
	#if defined SERIAL_DEDUG
		Serial.print("AP IP address: ");
		Serial.println(WiFi.softAPIP());	
	#endif	
	//ESP.eraseConfig();
	connectWifi();
	browserServer.begin(); 
  
	connect = SCALES.getSSID().length() > 0; // Request WLAN connect if there is a SSID
	Rtc.Begin();
	SCALES.saveEvent("weight", "ON");		
}

void takeBlink() {
	bool led = !digitalRead(LED);
	digitalWrite(LED, led);	
	taskBlink.setInterval(led ? COUNT_FLASH : COUNT_BLINK );
}

/**/
void takeBattery(){	
	unsigned int charge = SCALES.getBattery(1);
	charge = constrain(charge, MIN_CHG, MAX_CHG);	
	charge = map(charge, MIN_CHG, MAX_CHG, 0, 100);				
	SCALES.setCharge(charge);
	if (SCALES.getCharge() < 16){												//< Если заряд батареи 15% тогда выключаем модуль
		powerOff();
	}		
}

void powerSwitchInterrupt(){
	unsigned long t = millis();
	//delay(100);
	if(digitalRead(PWR_SW)==HIGH){ //
		digitalWrite(LED, HIGH);
		while(digitalRead(PWR_SW)==HIGH){ //
			delay(100);
			if(t + 4000 < millis()){ //
				digitalWrite(LED, LOW);
				while(digitalRead(PWR_SW) == HIGH){delay(10);};//
				powerOff();
				//ESP.reset();
				break;
			}
			digitalWrite(LED, !digitalRead(LED));
		}
	}
}

void connectWifi() {
	#if defined SERIAL_DEDUG
		Serial.println("Connecting as wifi client...");
	#endif
	WiFi.persistent(false);
	//WiFi.disconnect();
	/*!  */
	int n = WiFi.scanNetworks();	
	if (n == 0){		
		goto disconnect;	
	}else{
		for (int i = 0; i < n; ++i)	{
			/*!  */
			if(WiFi.SSID(i) == SCALES.getSSID().c_str()){
				WiFi.begin ( SCALES.getSSID().c_str(), SCALES.getPASS().c_str());
				int connRes = WiFi.waitForConnectResult();
				#if defined SERIAL_DEDUG
					Serial.print ( "connRes: " );
					Serial.println ( connRes );
				#endif
				return;
			}
		}
		goto disconnect;
	}
	disconnect:;
	{
		WiFi.disconnect();
	}
}

void loop() {
	taskController.run();	
	//DNS
	dnsServer.processNextRequest();
	//HTTP
	browserServer.handleClient();
	
	if (Serial.available()) {		
		SCALES.parseDate(Serial.readStringUntil(LF));
	}
	
	powerSwitchInterrupt();	
}

void onStationModeConnected(const WiFiEventStationModeConnected& evt) {
	taskConnectWiFi.pause();
	// Setup MDNS responder
	if (MDNS.begin(myHostname)) {
		// Add service to MDNS-SD
		MDNS.addService("http", "tcp", 80);
	}
	COUNT_FLASH = 50;
	COUNT_BLINK = 3000;
	SCALES.saveEvent("ip", SCALES.getIp());
	//attachInterrupt(digitalPinToInterrupt(PWR_SW), powerSwitchInterrupt, RISING);
}

void onStationModeDisconnected(const WiFiEventStationModeDisconnected& evt) {
	taskConnectWiFi.resume();
	COUNT_FLASH = 500;
	COUNT_BLINK = 500;
	//attachInterrupt(digitalPinToInterrupt(PWR_SW), powerSwitchInterrupt, RISING);
}



