// Tasks.h

#ifndef _TASK_h
#define _TASK_h
#include <Arduino.h>
/*
#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif*/

#include <inttypes.h>

/*
	Uncomment this line to enable TaskName Strings.

	It might be usefull if you are loging thread with Serial,
	or displaying a list of threads...
*/
// #define USE_TASK_NAMES	1

class Task{
protected:
	/*! Интервал между запусками */
	unsigned long interval;

	/*! Последнее время запкска в милисекундах */
	unsigned long last_run;

	/*! Запланированый пробег в милисекундах */	
	unsigned long _cached_next_run;
	
	bool Paused = false;

	/*!
		IMPORTANT! Run after all calls to run()
		Updates last_run and cache next run.
		NOTE: This MUST be called if extending
		this class and implementing run() method
	*/
	void runned(unsigned long time);

	// Default is to mark it runned "now"
	void runned() { runned(millis()); }

	// Callback for run() if not implemented
	void (*_onRun)(void);		

public:

	// If the current Tasks is enabled or not
	bool enabled;

	// ID of the Tasks (initialized from memory adr.)
	int TaskID;

	#ifdef USE_TASK_NAMES
		// Tasks Name (used for better UI).
		String TaskName;			
	#endif

	Task(void (*callback)(void) = NULL, unsigned long _interval = 0);

	// Set the desired interval for calls, and update _cached_next_run
	virtual void setInterval(unsigned long _interval);

	// Return if the Task should be runned or not
	virtual bool shouldRun(unsigned long time);

	// Default is to check whether it should run "now"
	bool shouldRun() { return shouldRun(millis()); }

	// Callback set
	void onRun(void (*callback)(void));

	/// Запуск
	virtual void run();
	
	void resume(){Paused = false; runned();};
	void pause(){Paused = true;};
	void updateCache(){runned();}
};

#endif


