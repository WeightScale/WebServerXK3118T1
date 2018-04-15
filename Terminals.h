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

#define TERMINAL_MAX 4

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
	|CHK| is 1?byte check?sum, which equals to the former 8 bytesТ
	XOR sum.
*/
	private:		 
		bool _reverse = true;
		int _sync_byte = 127;
		int _space_byte = 32;
		
	public:
		BK_Zevs3Class(String n): TerminalClass(n){};
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
		bool saveValueHttp(AsyncWebServerRequest * request){};
		bool downloadValue(int inx){};
		//bool sendClient(AsyncWebSocketClient * client){};
};

class KeliXK3118T1Class : public TerminalClass{	
	/* =5123.45(kg)\xd\xa */
	private:
		bool _reverse = false;
		int _start = 1, _end = 7;
		int _sync_byte = '=';
		
	public:
		KeliXK3118T1Class(String n): TerminalClass(n){};	
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
							return;
						}
						i++;
					}	
				}				
			}							
		};			
		bool saveValueHttp(AsyncWebServerRequest * request){};
		bool downloadValue(int inx){};
};

class Zevs_A12eClass : public TerminalClass{
	/* >SG   5687kg\xd\xa 
	S - стабильно 
	U - не стабильно
	N - стабильно негатив
	V - перегруз
	*/
	
	private:
	//bool _reverse = false;
	//int _start = 2, _end = 8;
	int _point = 0;				//где находитс€ точка
	int _stage, _type;
	int _sync_byte = '>',_space_byte = 32;
	
	public:
	Zevs_A12eClass(String n): TerminalClass(n){};
	void handlePort(){
		if (SerialPort.available()) {
			if(SerialPort.read() == _sync_byte){
				int i = 0;
				_w = "";
				_point = 0;
				while(SerialPort.available()){
					int b = SerialPort.read();
					switch(i){
						case 0:
							_stage = b;
						break;
						case 1:
							_type = b;
						break;						
						case 2:
						case 3:
						case 4:
						case 5:
						case 6:
						case 7:
						case 8:
							if (b != _space_byte)
								_w = _w+char(b);
							if(b == '.')
								_point = (8 - i);
							
						break;
						default:
							if (b == 0x0A){
								_weight = _w.toFloat();
							}
					}
					i++;
				}
			}
		}
	};	
	bool saveValueHttp(AsyncWebServerRequest * request){};
	bool downloadValue(int inx){};
	size_t doData(JsonObject& json ){
		char buff[10];
		formatValue(buff);
		json["w"]= String(buff);
		json["c"]= BATTERY.getCharge();
		if (_stage == 'S' || _stage == 'N')
			json["s"]= true;	
		else
			json["s"]= false;		
		json["st"] = _stage;
		json["t"] = _type;
		
		return json.measureLength();
	}
	void formatValue(char* string){
		dtostrf(_weight, 6-_point, _point, string);
	}
};

class ParserClass : public TerminalClass{
	private:
	bool _reverse = true;
	int _start = 0, _end = 2, _sync = 127, _trim = 32;
	
	public:
	ParserClass(String n): TerminalClass(n){};
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
	bool saveValueHttp(AsyncWebServerRequest * request){
		
		if (request->hasArg("rev"))
			_reverse = true;
		else
			_reverse = false;
		_start = request->arg("str").toInt();
		_end = request->arg("end").toInt();
		_sync = request->arg("syn").toInt();
		_trim = request->arg("trm").toInt();
		
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
	bool downloadValue(int inx){
		_reverse = false;
		_start = 0;
		_end = 1;
		_sync = 0;
		_trim = 0;
		File portFile;
		if (SPIFFS.exists(TERMINAL_FILE)){
			portFile = SPIFFS.open(TERMINAL_FILE, "r");
		}else{
			portFile = SPIFFS.open(TERMINAL_FILE, "w+");
		}
		if (!portFile) {
			portFile.close();
			return false;
		}

		size_t size = portFile.size();
		std::unique_ptr<char[]> buf(new char[size]);
		portFile.readBytes(buf.get(), size);
		portFile.close();
		DynamicJsonBuffer jsonBuffer(size);
		JsonObject& json = jsonBuffer.parseObject(buf.get());
		
		if (!json.success()) {
			return false;
		}
		
		_reverse = json[TERMINAL_TERMINAL_JSON][inx]["rev_id"];
		_sync = json[TERMINAL_TERMINAL_JSON][inx]["syn_id"];
		_start = json[TERMINAL_TERMINAL_JSON][inx]["str_id"];
		_end = json[TERMINAL_TERMINAL_JSON][inx]["end_id"];
		_trim = json[TERMINAL_TERMINAL_JSON][inx]["trm_id"];
		return true;	
	}
};

#endif /* TERMINALS_H_ */