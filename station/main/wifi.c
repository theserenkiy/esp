#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_event.h"

volatile int has_ip = 0;
volatile int wifi_trying_connect = 0;
esp_netif_t *netif;

int wait_for_ip(int ntry)
{
	for(int i = 0; i < ntry; i++)
	{
		if(has_ip)
			break;
		printf("Waiting for IP: %d of %d\n", i, ntry);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
	return has_ip;
}

int wifi_connect()
{
	if(wifi_trying_connect)
	{
		printf("Already trying to connect\n");
		return -1;
	}
	while(!has_ip)
	{
		printf("Connecting to wifi\n");
		esp_err_t err = esp_wifi_connect();
		if (err == ESP_ERR_WIFI_NOT_STARTED) {
			printf("Wi-Fi not started\n");
			return -1;
		}
		wifi_trying_connect = 1;
		if(wait_for_ip(16))
			break;
	}
	wifi_trying_connect = 0;
	return has_ip ? 0 : -1;
}


static void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	printf("Got IP event!\n");
	has_ip = 1;
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	int ip = event->ip_info.ip.addr;
	printf("Your ip: %d.%d.%d.%d\n", (ip & 0xff), (ip & 0x0000ff00) >> 8, (ip & 0x00ff0000) >> 16, (ip & 0xff000000) >> 24);
}

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	has_ip = 0;
	printf("Wi-Fi disconnected, trying to reconnect...\n");
	wifi_connect();
}

esp_netif_t *wifi_init(char *ssid, char *passwd)
{
	esp_event_loop_create_default();
	esp_netif_init();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	cfg.nvs_enable = 0;
	esp_wifi_init(&cfg);

	
	esp_netif_config_t netif_config = ESP_NETIF_DEFAULT_WIFI_STA();
	netif = esp_netif_new(&netif_config);

	//assert(netif);

	esp_netif_attach_wifi_station(netif);
	esp_wifi_set_default_wifi_sta_handlers();

	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);

	wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));
	strncpy((char *)wifi_config.sta.ssid, ssid, strlen(ssid));
	strncpy((char *)wifi_config.sta.password, passwd, strlen(passwd));
	
	printf("Connecting to %s...\n", wifi_config.sta.ssid);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
	esp_wifi_start();
	
	wifi_connect();

	return netif;
}