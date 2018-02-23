#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "SerialPort.h"
#include "DateTime.h"
#include "BrowserServer.h"
#include "Terminals.h"

SerialPortClass SerialPort(UART0);


SerialPortClass::SerialPortClass(int port) : HardwareSerial(port) {
	_server = NULL;
	_username = NULL;
	_password = NULL;
	_authenticated = false;	
}

//SerialPortClass::~SerialPortClass(){}
	
void SerialPortClass::init(){
	flush();
	begin(constrain(_port.speed, 600, 9600));
	TerminalController.identify(_port.terminal);
}

void SerialPortClass::setup(BrowserServerClass *server, const char * username, const char * password){	
	//ScaleMemClass::init();
	_server = server;
	_username = (char *)username;
	_password = (char *)password;
	_downloadPort();
	init();
	//Serial.setTimeout(100);
	_server->on("/wt", [&](){
		char buffer[10];
		float w = TerminalController.getCurrent()->getWeight();
		dtostrf(w, 6-getAccuracy(), getAccuracy(), buffer);
		_server->send(200, "text/plain", String("{\"w\":\""+String(buffer)+"\",\"c\":"+String(CORE.getCharge())+",\"s\":"+String(SerialPort.getStableWeight())+"}"));
		CORE.detectStable(w);	
		taskPower.updateCache();
	});
	_server->on("/setport.html",HTTP_POST, [&]() {
		if (!_server->isAuthentified())
			return _server->requestAuthentication();
		_saveValuePortHttp();
		taskPower.updateCache();
	});
	_server->on("/trm",HTTP_POST, handleValueTerminal);
	/*_server->on("/trm",[&](){
			String message = "";
			for (int i; i<TERMINAL_MAX; i++){
				message += String(TerminalController.getIndexOf(i)->getName()) + " " + String(i) + "\n";
			}
			_server->send(200,"text/html","ok");
	});*/
	
	/*_server->on("/trm",HTTP_GET,[&](){
			if(_server->hasArg("trm")){				
				TerminalController.identify(_server->arg("trm").toInt());
				_port.terminal = TerminalController.getIndex();
				savePort();				
				return _server->send(200,"text/html","terminal " + TerminalController.getCurrent()->getName());
			}
			_server->send(400, "text/html", "Error");
	});*/		
}

/*! Получаем значения отправленые клиентом */
void SerialPortClass::_saveValuePortHttp() {
	//if (browserServer.args() > 0){  // Save Settings
	if (_server->hasArg("spd")){
		_port.terminal = _server->arg("trm").toInt();
		_port.speed = _server->arg("spd").toInt();
		_port.accuracy = _server->arg("acr").toInt();
		flush();
		begin(_port.speed);
		TerminalController.identify(_port.terminal);
	}
	
	if (savePort()){		
		return _server->send(200, "text/html", "");
	}
	_server->send(400, "text/html", "Error");
}

bool SerialPortClass::savePort() {
	File portFile = SPIFFS.open(PORT_FILE, "w+");
	if (!portFile) {
		portFile.close();
		return false;
	}
	
	DynamicJsonBuffer jsonBuffer;
	JsonObject& json = jsonBuffer.createObject();

	if (!json.success()) {
		return false;
	}
	
	json[PORT_TERMINAL_JSON] = _port.terminal;
	json[PORT_SPEED_JSON]	= _port.speed;	
	json[PORT_ACCURACY_JSON] = _port.accuracy;

	json.printTo(portFile);
	portFile.flush();
	portFile.close();
	return true;	
}

bool SerialPortClass::_downloadPort() {
	_port.terminal = 0;
	_port.speed = 9600;	
	_port.accuracy = 1;
	File portFile;
	if (SPIFFS.exists(PORT_FILE)){
		portFile = SPIFFS.open(PORT_FILE, "r");
	}else{
		portFile = SPIFFS.open(PORT_FILE, "w+");
	}
	if (!portFile) {
		portFile.close();
		return false;
	}	

	size_t size = portFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	portFile.readBytes(buf.get(), size);
	portFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	JsonObject& json = jsonBuffer.parseObject(buf.get());
	
	if (!json.success()) {		
		return false;
	}
	
	_port.terminal = json[PORT_TERMINAL_JSON];
	_port.speed = json[PORT_SPEED_JSON];	
	_port.accuracy = json[PORT_ACCURACY_JSON];
	return true;
}

void handleValueTerminal(){
	BrowserServerClass *server = SerialPort.getServer();
	if (!server->isAuthentified())
		return server->requestAuthentication();
	if (server->args() > 0){
		if(TerminalController.getCurrent()->saveValueHttp(server))
			return server->send(200, TEXT_HTML, "");		
	}
	//server->send(400, TEXT_HTML, "");
}

/*
void SerialPortClass::strrev(char *string){
	
	int len = 0;
	while (string[len])
		len++;

	int down = 0;
	int up = len - 1;

	while (up > down){
		char c = string[down];
		string[down++] = string[up];
		string[up--] = c;
	}
}*/

/*
void SerialPortClass::reverse(char *string){
	int length, c;
	char *begin, *end, temp;
	
	length = string_length(string);
	begin  = string;
	end    = string;
	
	for (c = 0; c < length - 1; c++)
	end++;
	
	for (c = 0; c < length/2; c++){
		temp   = *end;
		*end   = *begin;
		*begin = temp;
		
		begin++;
		end--;
	}
}*/

/*
int SerialPortClass::string_length(char *pointer){
	int c = 0;
	
	while( *(pointer + c) != '\0' )
	c++;
	
	return c;
}*/

/*
void powerOff(){
	digitalWrite(EN_NCP, LOW); /// Выключаем стабилизатор
	ESP.reset();
}*/
