/*
 * Terminals.h
 *
 * Created: 05.02.2018 11:40:51
 *  Author: Kostya
 */ 


#ifndef TERMINALS_H_
#define TERMINALS_H_
#include "Terminal.h"
#include "SerialPort.h"

#define TERMINAL_MAX 3

class BK_Zevs3Class : public TerminalClass{
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
			while(SerialPort.available()){
				if(SerialPort.read() == _sync_byte){
					int i = 0;
					int XOR = 0;
					_w = "";
					while(SerialPort.available()){
						int b = SerialPort.read();
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
								//SerialPort.println(_weight);
								//println(_w.toFloat());
							}
						}
						XOR = XOR ^ b;		/* check sum XOR 8 bit */
						i++;
					}
				}
			}	
		};		
		String getName(){
			return _name;
		};
		bool saveValueHttp(BrowserServerClass *s){};
};

class KeliXK3118T1Class : public TerminalClass{	
	/* =5123.45(kg)\xd\xa */
	private:
		String _name;
		bool _reverse = false;
		int _start = 1, _end = 7;
		int _sync_byte = '=';
		
	public:
		KeliXK3118T1Class(String n): _name(n){};	
		void handlePort(){
			if (SerialPort.available()) {
				if(SerialPort.read() == _sync_byte){
					int i = 1;
					_w = "";
					while(SerialPort.available()){
						int b = SerialPort.read();
						if(i >=_start && i <= _end ){							
							_w = _reverse?char(b)+_w:_w+char(b);
						}
						if(i == _end){
							_weight = _w.toFloat();
						}
						i++;
					}	
				}				
			}				
		};	
		String getName(){
			return _name;
		};
		bool saveValueHttp(BrowserServerClass *s){};
};

class ParserClass : public TerminalClass{
	private:
	String _name;
	bool _reverse = true;
	int _start = 0, _end = 2, _sync = 127, _trim = 32;
	
	public:
	ParserClass(String n): _name(n){};
	void handlePort(){
		while(SerialPort.available()){
			if(SerialPort.read() == _sync){
				int i = 1;				
				_w = "";
				while(SerialPort.available()){
					int b = SerialPort.read();
					if(i >=_start && i <= _end ){
						if (b != _trim || _trim == 0)
							_w = _reverse?char(b)+_w:_w+char(b);	
					}
					if(i == _end){
						_weight = _w.toFloat();	
					}					
					i++;
				}
			}
		}
	};
	String getName(){return _name;};
	bool saveValueHttp(BrowserServerClass *s){
		
		if (s->hasArg("rev"))
			_reverse = true;
		else
			_reverse = false;
		_start = s->arg("str").toInt();
		_end = s->arg("end").toInt();
		_sync = s->arg("syn").toInt();
		_trim = s->arg("trm").toInt();
		
		File readFile = SPIFFS.open(TERMINAL_FILE, "r");
		if (!readFile) {        
			readFile.close();
			return false;	
		}
		
		size_t size = readFile.size();
		std::unique_ptr<char[]> buf(new char[size]);
		readFile.readBytes(buf.get(), size);
		readFile.close();
		
		DynamicJsonBuffer jsonBuffer(JSON_ARRAY_SIZE(TERMINAL_MAX));
		JsonObject& json = jsonBuffer.parseObject(buf.get());

		if (json.containsKey(TERMINAL_TERMINAL_JSON)){
			for(int i; i<TERMINAL_MAX;i++){
				if (json[TERMINAL_TERMINAL_JSON][i]["name"] == "parser"){
					json[TERMINAL_TERMINAL_JSON][i]["rev_id"] = _reverse;
					json[TERMINAL_TERMINAL_JSON][i]["syn_id"] = _sync;
					json[TERMINAL_TERMINAL_JSON][i]["str_id"] = _start;
					json[TERMINAL_TERMINAL_JSON][i]["end_id"] = _end;
					json[TERMINAL_TERMINAL_JSON][i]["trm_id"] = _trim;
					File saveFile = SPIFFS.open(TERMINAL_FILE, "w");
					if (!saveFile) {
						saveFile.close();
						return false;
					}
					json.printTo(saveFile);
					saveFile.flush();
					saveFile.close();
					return true;		
				}
			}	
		}		
		return false;	
	};	
};

class TerminalControllerClass{
	private:
		TerminalClass *_t[TERMINAL_MAX] = {new KeliXK3118T1Class("xk3118t1"), new BK_Zevs3Class("vk_zevs3"), new ParserClass("parser")};
		int _index = 0;
	public:
		TerminalControllerClass(int index){	identify(index);};
		TerminalClass* getIndexOf(int index){
			int pos = -1;
			for(int i = 0; i < TERMINAL_MAX; i++){
				if(_t[i] != NULL){
					pos++;
					if(pos == index){
						_index = index;
						return _t[_index];
					}
				}
			}
			return NULL;
		}
		void identify(int t){
			int pos = -1;
			for(int i = 0; i < TERMINAL_MAX; i++){
				if(_t[i] != NULL){
					pos++;
					if(pos == t)
						_index = t;
				}
			}	
		}	
		TerminalClass* getCurrent(){return (!_t[_index]) ?	_t[0] :	_t[_index];}	
		int getCount(){	return TERMINAL_MAX;}
		int getIndex(){	return _index;}
		void handle(){ _t[_index]->handlePort();}	
};

extern TerminalControllerClass TerminalController;

#endif /* TERMINALS_H_ */