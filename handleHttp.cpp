#include <EEPROM.h>
#include "handleHttp.h"
#include "tools.h"
#include "Scales.h"
#include "Page.h"
//#include "XK3118T1.h"
#include "DateTime.h"
#include "BrowserServer.h"

void handleScaleProp(){	
	if (!is_authentified())
		return browserServer.requestAuthentication();
	String values = "";
	//values += "id_nvp|" + String(SCALES.getMax()) + "|input\n";
	values += "id_date|" + getDateTime() + "|div\n";
	//values += "id_code|" + String(SCALES.getCodeServer()) + "|input\n";
	//values += "id_email|" + String(SCALES.getEmail()) + "|input\n";
	//values += "id_pin|" + String(SCALES.getPin()) + "|input\n";
	//values += "id_name_admin|" + String(SCALES.getNameAdmin()) + "|input\n";
	//values += "id_pass_admin|" + String(SCALES.getPassAdmin()) + "|input\n";
	values += "id_local_host|http://"+String(myHostname)+".local|div\n";
	values += "id_ap_ssid|" + String(softAP_ssid) + "|div\n";
	values += "id_ap_ip|" + toStringIp(WiFi.softAPIP()) + "|div\n";
	//values += "id_lan_ssid|" + String(SCALES.getSSID()) + "|div\n";
	values += "id_lan_ip|" + toStringIp(WiFi.localIP()) + "|div\n";
	//values += "id_ssid|" + String(SCALES.getSSID()) + "|input\n";
	//values += "id_pass|" + String(SCALES.getPASS()) + "|input\n";
	browserServer.send(200, "text/plain", values);
}

/** Handle the WLAN save form and redirect to WLAN config page again */
void handlePropSave() {
	bool rec = false;	
		
	if (browserServer.args() > 0){ // Save Settings			
		SCALES.getScaleSettingsValue();		
	}else{
		handleFileRead(browserServer.uri());
	}	
}

void handlePortSave(){
	if (browserServer.args() > 0){ // Save Settings
		if(SCALES.getPortValue()){
			browserServer.send(200, "text/html", "");
		}else{
			browserServer.send(400, "text/html", "Ошибка");
		}
	}else{
		handleFileRead(browserServer.uri());
	}
}

//Check if header is present and correct
bool is_authentified(){
	if (!browserServer.authenticate(SCALES.getNameAdmin().c_str(), SCALES.getPassAdmin().c_str())){
		if (!browserServer.checkAuth()){
			return false;	
		}
	}	
	return true;
}



