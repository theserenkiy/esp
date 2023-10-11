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
//#include "wifi.c"


#include "esp_netif_sntp.h"


#include "stdin.c"


#define WIFI_SSID      "ConnectToMeBaby"
#define WIFI_PASS      "pankihoy"



void app_main(void)
{
	//nvs_flash_init();
	
	wifi_init(WIFI_SSID, WIFI_PASS);

	esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG_MULTIPLE(2,
                               ESP_SNTP_SERVER_LIST("time.windows.com", "pool.ntp.org" ) );
    esp_netif_sntp_init(&config);

	for(int i = 0; i < 10; i++)
	{
		if(ERR_OK == esp_netif_sntp_sync_wait(pdMS_TO_TICKS(2000)))
			break
	}



	//fat_test();

	//mount_partition("failo1","/storage");

	
}

