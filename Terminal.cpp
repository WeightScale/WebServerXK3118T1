#include <string>
#include <cstdlib>
#include <algorithm>
#include <Arduino.h>
#include "Terminal.h"
#include "SerialPort.h"
#include "TerminalController.h"



size_t TerminalClass::doData(JsonObject& json ){	
	char buff[10];
	formatValue(buff);
	//dtostrf(_weight, 6-SerialPort.getAccuracy(), SerialPort.getAccuracy(), buff);
	json["w"]= String(buff);
	json["c"]= BATTERY.getCharge();
	json["s"]= SerialPort.getStableWeight();
	
	return json.measureLength();
	
}