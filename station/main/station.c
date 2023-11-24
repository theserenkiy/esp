#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
//#include "freertos/queue.h"
//#include "freertos/task.h"
//#include "freertos/event_groups.h"
//#include "esp_system.h"
//#include "esp_wifi.h"
//#include "esp_event.h"
//#include "esp_log.h" 
#include "nvs_flash.h"
#include "driver/gpio.h"

//#include "lwip/err.h"
//#include "lwip/sys.h"


//#include "fat.c"
#include "wifi.c"
//#include "stdin.c"
#include "time_sync.c"

//#include "driver/gptimer.h"
//#include "timer.c"

//#include "keyboard_test.c"

//#include "ledc_test.c"
#include "iotport.c"

#define WIFI_SSID      "ConnectToMeBaby"
#define WIFI_PASS      "pankihoy"


void app_main(void)
{
	gpio_reset_pin(12);
	gpio_set_direction(12, GPIO_MODE_OUTPUT);
	gpio_set_level(12, 0);

	nvs_flash_init();
	wifi_init(WIFI_SSID, WIFI_PASS);
	//keyboardSummator();	
	//ledc_test();
	int len;
	int ledstate;
	while(1)
	{
  		len = iotport_getVar(1, &ledstate);
		if(len > 0)
		{
			gpio_set_level(12, !!ledstate);
		}
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

