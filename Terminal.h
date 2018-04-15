
#ifndef _TERMINAL_h
#define _TERMINAL_h
#include <Arduino.h>
#include "BrowserServer.h"
#include "SerialPort.h"

#define TERMINAL_FILE "/terminal.json"
#define TERMINAL_TERMINAL_JSON "trm"

class TerminalClass  {
	protected:
		String _name;		
		String _w;
		float _weight;
	private:
	
	
	public:	
		TerminalClass(String n): _name(n){};
		~TerminalClass();
		String getName(){return _name;};	
		virtual void handlePort()=0;
		virtual bool saveValueHttp(AsyncWebServerRequest * request)=0;
		virtual bool downloadValue(int)=0;
		virtual size_t doData(JsonObject& json );			
		float getWeight(){return _weight;};
		virtual void formatValue(char* string){
			dtostrf(_weight, 6-SerialPort.getAccuracy(), SerialPort.getAccuracy(), string);
		};
};

//extern TerminalClass *Terminal;

#endif

