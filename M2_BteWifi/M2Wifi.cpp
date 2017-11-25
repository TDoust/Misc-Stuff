/*
M2Wifi.cpp

Created: July 10, 2017
Author: Tony Doust

Copyright (c) 2014-2017 Tony Doust

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/


#include "M2Wifi.h"
#include "WiFi.h"


#define STA_SSID "HCS"
#define STA_PASS "tdhcsmx542nc"
#define AP_SSID  "esp32"

enum{
	STEP_BTON, STEP_BTOFF, STEP_STA, STEP_AP, STEP_AP_STA, STEP_OFF, STEP_BT_STA, STEP_END
};

const int Pin = 0;

void onButton(){
	static uint32_t step = STEP_BTON;
	switch(step){
		case STEP_BTON://BT Only
			Serial.println("** Starting BT");
			btStart();
			break;
		case STEP_BTOFF://All Off
			Serial.println("** Stopping BT");
			btStop();
			break;
		case STEP_STA://STA Only
			Serial.println("** Starting STA");
			WiFi.begin(STA_SSID, STA_PASS);
			break;
		case STEP_AP://AP Only
			Serial.println("** Stopping STA");
			WiFi.mode(WIFI_AP);
			Serial.println("** Starting AP");
			WiFi.softAP(AP_SSID);
			break;
		case STEP_AP_STA://AP+STA
			Serial.println("** Starting STA");
			WiFi.begin(STA_SSID, STA_PASS);
			break;
		case STEP_OFF://All Off
			Serial.println("** Stopping WiFi");
			WiFi.mode(WIFI_OFF);
			break;
		case STEP_BT_STA://BT+STA
			Serial.println("** Starting STA+BT");
			WiFi.begin(STA_SSID, STA_PASS);
			btStart();
			break;
		case STEP_END://All Off
			Serial.println("** Stopping WiFi+BT");
			WiFi.mode(WIFI_OFF);
			btStop();
			break;
		default:
			break;
	}
	if(step == STEP_END){
		step = STEP_BTON;
	} else{
		step++;
	}
	//little debounce
	delay(100);
}

void WiFiEvent(WiFiEvent_t event){
	switch(event){
		case SYSTEM_EVENT_AP_START:
			Serial.println("AP Started");
			WiFi.softAPsetHostname(AP_SSID);
			break;
		case SYSTEM_EVENT_AP_STOP:
			Serial.println("AP Stopped");
			break;
		case SYSTEM_EVENT_STA_START:
			Serial.println("STA Started");
			WiFi.setHostname(AP_SSID);
			break;
		case SYSTEM_EVENT_STA_CONNECTED:
			Serial.println("STA Connected");
			WiFi.enableIpV6();
			break;
		case SYSTEM_EVENT_AP_STA_GOT_IP6:
			Serial.print("STA IPv6: ");
			Serial.println(WiFi.localIPv6());
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			Serial.print("STA IPv4: ");
			Serial.println(WiFi.localIP());
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			Serial.println("STA Disconnected");
			break;
		case SYSTEM_EVENT_STA_STOP:
			Serial.println("STA Stopped");
			break;
		default:
			break;
	}
}

void Wifi_BTE_Switch_setup(){
	Serial.begin(115200);
	pinMode(Pin, INPUT_PULLUP);
	WiFi.onEvent(WiFiEvent);
	Serial.print("ESP32 SDK: ");
	Serial.println(ESP.getSdkVersion());
	Serial.println("Press the button to select the next mode");
}

void Wifi_BTE_Switch_loop(){
	static uint8_t lastPinState = 1;
	uint8_t pinState = digitalRead(Pin);
	if(!pinState && lastPinState){
		onButton();
	}
	lastPinState = pinState;
}
