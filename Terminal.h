#ifndef _TERMINAL_h
#define _TERMINAL_h
#include <Arduino.h>
#include "SerialPort.h"
#define TERMINAL_MAX 2


class BK_Zevs3Class : public SerialPortClass{
	/*
	The scale sends out data in the format of string. A string is
	consisted of 9 bytes, showed as below:
	|SYNC|ADR|D0|D1|D2|D3|D4|ST|CHK|
	|SYNC| is 1 byte start flag, fixed to 0x7F;
	|ADR| is 1 bye address, which can be modified in Serial
	Communication Setup;
	|D0|D1|D2|D3|D4| are 5 bytes weight data ASCII code;
	|ST| is 1 byte status;
	|CHK| is 1?byte check?sum, which equals to the former 8 bytes’
	XOR sum.
*/
	private:
		String _name; 
		bool _reverse = true;
		int _sync_byte = 127;
		int _space_byte = 32;
		
	public:
		BK_Zevs3Class(String n): _name(n){};
		//virtual ~TerminalClass();
		void handlePort(){
			while(available()){
				if(read() == _sync_byte){
					int i = 0;
					int XOR = 0;
					String _w = "";
					while(available()){
						int b = read();
						switch(i){
							case 0:
							break;
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							if (b != _space_byte)
							_w = _reverse?char(b)+_w:_w+char(b);
							break;
							default:
							if (b == XOR){
								_weight = _w.toFloat();
								//println(_weight);
								//println(_w.toFloat());
							}
						}
						XOR = XOR ^ b;		/* check sum XOR 8 bit */
						i++;
					}
				}
			}	
		};		
		//float getWeight(){return _weight;};
};

class KeliXK3118T1Class : public SerialPortClass{
	private:
		String _name;
		bool _reverse = true;
		int _total = 12, _valid = 7;
		
	public:
		KeliXK3118T1Class(String n): _name(n){};	
		void handlePort(){
			if (available()) {
				String str = readStringUntil(0xa); //LF	
				int len = str.length();	
				if(len >= _total){
					int b = len - _total;
					int e = len - (_total - _valid);
					_weight = str.substring(b, e).toFloat();
					//println(_weight);
				}				
			}
			/*while(available()){
				if(read() == SYNC_BYTE){
					int i = 0;
					int XOR = 0;
					_w = "";
					while(available()){
						int b = read();
						switch(i){
							case 0:
							break;
							case 1:
							case 2:
							case 3:
							case 4:
							case 5:
							if (b != SPACE_BYTE)
								_w = _reverse?char(b)+_w:_w+char(b);
							break;
							default:
							if (b == XOR){
								println(_w);
								println(_w.toFloat());
							}
						}
						XOR = XOR ^ b;		/ * check sum XOR 8 bit * /
						i++;
					}
				}
			}*/
		};	
		
};

class TerminalClass {
	private:
		SerialPortClass *_t[TERMINAL_MAX] = {new KeliXK3118T1Class("xk3118t1"), new BK_Zevs3Class("vk_zevs3")};
		SerialPortClass* _terminal;
		int _index = 0;	
	public:
		TerminalClass();
		SerialPortClass* getIndexOf(int index){
			int pos = -1;
			for(int i = 0; i < TERMINAL_MAX; i++){
				if(_t[i] != NULL){
					pos++;

					if(pos == index)
					return _t[i];
				}
			}
			return NULL;			
		}
		void setTerminal(SerialPortClass* s){
			_terminal = s;
		}
		SerialPortClass* getTerminal(){
			return (!_terminal) ?	_terminal = _t[0] :	_terminal;	
		}
};

extern TerminalClass Terminals;
extern SerialPortClass *port;

#endif

