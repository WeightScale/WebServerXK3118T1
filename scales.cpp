#include <Arduino.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <FS.h>
#include <ArduinoJson.h>
#include "Scales.h"
#include "DateTime.h"
#include "BrowserServer.h"


//ScalesClass SCALES(D1,D2);								/* АЦП весов. gpio5 gpio04  */
ScalesClass SCALES;
//ScalesClass SCALES(D0,D3);							/* АЦП весов. gpio16 gpio0  */

ScalesClass::ScalesClass() {	
}

ScalesClass::~ScalesClass(){}

void ScalesClass::begin(){	
	ScaleMemClass::init();
	//loadAuth();	
	loadSettings();
	loadPortValue();
	Serial.flush();
	Serial.begin(constrain(_settings.speed, 600, 115200));
	//Serial.setTimeout(100);
	#if defined SERIAL_DEDUG
		Serial.println();
		Serial.print("Configuring access point...");
	#endif		
}

void ScalesClass::setSSID(const String& ssid){
	_settings.scaleWlanSSID = ssid;
	//ssid.toCharArray(_settings.ssid, sizeof(_settings.ssid));
}

void ScalesClass::setPASS(const String& pass){
	_settings.scaleWlanKey = pass;
	//pass.toCharArray(_settings.key, sizeof(_settings.key));
}

bool ScalesClass::saveEvent(const String& event, const String& value) {
	File readFile = SPIFFS.open("/events.json", "r");
    if (!readFile) {        
        readFile.close();
        return false;
    }
	String date = getDateTime();
    size_t size = readFile.size(); 
	
	bool flag = WiFi.status() == WL_CONNECTED?SCALES.eventToServer(date, event, value):false;
	
    std::unique_ptr<char[]> buf(new char[size]);
    readFile.readBytes(buf.get(), size);	
    readFile.close();
	
	//StaticJsonBuffer<JSON_OBJECT_SIZE(3)> jsonBuffer;	
    DynamicJsonBuffer jsonBuffer(JSON_ARRAY_SIZE(110));
    //StaticJsonBuffer<256> jsonBuffer;
    JsonObject& json = jsonBuffer.parseObject(buf.get());

    if (!json.success()) {
        return false;
    }
	long n = json["cur_num"];
	
	json["events"][n]["date"] = date;
    //json["events"][n]["type"] = event;
	json["events"][n]["value"] = value;	
	json["events"][n]["server"] = flag;
		
	if ( ++n == json["events"].size()){
		n = 0;
	}
	json["max_events"] = json["events"].size();
	json["cur_num"] = n;
	//TODO add AP data to html
	File saveFile = SPIFFS.open("/events.json", "w");
	if (!saveFile) {
		saveFile.close();
		return false;
	}

	json.printTo(saveFile);
	saveFile.flush();
	saveFile.close();
	return true;
}

/*
bool ScalesClass::pingServer(){
	HTTPClient http;
	http.begin("http://viktyusk.dp.ua/scales.php?ping");
	http.setTimeout(500);
	int httpCode = http.GET();
	http.end();
	if(httpCode == HTTP_CODE_OK) {
		return true;
	}
	return false;	
}
*/

/*
	Зарегистрировать нового пользователя по почте:
	http://viktyusk.dp.ua/scales.php?email=<почта>
	Создать новые весы:Название весов - любой текст, просто для удобства. После создания новых весов отобразится уникальный код для доступа к ним.
	http://viktyusk.dp.ua/scales.php?email=<почта>&password=<пароль>&name=<название_весов>
	Добавить событие (code - код весов, date - дата события, type - тип события, value - значение события):
	http://viktyusk.dp.ua/scales.php?code=...&date=...&type=...&value=...
	Получить события (code - код весов, size - размер страницы, page - номер страницы (начиная с нуля)):
	http://viktyusk.dp.ua/scales.php?code=...&size=...&page=...	
	Если потерялся код весов, можно посмотреть его в списке весов пользователя:
	http://viktyusk.dp.ua/scales.php?email=<почта>&password=<пароль>
	Если потерялся сам пароль пользователя, то его можно восстановить так:
	http://viktyusk.dp.ua/scales.php?email=<почта>
*/

