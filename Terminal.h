
#ifndef _TERMINAL_h
#define _TERMINAL_h
#include <Arduino.h>

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
		float getWeight(){
			return _weight;	
		};
};

//extern TerminalClass *Terminal;

#endif

