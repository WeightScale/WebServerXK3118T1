// scales.h

#ifndef _SCALES_h
#define _SCALES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <ArduinoJson.h>
#include "ScaleMem.h"
#include "TaskController.h"
#include "Task.h"
#include "BrowserServer.h"
//using namespace ArduinoJson;

#define PORT_FILE		"/port.json"
#define TERMINAL_JSON	"tr_id"
#define SPEED_JSON		"sp_id"
#define ACCURACY_JSON	"ac_id"

//#define STABLE_NUM_MAX		100
#define STABLE_DELTA_STEP	10


//#define EN_NCP  12							/* сигнал включения питания  */
//#define PWR_SW  13							/* сигнал от кнопки питания */
//#define LED  2								/* индикатор работы */

#define CR						0xd
#define LF						0xa
#define szCR_LF					"\x0D\x0A"

//typedef double d_type;

typedef struct {
	unsigned long speed;
	unsigned char terminal;	
	int accuracy;	
} serial_port_t;

class SerialPortClass : public HardwareSerial/*, public ScaleMemClass*/{
	protected:
		BrowserServerClass *_server;
		char * _username;
		char * _password;
		bool _authenticated;	
		serial_port_t _port;
		double _weight;
		//String _s_weight = "***";
		bool saveAuth();
		bool loadAuth();
	
		bool _downloadPort();
		void _saveValuePortHttp();
	
		//bool loadPortValue();		

	public:			
		SerialPortClass();
		virtual ~SerialPortClass(){};
		void init();	
		virtual void handlePort()=0;	
		void setup(BrowserServerClass *server, const char * username, const char * password);
		void getScaleSettingsValue();
		bool savePort();	
		int getAccuracy(){return _port.accuracy;};		
		void parseDate(String);
		double getWeight(){return _weight;};
		//String getWeight(){return _s_weight;};
		//friend ScaleMemClass;		
};

void handlePortSave();
//void powerOff();

//extern SerialPortClass SerialPort;
extern TaskController taskController;				/*  */
extern Task taskBlink;								/*  */
extern Task taskBattery;							/*  */
extern Task taskPower;

#endif

