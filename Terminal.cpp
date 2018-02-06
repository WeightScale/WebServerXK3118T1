#include <string>
#include <cstdlib>
#include <algorithm>
#include <Arduino.h>
#include "Terminal.h"
#include "SerialPort.h"

TerminalClass Terminals;
SerialPortClass *port = Terminals.getIndexOf(1);

TerminalClass::TerminalClass() {
	_terminal = _t[1];
}