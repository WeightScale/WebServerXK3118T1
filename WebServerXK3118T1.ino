//#define SERIAL_DEDUG

#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <Arduino.h>

#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "DateTime.h"
#include "XK3118T1.h"
#include "Terminal.h"
#include "ScaleMem.h"
#include "tools.h"
#include "BrowserServer.h" 
#include "Scales.h"

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

// Web server
//ESP8266WebServer server(80);
//int recPin = RX;
//long baudRate;   // global in case useful elsewhere in a sketch

/** Last time I tried to connect to WLAN */
long lastConnectTry = 0;

/** Current WLAN status */
int status = WL_IDLE_STATUS;

void setup() {
	
	Serial.begin(9600);
	//Serial.setTimeout(100);
	#if defined SERIAL_DEDUG
		Serial.println();
		Serial.print("Configuring access point...");
	#endif	
	delay(1000);
	SPIFFS.begin(); // Not really needed, checked inside library and started if needed
	
	SCALES.begin();
	
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
	browserServer.begin(); 
  
	connect = SCALES.getSSID().length() > 0 /*strlen(SCALES.getSSID()) > 0*/; // Request WLAN connect if there is a SSID
	Rtc.Begin();
	SCALES.saveEvent("weight", "ON");	
}

void connectWifi() {
	#if defined SERIAL_DEDUG
		Serial.println("Connecting as wifi client...");
	#endif

	WiFi.disconnect();
	/*!  */
	int n = WiFi.scanNetworks();
	if (n == 0)
	return;
	else{
		for (int i = 0; i < n; ++i)	{
			/*!  */
			if(WiFi.SSID(i) == SCALES.getSSID().c_str()){
				WiFi.begin ( SCALES.getSSID().c_str(), SCALES.getPASS().c_str());
				int connRes = WiFi.waitForConnectResult();
				#if defined SERIAL_DEDUG
					Serial.print ( "connRes: " );
					Serial.println ( connRes );
				#endif
				break;
			}
		}
	}
}

void loop() {
	//ArduinoOTA.handle();
	if (connect) {
		#if defined SERIAL_DEDUG
			Serial.println ( "Connect requested" );
		#endif
		connect = false;
		connectWifi();
		lastConnectTry = millis();
	}
	{
		int s = WiFi.status();
		if (s == 0 && millis() > (lastConnectTry + 60000) ) {
			/* If WLAN disconnected and idle try to connect */
			/* Don't set retry time too low as retry interfere the softAP operation */
			connect = true;
		}
		if (status != s) { // WLAN status change
			#if defined SERIAL_DEDUG
				Serial.print ( "Status: " );
				Serial.println ( s );
			#endif			
			status = s;
			if (s == WL_CONNECTED) {
				/* Just connected to WLAN */
				#if defined SERIAL_DEDUG
					Serial.println ( "" );
					Serial.print ( "Connected to " );
					Serial.println ( SCALES.getSSID() );
					Serial.print ( "IP address: " );
					Serial.println ( WiFi.localIP() );
				#endif	
				// Setup MDNS responder
				if (!MDNS.begin(myHostname)) {
					#if defined SERIAL_DEDUG
						Serial.println("Error setting up MDNS responder!");
					#endif					
				} else {
					#if defined SERIAL_DEDUG
						Serial.println("mDNS responder started");
					#endif					
					// Add service to MDNS-SD
					MDNS.addService("http", "tcp", 80);
				}
				//SCALES.saveEvent("net", "WLAN");
				SCALES.saveEvent("ip", SCALES.getIp());
			} else if (s == WL_NO_SSID_AVAIL) {
				WiFi.disconnect();
			}
		}
	}
	// Do work:
	//DNS
	dnsServer.processNextRequest();
	//HTTP
	browserServer.handleClient();	
	//XK3118T1.setWeight("25");
	//int a = Serial.available();
	if (Serial.available()) {		
		XK3118T1.buildCommand();
	}
}

