#include "ScaleMem.h"

ScaleMemClass::~ScaleMemClass(){};

void ScaleMemClass::init(){
	/** ����� �������������� ��������� ��������� �� eeprom �������� */
	begin(sizeof(scale_t));
	//scale_eep = (scale_t *)getDataPtr();
}

bool ScaleMemClass::save(){
	_dirty = true;
	return EEPROMClass::commit();
}


