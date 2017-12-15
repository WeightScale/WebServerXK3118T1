// handleHttp1.h

#ifndef _HANDLEHTTP1_h
#define _HANDLEHTTP1_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

void handleScaleProp();

/** Handle the WLAN save form and redirect to WLAN config page again */
void handlePropSave();

//void sectionDateTime();

void sectionSetCal();

//String styleGeneral();

void handleLogin();

bool is_authentified();

void requestLogin(String session, String path, String msg="");

#endif

