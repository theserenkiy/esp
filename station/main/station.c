#include <stdio.h>
#include <string.h>
//#include "freertos/FreeRTOS.h"
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
#include "wifi.c"


#include "esp_netif_sntp.h"


#include "stdin.c"
#include "time.h"


#define WIFI_SSID      "ConnectToMeBaby"
#define WIFI_PASS      "pankihoy"


int syncTime()
{
	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(2,
                               ESP_SNTP_SERVER_LIST("time.windows.com", "pool.ntp.org" ) );
    esp_netif_sntp_init(&config);
	int attempts = 10;
	for(int i = 0; i < attempts; i++)
	{
		printf("Trying to sync time: %d/%d\n", i, attempts);
		if(!esp_netif_sntp_sync_wait(pdMS_TO_TICKS(2000)))
			return 0;
	}
	return -1;
}


void app_main(void)
{
	//nvs_flash_init();
	
	wifi_init(WIFI_SSID, WIFI_PASS);

	syncTime();

	time_t utime; 
	struct tm *tm;
	
	while(1)
	{
		utime = time(NULL);
		tm = gmtime(&utime);
		//printf("Current time: %d-%d-%d %d:%d:%d\n", tm->year, tm->mon, tm->mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
		printf("Current time: %d:%d:%d\n", tm->tm_hour, tm->tm_min, tm->tm_sec);
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}

	//fat_test();

	//mount_partition("failo1","/storage");
	
}

