#include "ScaleMem.h"

ScaleMemClass::~ScaleMemClass(){};

void ScaleMemClass::init(){
	/** Перед инициализацией устройств считываем из eeprom значение */
	begin(sizeof(scale_t));
	//scale_eep = (scale_t *)getDataPtr();
}

bool ScaleMemClass::save(){
	_dirty = true;
	return EEPROMClass::commit();
}


