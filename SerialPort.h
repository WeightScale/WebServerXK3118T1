// scales.h

#ifndef _SCALES_h
#define _SCALES_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include "ScaleMem.h"
#include "TaskController.h"
#include "Task.h"
#include "BrowserServer.h"
//using namespace ArduinoJson;

#define PORT_FILE		"/port.json"
#define PORT_TERMINAL_JSON	"tr_id"
#define PORT_SPEED_JSON		"sp_id"
#define PORT_ACCURACY_JSON	"ac_id"
#define PORT_USER_JSON		"us_id"
#define PORT_PASS_JSON		"ps_id"

//#define STABLE_NUM_MAX		100
#define STABLE_DELTA_STEP	10


//#define EN_NCP  12							/* сигнал включения питания  */
//#define PWR_SW  13							/* сигнал от кнопки питания */
//#define LED  2								/* индикатор работы */

#define CR						0xd
#define LF						0xa
#define szCR_LF					"\x0D\x0A"

//setport.html
const char setport_html[] PROGMEM = R"(<!DOCTYPE html><html lang="en"><head> <meta charset="UTF-8"> <meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/> <meta http-equiv="Cache-Control" content="no-cache, no-store, must-revalidate"/> <meta http-equiv="Pragma" content="no-cache"/> <title>Настройки данных</title> <link rel="stylesheet" type="text/css" href="global.css"> <script>var terminal; function GetTerminals(){var http_request=new XMLHttpRequest(); http_request.overrideMimeType('application/json'); http_request.onreadystatechange=function(){if (http_request.readyState===4 ){var json=JSON.parse(http_request.responseText); terminal=json["trm"]; var select=document.getElementById("tr_id"); for (var entry in terminal){var option=document.createElement('option'); option.value=entry; option.textContent=terminal[entry].name; select.appendChild(option);}GetSettings();}}; http_request.open("GET", "/terminal.json", true); http_request.send(null);}function GetSettings(){var http_request=new XMLHttpRequest(); http_request.overrideMimeType('application/json'); http_request.onreadystatechange=function(){if (http_request.readyState===4 ){var json=JSON.parse(http_request.responseText); for (entry in json){if(document.getElementById(entry)!==null) document.getElementById(entry).value=json[entry];}var t=parseInt(document.getElementById("tr_id").value); var u=terminal[t].page; var a=document.getElementById("url_id"); a.href=u; document.body.style.visibility='visible';}}; http_request.open("GET", "/port.json", true); http_request.send(null);}window.onload=function (){GetTerminals();}; function saveValue(f){var form=document.getElementById(f); var formData=new FormData(form); var http_request=new XMLHttpRequest(); http_request.onreadystatechange=function(){if (this.readyState===4 && this.status===200){if (this.responseText !==null){window.open("/","_self");}}}; http_request.onerror=function(){alert("Ошибка: " + this.src);}; http_request.open('POST','setport.html',true); http_request.send(formData);}; function setUrl(i){document.getElementById("url_id").href=terminal[i].page;}</script></head><body style="visibility: hidden"><a href="/" class="btn btn--s btn--blue">&lt;</a>&nbsp;&nbsp;<strong>Настройка данных</strong><hr><fieldset id="form_max" style="visibility: visible"> <legend>Общии настройки</legend> <form id="form_id" method='POST' > <table> <tr> <td>Скорость порта</td><td> <select id="sp_id" name="spd" title="Выбор скорости COM порта" style="width: 100%"> <option name="600" value="600"> 600 </option> <option name="1200" value="1200"> 1200 </option> <option name="2400" value="2400"> 2400 </option> <option name="4800" value="4800"> 4800 </option> <option name="9600" value="9600"> 9600 </option> <option name="19200" value="19200"> 19200 </option> <option name="38400" value="38400"> 38400 </option> <option name="115200" value="115200"> 115200 </option> </select> </td></tr><tr> <td>Точность измерения</td><td> <select id="ac_id" name="acr" title="Введите шаг измерения" style="width: 100%"> <option name="0" value="0"> 0 </option> <option name="0.0" value="1"> 0.0 </option> <option name="0.00" value="2"> 0.00 </option> <option name="0.000" value="3"> 0.000 </option> </select> </td></tr><tr> <td><a id="url_id">Терминал</a></td><td> <select id="tr_id" name="trm" onchange="document.getElementById('url_id').href=terminal[this.selectedIndex].page;"></select> </td></tr></table> <a href="javascript:saveValue('form_id');">сохранить и выйти</a> </form></fieldset><fieldset> <details><summary>Авторизация для терминала</summary> <form method='POST'> <table> <tr> <td>ИМЯ:</td><td> <input id='us_id' name='user' placeholder='имя админ'> </td></tr><tr> <td>ПАРОЛЬ:</td><td><input type='password' id='ps_id' name='pass' placeholder='пароль админ'></td></tr><tr> <td></td><td> <input type='submit' value='СОХРАНИТЬ'/></td></tr></table> </form> </details></fieldset><hr><footer align="center">2018 © Powered by www.scale.in.ua</footer></body></html>)";


typedef struct {
	unsigned long speed;
	unsigned char terminal;	
	int accuracy;
	String user;
	String password;	
} serial_port_t;

class SerialPortClass : public HardwareSerial/*, public ScaleMemClass*/{
	protected:
		BrowserServerClass *_server;
		bool _authenticated;
		bool stableWeight;	
		serial_port_t _port;
		double _weight;
		//String _s_weight = "***";
		bool saveAuth();
		bool loadAuth();
	
		bool _downloadPort();
		void _saveValuePortHttp(AsyncWebServerRequest * request);
	
		//bool loadPortValue();		

	public:			
		SerialPortClass(int port);
		virtual ~SerialPortClass(){};
		void init();	
		//virtual void handlePort()=0;	
		void setup(BrowserServerClass *server);
		void getScaleSettingsValue();
		bool savePort();	
		int getAccuracy(){return _port.accuracy;};	
		double getWeight(){return _weight;};
		BrowserServerClass *getServer(){ return _server;};
		void setStableWeight(bool s){stableWeight = s;};
		bool getStableWeight(){return stableWeight;};
};

void handlePortSave();
void handleValueTerminal(AsyncWebServerRequest * request);
//void powerOff();

extern SerialPortClass SerialPort;
extern TaskController taskController;				/*  */
extern Task taskBlink;								/*  */
extern Task taskBattery;							/*  */
//extern Task taskPower;

#endif

