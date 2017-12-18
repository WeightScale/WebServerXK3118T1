// A12E.h

#ifndef _A12E_h
#define _A12E_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Terminal.h"

class XK3118T1Class : public TerminalClass{
	protected:
		//String weight="---";
		bool isFloat = false;		

	public:
		XK3118T1Class(String n) : TerminalClass(n){};
		~XK3118T1Class();
		void init();
		d_type getWeight(){return _weight;};
		//void setWeight(const String& s){ weight = s;}
		void parseDate(String);
		void buildCommand();
		void tape();
		void zero();
		void reload();
		void reverse(char *);
		int string_length(char *);
};

long detRate(int recpin);

extern XK3118T1Class XK3118T1;

#endif