/** Отправка события на сервер. */	
bool ScalesClass::eventToServer(const String& date, const String& type, const String& value){
	HTTPClient http;
	String message = "http://";
	message += _settings.hostUrl.c_str();
	String hash = getHash(_settings.hostPin.c_str(), date, type, value);
	//Serial.println("Hash="+hash);
/*
	message += "?code=" + _settings.hostPin + "&";
	message += "date=" + date + "&";
	message += "type=" + type + "&";
	message += "value=" + value;
*/
	message += "/scales.php?hash=" + hash;
	http.begin(message);
	http.setTimeout(3000);
	int httpCode = http.GET();
	http.end();
	if(httpCode == HTTP_CODE_OK) {
		return true;
	}
	return false;
}
/*! Получить настройки весов которые отправил клиент */
void ScalesClass::getScaleSettingsValue() {	
	bool flag = false;
	for (uint8_t i = 0; i < browserServer.args(); i++) {
		if (browserServer.argName(i)=="date"){
			DateTimeClass DateTime(browserServer.arg("date"));
			Rtc.SetDateTime(DateTime.toRtcDateTime());
			String message = "<div>Дата синхронизирована<br/>";
			message+=getDateTime()+"</div>";
			browserServer.send(200, "text/html", message);
			return;
		}
		if (browserServer.argName(i) == "host") {
			_settings.hostUrl = browserServer.urldecode(browserServer.arg(i));
			flag = true;
			//continue;
		}else if (browserServer.argName(i) == "pin") {
			_settings.hostPin = browserServer.urldecode(browserServer.arg(i));
			flag = true;
			//continue;
		}else if (browserServer.argName(i) == "name_admin") {
			_settings.scaleName = browserServer.urldecode(browserServer.arg(i));
			flag = true;
			//continue;
		}else if (browserServer.argName(i) == "pass_admin") {
			_settings.scalePass = browserServer.urldecode(browserServer.arg(i));
			flag = true;
			//continue;
		}else if (browserServer.argName(i) == "ssid") {
			_settings.scaleWlanSSID = browserServer.urldecode(browserServer.arg(i));
			flag = true;
			//continue;
		}else if (browserServer.argName(i) == "key") {
			_settings.scaleWlanKey = browserServer.urldecode(browserServer.arg(i));
			connect = _settings.scaleWlanKey.length() > 0;
			flag = true;
			//continue;
		}
	}
	if(flag){
		if (saveSettings()){
			handleFileRead(browserServer.uri());
		}else{
			browserServer.send(400, "text/html", "Ошибка записи");
		}
	}
	
}
/*! Получаем значения отправленые клиентом */
bool ScalesClass::getPortValue() {
	//if (browserServer.args() > 0){  // Save Settings
	bool flag = false;
	for (uint8_t i = 0; i < browserServer.args(); i++) {
		if (browserServer.argName(i) == "speed") {
			_settings.speed = browserServer.arg(i).toInt();
			Serial.flush();
			Serial.begin(_settings.speed);
			flag = true;
		}if (browserServer.argName(i) == "lengthWord") {
			_settings.lengthWord = browserServer.arg(i).toInt();
			flag = true;
		}if (browserServer.argName(i) == "numberSigns") {
			_settings.numberSigns = browserServer.arg(i).toInt();
			flag = true;
		}if (browserServer.argName(i) == "endSymbol") {
			_settings.endSymbol = char(browserServer.arg(i).toInt());
			flag = true;
		}if (browserServer.argName(i) == "accuracy") {
			_settings.accuracy = browserServer.arg(i).toInt();
			flag = true;
		}
	}
	if (browserServer.hasArg("update")){
		SCALES.savePortValue();		
	}
	return flag;
}

String ScalesClass::getHash(const String& code, const String& date, const String& type, const String& value){
	
	String event = String(code);
	event += "\t" + date + "\t" + type + "\t" + value;
	int s = 0;
	for (int i = 0; i < event.length(); i++)
		s += event[i];
	event += (char) (s % 256);
	String hash = "";
	for (int i = 0; i < event.length(); i++){
		int c = (event[i] - (i == 0? 0 : event[i - 1]) + 256) % 256;
		int c1 = c / 16; int c2 = c % 16;
		char d1 = c1 < 10? '0' + c1 : 'a' + c1 - 10;
		char d2 = c2 < 10? '0' + c2 : 'a' + c2 - 10;
		hash += "%"; hash += d1; hash += d2;
	} 
	return hash;
}

/*
void ScalesClass::sendServerAuthSaveValue() {
	if (browserServer.args() > 0){  // Save Settings
		bool flag = false;
		for (uint8_t i = 0; i < browserServer.args(); i++) {
			if (browserServer.argName(i) == "host") {
				_auth.host = browserServer.urldecode(browserServer.arg(i));
				flag = true;
				continue;
			}
			if (browserServer.argName(i) == "email") {
				_auth.email = browserServer.urldecode(browserServer.arg(i));
				flag = true;
				continue;
			}
			if (browserServer.argName(i) == "password") {
				_auth.password = browserServer.urldecode(browserServer.arg(i));
				flag = true;
				continue;
			}
			if (browserServer.argName(i) == "pin") {
				_auth.pin = browserServer.urldecode(browserServer.arg(i));
				flag = true;
				continue;
			}
		}
		if(flag){
			if (saveAuth()){

				browserServer.send(200, "text/html", "");

			}else{

				browserServer.send(400, "text/html", "Ошибка записи");

			}	
		}				 
	}	
}*/

