#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/ledc.h"

#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_device.h"
#include "esp_gap_bt_api.h"
#include "esp_a2dp_api.h"
#include "esp_avrc_api.h"
#include "esp_err.h"

int error(char *msg, int code)
{
	printf("%s: %d (%s)\n",msg,code,esp_err_to_name(code));
	return 0;
}

int app_main()
{
	int err;
	nvs_flash_init();
 	esp_bt_controller_mem_release(ESP_BT_MODE_BLE);

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_bt_controller_init(&bt_cfg);
	esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
	err = esp_bluedroid_init();
	if(err)return error("Bluedroid init",err);
	err = esp_bluedroid_enable();
	if(err)return error("bluedroid enable",err);

	esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_FIXED;
	esp_bt_pin_code_t pin_code;
	pin_code[0] = '1';
	pin_code[1] = '2';
	pin_code[2] = '3';
	pin_code[3] = '4';
	esp_bt_gap_set_pin(pin_type, 4, pin_code);

	return 0;
}