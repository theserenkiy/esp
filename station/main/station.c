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

//#include "lwip/err.h"
//#include "lwip/sys.h"


//#include "fat.c"
//#include "wifi.c"
//#include "stdin.c"
//#include "time_sync.c"

//#include "driver/gptimer.h"

#include "timer.c"


#define WIFI_SSID      "ConnectToMeBaby"
#define WIFI_PASS      "pankihoy"






void app_main(void)
{
	//nvs_flash_init();
	
	//wifi_init(WIFI_SSID, WIFI_PASS);
	//timeTest();
	//fat_test();
	//mount_partition("failo1","/storage");

	timer_test();
	
}
