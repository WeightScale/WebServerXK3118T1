
#ifndef _SCALEMEM_h
#define _SCALEMEM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <EEPROM.h>
extern "C" {
	#include "c_types.h"
	#include "ets_sys.h"
	#include "os_type.h"
	#include "osapi.h"
	#include "spi_flash.h"
}

extern "C" uint32_t _SPIFFS_end;

#define UART_RX_BUFFER			0xFF

typedef struct{
	char date[20];
	double weight;	
}weight_stroke_t;

typedef struct{		
	unsigned long max;					/* ������������ ��� */	
	char name_admin[7];					/* ��� ������� �������������� */
	char pass_admin[7];					/* ��� ������� �������������� */
	char ssid[32];						/* ��� ����� �������. */
	char pass[32];						/* ���� ����� �������. */
	char email[32];						/* ����� ����������� ����� ��� ����������� �����. */
	char code_server[10];				/* ��� ������� � ����������� �������.*/
	char pin[10];						/* ��� �����. */
}scale_t;

class ScaleMemClass : protected EEPROMClass {
	protected:
		void init();	

	public:
		//ScaleMemClass(uint32_t sector);
		ScaleMemClass(uint32_t section = (((uint32_t)&_SPIFFS_end - 0x40200000) / SPI_FLASH_SEC_SIZE)) : EEPROMClass(section){};
		~ScaleMemClass();			
		bool save();
				
};


#endif

