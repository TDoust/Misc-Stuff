/*
M2_BteWifi.ino

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

#include <Arduino.h>
#include <cstdint>

#include "M2_BteWifi.h"
#include "M2Bte.h"
#include "M2Wifi.h"

//#include "SimpleBLE.h"
#include "esp32-hal-log.h"

#include "bt.h"
#include "bta_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

void serialEvent(){
	// Serial data from the serial Input can just be looped back
	while(Serial.available()){
	}
}

void setup(){
	pinMode(TX, OUTPUT);
	digitalWrite(TX, LOW);
	Serial.begin(115200);	// XBEE Serial speed
}

void loop(){
	while(Serial.available()){	// use serialEvent to get the data from the XBEE interface & loop it back
		int c = (uint8_t) Serial.read();
		Serial.write(c);
		Serial.flush();
	}
	Serial.println("Wifi module");
	delay(2000);
}