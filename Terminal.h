
#ifndef _TERMINAL_h
#define _TERMINAL_h
#include <Arduino.h>
#include "BrowserServer.h"

#define TERMINAL_FILE "/terminal.json"
#define TERMINAL_TERMINAL_JSON "trm"

class TerminalClass  {
	protected:		
		String _w;
		float _weight;
	private:
	
	
	public:	
		TerminalClass(){};
		~TerminalClass();	
		virtual void handlePort()=0;
		virtual String getName()=0;
		virtual bool saveValueHttp(BrowserServerClass *s)=0;
		float getWeight(){
			return _weight;	
		};
};

//extern TerminalClass *Terminal;

#endif