/*
void ScalesClass::sendServerAuthValues() {
	String values = "";

	values += "id_host|" + (String)_auth.host  + "|input\n";
	values += "id_email|" + (String)_auth.email + "|input\n";
	values += "id_password|" + (String)_auth.password + "|input\n";
	values += "id_pin|" + (String)_auth.pin + "|input\n";

	browserServer.send(200, "text/plain", values);
}*/

/*
JsonObject &ScalesClass::openJsonFile(const String& path){
	File readFile = SPIFFS.open(path, "r");
	if (!readFile) {
		readFile.close();
		DynamicJsonBuffer jsonBuffer(256);
		//StaticJsonBuffer<256> jsonBuffer;
		return jsonBuffer.createObject();
	}
	size_t size = readFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	readFile.readBytes(buf.get(), size);
	readFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	//StaticJsonBuffer<256> jsonBuffer;
	return jsonBuffer.parseObject(buf.get());;	
}*/

/*
bool ScalesClass::saveAuth() {
	File readFile = SPIFFS.open(SERVER_FILE, "r");

	if (!readFile) {

		readFile.close();

		return false;

	}

	size_t size = readFile.size();

	std::unique_ptr<char[]> buf(new char[size]);

	readFile.readBytes(buf.get(), size);

	readFile.close();

	DynamicJsonBuffer jsonBuffer(size);

	//StaticJsonBuffer<256> jsonBuffer;

	JsonObject& json = jsonBuffer.parseObject(buf.get());
	//JsonObject& json = openJsonFile(SERVER_FILE);

	if (!json.success()) {
		return false;
	}	
	
	json["server"]["host"] = _auth.host;
	json["server"]["email"] = _auth.email;
	json["server"]["password"] = _auth.password;
	json["server"]["pin"] = _auth.pin;

	//TODO add AP data to html
	File serverFile = SPIFFS.open(SERVER_FILE, "w");
	if (!serverFile) {
		serverFile.close();
		return false;
	}

	json.printTo(serverFile);
	serverFile.flush();
	serverFile.close();
	return true;
}*/

/*
bool ScalesClass::loadAuth() {
	File serverFile = SPIFFS.open(SERVER_FILE, "r");
	if (!serverFile) {
		_auth.host = "";
		_auth.email = "";
		_auth.password = "";
		_auth.pin = "";
		serverFile.close();
		return false;
	}

	size_t size = serverFile.size();

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	serverFile.readBytes(buf.get(), size);
	serverFile.close();
	DynamicJsonBuffer jsonBuffer(256);
	//StaticJsonBuffer<256> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) {
		return false;
	}
	_auth.host = json["server"]["host"].asString();
	_auth.email = json["server"]["email"].asString();
	_auth.password = json["server"]["password"].asString();
	_auth.pin = json["server"]["pin"].asString();
	return true;
}*/

bool ScalesClass::saveSettings() {
	File readFile = SPIFFS.open(SETTINGS_FILE, "r");
	if (!readFile) {
		readFile.close();
		return false;
	}
	size_t size = readFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	readFile.readBytes(buf.get(), size);
	readFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	//StaticJsonBuffer<256> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());
	//JsonObject& json = openJsonFile(SERVER_FILE);

	if (!json.success()) {
		return false;
	}
	
	json["scale"]["id_name_admin"] = _settings.scaleName;
	json["scale"]["id_pass_admin"] = _settings.scalePass;
	json["scale"]["id_ssid"] = _settings.scaleWlanSSID;
	json["scale"]["id_key"] = _settings.scaleWlanKey;
	
	json["server"]["id_host"] = _settings.hostUrl;
	json["server"]["id_pin"] = _settings.hostPin;

	//TODO add AP data to html
	File serverFile = SPIFFS.open(SETTINGS_FILE, "w");
	if (!serverFile) {
		serverFile.close();
		return false;
	}

	json.printTo(serverFile);
	serverFile.flush();
	serverFile.close();
	return true;
}

