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
using namespace ArduinoJson;

#define SETTINGS_FILE "/settings.json"

typedef struct {
	String scaleName;
	String scalePass;
	String scaleWlanSSID;
	String scaleWlanKey;
	String hostUrl;
	//String hostEmail;
	//String hostPass;
	String hostPin;
} settings_t;

class ScalesClass : public ScaleMemClass{
	protected:	
	settings_t _settings;
	bool saveAuth();
	bool loadAuth();
	bool saveSettings();	
	bool loadSettings();	

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
		void sendScaleSettingsSaveValue();
		String getHash(const String&, const String&, const String&, const String&);
		String getIp();
		//void sendServerAuthSaveValue();
		//void sendServerAuthValues();
		//JsonObject &openJsonFile(const String&);
		
		friend ScaleMemClass;		
};

extern ScalesClass SCALES;

#endif

