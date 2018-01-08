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
using namespace ArduinoJson;

#define SETTINGS_FILE "/settings.json"
#define STABLE_NUM_MAX 100
#define STABLE_DELTA_STEP 10
#define MAX_CHG 1018
#define MIN_CHG 720

#define EN_NCP  12							/* сигнал включения питания  */
#define PWR_SW  13							/* сигнал от кнопки питания */
#define LED  2								/* индикатор работы */

#define CR						0xd
#define LF						0xa
#define szCR_LF					"\x0D\x0A"

typedef double d_type;

typedef struct {
	unsigned long speed;
	unsigned char lengthWord;
	unsigned char numberSigns;
	char endSymbol;
	int accuracy;
	String scaleName;
	String scalePass;
	String scaleWlanSSID;
	String scaleWlanKey;
	String hostUrl;
	String hostPin;
} settings_t;

class ScalesClass : public ScaleMemClass{
	protected:	
	settings_t _settings;
	unsigned int charge;
	d_type _weight=-1, _weight_temp;
	unsigned char _stable_num = 0;
	bool isStable = false;
	bool saveAuth();
	bool loadAuth();
	bool saveSettings();
	bool loadSettings();
	bool savePortValue();
	bool loadPortValue();		

	public:			
		ScalesClass();
		~ScalesClass();
		void begin();
		String& getNameAdmin(){return _settings.scaleName;};
		String& getPassAdmin(){return _settings.scalePass;};
		String& getSSID(){return _settings.scaleWlanSSID;};
		void setSSID(const String&);
		String& getPASS(){return _settings.scaleWlanKey;};
		void setPASS(const String&);
		bool saveEvent(const String&, const String&);
		//bool pingServer();
		bool eventToServer(const String&, const String&, const String&);
		void getScaleSettingsValue();
		bool getPortValue();
		String getHash(const String&, const String&, const String&, const String&);
		String getIp();
		//unsigned char getLengthWord(){return _settings.lengthWord;};
		//unsigned char getNumberSigns(){return _settings.numberSigns;};
		//char getEndSymbol(){return _settings.endSymbol;};
		int getAccuracy(){return _settings.accuracy;};
		void setCharge(unsigned int ch){charge = ch;}
		unsigned int getCharge(){return charge;}	
		int getBattery(int);
		void parseDate(String);
		void detectStable();
		d_type getWeight(){return _weight;};	
		//void sendServerAuthSaveValue();
		//void sendServerAuthValues();
		//JsonObject &openJsonFile(const String&);
		
		friend ScaleMemClass;		
};

void powerOff();
extern ScalesClass SCALES;
extern TaskController taskController;				/*  */
extern Task taskBlink;								/*  */
extern Task taskBattery;							/*  */
extern Task taskPower;

#endif

