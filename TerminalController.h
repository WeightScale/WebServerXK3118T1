// TerminalController.h

#ifndef _TERMINALCONTROLLER_h
#define _TERMINALCONTROLLER_h
#include "Terminal.h"
#include "Terminals.h"

class TerminalControllerClass{
	private:
	TerminalClass *_t[TERMINAL_MAX] = {new KeliXK3118T1Class("xk3118t1"), new Zevs_A12eClass("Zevs_A12e"), new BK_Zevs3Class("vk_zevs3"), new ParserClass("parser")};
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
				if(pos == t){
					_index = t;
					_t[_index]->downloadValue(_index);
				}
			}
		}
	}
	TerminalClass* getCurrent(){return (!_t[_index]) ?	_t[0] :	_t[_index];}
	int getCount(){	return TERMINAL_MAX;}
	int getIndex(){	return _index;}
	void handle(){ _t[_index]->handlePort();}
};

extern TerminalControllerClass TERMINAL;


#endif

