#include <EEPROM.h>
#include "handleHttp.h"
#include "tools.h"
#include "Scales.h"
#include "Page.h"
#include "XK3118T1.h"
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
		SCALES.sendScaleSettingsSaveValue();
		/*for (uint8_t i = 0; i < browserServer.args(); i++) {
			if (browserServer.argName(i)=="n"){
				SCALES.setSSID(browserServer.arg(i));
				rec = true;	
			}
			if (browserServer.argName(i)=="p"){
				rec = true;
				SCALES.setPASS(browserServer.arg(i));
			}
			if (browserServer.argName(i)=="name_admin"){
				SCALES.setNameAdmin(browserServer.arg(i));
				rec = true;
			}
			if (browserServer.argName(i)=="pass_admin"){
				rec = true;
				SCALES.setPassAdmin(browserServer.arg(i));
			}
			if (browserServer.argName(i)=="nvp"){
				SCALES.setMax(browserServer.arg(i).toInt());
				browserServer.send(200, "text/html", String(SCALES.getMax()));
			}
			/ *if (browserServer.argName(i)=="code"){
				SCALES.setCodeServer(browserServer.arg(i));
				browserServer.send(200, "text/html", String(SCALES.getCodeServer()));
			}
			if (browserServer.argName(i)=="email"){
				SCALES.setEmail(browserServer.arg(i));
				browserServer.send(200, "text/html", String(SCALES.getEmail()));
			}
			if (browserServer.argName(i)=="pin"){
				SCALES.setPin(browserServer.arg(i));
				browserServer.send(200, "text/html", String(SCALES.getPin()));
			}* /
			if (browserServer.argName(i)=="date"){
				DateTimeClass DateTime(browserServer.arg("date"));
				Rtc.SetDateTime(DateTime.toRtcDateTime());
				String message = "<div>Дата синхронизирована<br/>";
				message+=getDateTime()+"</div>";
				browserServer.send(200, "text/html", message);
				return;
			}				
		}
		SCALES.save();
		if(rec){
			//connect = strlen(SCALES.getSSID()) > 0; // Request WLAN connect with new credentials if there is a SSID	
			String message = "<meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/>";
			message += "<meta http-equiv='refresh' content='15; URL=/scaleprop.html'>Please Wait....Configuring and Restarting.";
			browserServer.send(200, "text/html", message);
			delay(1000);	
			ESP.restart();	
		}*/
		//browserServer.sendHeader("Location", browserServer.uri(), true);
		//browserServer.send(301, "text/html", "");	
	}else{
		handleFileRead(browserServer.uri());
	}	
}

void handlePortSave(){
	if (browserServer.args() > 0){ // Save Settings
		SCALES.getPortValue();
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