bool ScalesClass::loadSettings() {
	File serverFile = SPIFFS.open(SETTINGS_FILE, "r");
	if (!serverFile) {
		_settings.scaleName = "admin";
		_settings.scalePass = "admin";
		_settings.scaleWlanSSID = "";
		_settings.scaleWlanKey = "";
		_settings.hostUrl = "";
		_settings.hostPin = "";
		serverFile.close();
		return false;
	}

	size_t size = serverFile.size();

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	serverFile.readBytes(buf.get(), size);
	serverFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	//StaticJsonBuffer<256> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) {
		_settings.scaleName = "admin";
		_settings.scalePass = "admin";
		return false;
	}
	_settings.scaleName = json["scale"]["id_name_admin"].asString();
	_settings.scalePass = json["scale"]["id_pass_admin"].asString();
	_settings.scaleWlanSSID = json["scale"]["id_ssid"].asString();
	_settings.scaleWlanKey = json["scale"]["id_key"].asString();
	_settings.hostUrl = json["server"]["id_host"].asString();
	_settings.hostPin = json["server"]["id_pin"].asString();
	return true;
}

bool ScalesClass::savePortValue() {
	File readFile = SPIFFS.open(SETTINGS_FILE, "r");
	if (!readFile) {
		readFile.close();
		return false;
	}
	size_t size = readFile.size();
	std::unique_ptr<char[]> buf(new char[size]);
	readFile.readBytes(buf.get(), size);
	readFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	//StaticJsonBuffer<256> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());
	//JsonObject& json = openJsonFile(SERVER_FILE);

	if (!json.success()) {
		return false;
	}
	
	json["port"]["speed_id"] = _settings.speed;
	json["port"]["length_word_id"] = _settings.lengthWord;
	json["port"]["number_signs_id"] = _settings.numberSigns;
	json["port"]["end_symbol_id"] = _settings.endSymbol;
	json["port"]["accuracy_id"] = _settings.accuracy;

	//TODO add AP data to html
	File saveFile = SPIFFS.open(SETTINGS_FILE, "w");
	if (!saveFile) {
		saveFile.close();
		return false;
	}

	json.printTo(saveFile);
	saveFile.flush();
	saveFile.close();
	return true;
}

bool ScalesClass::loadPortValue() {
	File serverFile = SPIFFS.open(SETTINGS_FILE, "r");
	if (!serverFile) {
		_settings.speed = 9600;
		_settings.lengthWord = 12;
		_settings.numberSigns = 7;
		_settings.endSymbol = '(';
		_settings.accuracy = 1;
		serverFile.close();
		return false;
	}

	size_t size = serverFile.size();

	// Allocate a buffer to store contents of the file.
	std::unique_ptr<char[]> buf(new char[size]);

	// We don't use String here because ArduinoJson library requires the input
	// buffer to be mutable. If you don't use ArduinoJson, you may as well
	// use configFile.readString instead.
	serverFile.readBytes(buf.get(), size);
	serverFile.close();
	DynamicJsonBuffer jsonBuffer(size);
	//StaticJsonBuffer<256> jsonBuffer;
	JsonObject& json = jsonBuffer.parseObject(buf.get());

	if (!json.success()) {		
		return false;
	}
	_settings.speed = json["port"]["speed_id"];
	_settings.lengthWord = json["port"]["length_word_id"];
	_settings.numberSigns = json["port"]["number_signs_id"];
	_settings.endSymbol = char(json["port"]["end_symbol_id"]);
	_settings.accuracy = json["port"]["accuracy_id"];
	return true;
}

String ScalesClass::getIp() {
	WiFiClient client ;
	String ip = "";
	if (client.connect("api.ipify.org", 80)) {
		client.println("GET / HTTP/1.0");
		client.println("Host: api.ipify.org");
		client.println();
		delay(50);
		int a=1;
		while (client.connected()) {
			++a;
			String line = client.readStringUntil('\n');
			if (a == 11) {
				ip = line;
				client.stop();
			}
		}
	}
	client.stop();
	return ip;
}

int ScalesClass::getBattery(int times){
	long sum = 0;
	for (byte i = 0; i < times; i++) {
		sum += analogRead(A0);
	}
	return times == 0?sum :sum / times;
}

void ScalesClass::parseDate(String str){
	int len = str.length();	 
	if(len > _settings.lengthWord){		
		_weight = str.substring(str.indexOf(_settings.endSymbol)-_settings.numberSigns, str.indexOf(_settings.endSymbol)).toFloat();
		detectStable();	
	}		
}

void ScalesClass::detectStable(){
	if (_weight_temp - STABLE_DELTA_STEP <= _weight && _weight_temp + STABLE_DELTA_STEP >= _weight && _weight != 0) {
		if (_stable_num <= STABLE_NUM_MAX){
			if (_stable_num == STABLE_NUM_MAX) {
				if (!isStable){
					saveEvent("weight", String(_weight)+"_kg");
					isStable = true;
				}
				return;
			}
			_stable_num++;
		}
	} else {
		_stable_num=0;
		isStable = false;
	}
	_weight_temp = _weight;
}

void powerOff(){
	digitalWrite(EN_NCP, LOW); /// Выключаем стабилизатор
	ESP.reset();
}
