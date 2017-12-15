// 
// 
// 
#include <stdio.h>
#include <string.h>
#include "XK3118T1.h"
#include "Scales.h"

XK3118T1Class::~XK3118T1Class(){}

void XK3118T1Class::init(){
}

void XK3118T1Class::parseDate(String str){
	int len = str.length();
	/*if (len == 7){
		char string[8];
		str.toCharArray(string, sizeof(string));
		reverse(string);
		weight = String(string);	
	}else*/ 
	if(len > 12){
		//weight = str.substring(0,7);
		//weight = str.substring(1,8).toInt();
		//_weight = str.substring(len-12,len-5).toInt();
		_weight = str.substring(str.indexOf("(")-7,str.indexOf("(")).toInt();
		
		//_weight = weight.toInt();		
		detectStable();	
	}
	//temp_w = str;
	
	//String fs;
	/*if (weight.indexOf(".")!=-1){
		isFloat = true;
		String fs = weight.substring(weight.indexOf(".")+1);
		int i = fs.toInt();
		if (i != 0){
			weight = String(weight.toFloat(),fs.length());			
		}else{
			weight = String(weight.toInt()) ;
		}
	}else{
		isFloat = false;
		weight = String(weight.toInt());
	}*/
	//weight = String(weight.toInt());
	//_weight = weight.toFloat();
	//weight = str;
	
	
	/*if(str.startsWith("=",0)){
		weight = str.substring(1,str.indexOf("("));
		_weight = weight.toFloat();
		String fs;
		if (weight.indexOf(".")!=-1){
			fs = weight.substring(weight.indexOf(".")+1,weight.indexOf("("));
			int i = fs.toInt();
			if (i == 0){
				weight = String(weight.toInt()) ;
				}else{
				weight = String(weight.toFloat());
			}
			}else{
			weight = String(weight.toInt());
		}	
	}else{
		weight="---";
	}*/
	
	//SCALES.saveEvent("вес", weight);	
}

void XK3118T1Class::reverse(char *string){
	int length, c;
	char *begin, *end, temp;
	
	length = string_length(string);
	begin  = string;
	end    = string;
	
	for (c = 0; c < length - 1; c++)
		end++;
	
	for (c = 0; c < length/2; c++){
		temp   = *end;
		*end   = *begin;
		*begin = temp;
		
		begin++;
		end--;
	}
}

int XK3118T1Class::string_length(char *pointer){
	int c = 0;
	
	while( *(pointer + c) != '\0' )
	c++;
	
	return c;
}

/*********************************/
/*         Строим команду        */
/*********************************/
void XK3118T1Class::buildCommand(){	
	parseDate(Serial.readStringUntil(LF));
}

void XK3118T1Class::tape(){
	Serial.println("E");	
}

void XK3118T1Class::zero(){
	Serial.println("D");	
}

void XK3118T1Class::reload(){
	Serial.println("R");	
}

XK3118T1Class XK3118T1("XK3118T1");

