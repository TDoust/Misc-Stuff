/*
M2Bte.cpp

Modified: July 10, 2017
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


#include "M2Bte.h"

#include "esp32-hal-log.h"

#include "bt.h"
#include "bta_api.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"

static esp_ble_adv_data_t _adv_config = {
	.set_scan_rsp = false,
	.include_name = true,
	.include_txpower = true,
	.min_interval = 512,
	.max_interval = 1024,
	.appearance = 0,
	.manufacturer_len = 0,
	.p_manufacturer_data = NULL,
	.service_data_len = 0,
	.p_service_data = NULL,
	.service_uuid_len = 0,
	.p_service_uuid = NULL,
	.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT)
};

static esp_ble_adv_params_t _adv_params = {
	.adv_int_min = 512,
	.adv_int_max = 1024,
	.adv_type = ADV_TYPE_NONCONN_IND,
	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.peer_addr = { 0x00, },
	.peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
	.channel_map = ADV_CHNL_ALL,
	.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static void _on_gap(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param){
	if(event == ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT){
		esp_ble_gap_start_advertising(&_adv_params);
	}
}

static bool _init_gap(const char * name){
	if(!btStarted() && !btStart()){
		log_e("btStart failed");
		return false;
	}
	esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
	if(bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED){
		if(esp_bluedroid_init()){
			log_e("esp_bluedroid_init failed");
			return false;
		}
	}
	if(bt_state != ESP_BLUEDROID_STATUS_ENABLED){
		if(esp_bluedroid_enable()){
			log_e("esp_bluedroid_enable failed");
			return false;
		}
	}
	if(esp_ble_gap_set_device_name(name)){
		log_e("gap_set_device_name failed");
		return false;
	}
	if(esp_ble_gap_config_adv_data(&_adv_config)){
		log_e("gap_config_adv_data failed");
		return false;
	}
	if(esp_ble_gap_register_callback(_on_gap)){
		log_e("gap_register_callback failed");
		return false;
	}
	return true;
}

static bool _stop_gap(){
	if(btStarted()){
		esp_bluedroid_disable();
		esp_bluedroid_deinit();
		btStop();
	}
	return true;
}

/*
* BLE Arduino
*
* */

SimpleBLE::SimpleBLE(){
	local_name = "M2esp32";
}

SimpleBLE::~SimpleBLE(void){
	_stop_gap();
}

bool SimpleBLE::begin(String localName){
	if(localName.length()){
		local_name = localName;
	}
	return _init_gap(local_name.c_str());
}

void SimpleBLE::end(){
	_stop_gap();
}
