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

#define WIFI_SSID      "ConnectToMeBaby"
#define WIFI_PASS      "pankihoy"


typedef struct {
	int state;
	char time[10];
}log_row_t;

void app_main(void)
{
	nvs_flash_init();
	wifi_init(WIFI_SSID, WIFI_PASS);

	wait_for_wifi();

	syncTime();

	char time[32];
	

	gpio_reset_pin(12);
	gpio_set_direction(12,GPIO_MODE_OUTPUT);
	gpio_set_drive_capability(12,GPIO_DRIVE_CAP_0);
	
	int laststate = 0, state = 0;
	
	log_row_t log[32];
	int log_cnt = 0;

	while(1)
	{
		state = is_wifi_ready();
		gpio_set_level(12,state);
		//printf("WiFi ready: %d\n",is_wifi_ready());

		if(state != laststate)
		{
			log[log_cnt].state = state;
			getCurTime(log[log_cnt].time);
			log_cnt++;

			printf("********************************\n");
			for(int i=0; i < log_cnt;i++)
			{
				printf("%d:	%s\n",log[i].state,log[i].time);
			}

			printf("********************************\n");

			if(state && !laststate)
			{
				printf("WiFi ON\n");
			}
			else
			{
				printf("WiFi OFF\n");
			}
		}

		laststate = state;

		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

