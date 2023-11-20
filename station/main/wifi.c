#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include <string.h>

volatile int has_ip = 0;
//volatile int wifi_ready = 0;
volatile int wifi_trying_connect = 0;
esp_netif_t *netif;

typedef struct {
	char ssid[64];
	char passwd[64];
} wifi_data_t;

wifi_data_t wifi_data;

static void on_wifi_disconnect();
static void on_got_ip();

int is_wifi_ready(void)
{
	return has_ip;
}

int wait_for_wifi()
{
	while(1)
	{
		if(is_wifi_ready())
			return 0;
		printf("Waiting for wifi...\n");
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}

int wifi_connect()
{
	printf("Call wifi_connect\n");
	wifi_config_t wifi_config;
	memset(&wifi_config, 0, sizeof(wifi_config));
	strncpy((char *)wifi_config.sta.ssid, wifi_data.ssid, strlen(wifi_data.ssid));
	strncpy((char *)wifi_config.sta.password, wifi_data.passwd, strlen(wifi_data.passwd));
	
	printf("Connecting to %s...\n", wifi_config.sta.ssid);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(WIFI_IF_STA, &wifi_config);

	esp_wifi_start();

	esp_err_t err = esp_wifi_connect();
		
	printf("esp_wifi_connect() returns: %d\nWaiting for connection...\n", err);
	
	if (err == ESP_ERR_WIFI_NOT_STARTED) {
		printf("Wi-Fi not started\n");
		return -1;
	}
	return 0;
}


static void on_got_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	printf("Wi-Fi Got IP\n");
	has_ip = 1;
	ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
	int ip = event->ip_info.ip.addr;
	printf("Your ip: %d.%d.%d.%d\n", (ip & 0xff), (ip & 0x0000ff00) >> 8, (ip & 0x00ff0000) >> 16, (ip & 0xff000000) >> 24);
}

static void on_lost_ip(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	has_ip = 0;
	printf("Wi-Fi LOST IP\n");
}

static void on_wifi_disconnect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	printf("******************************\nWi-Fi disconnected, trying to reconnect...\n");
	has_ip = 0;
	esp_wifi_stop();
	wifi_connect();
}

static void on_wifi_start(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	printf("Wi-Fi started\n");
}

static void on_wifi_connect(void *arg, esp_event_base_t event_base,
							   int32_t event_id, void *event_data)
{
	printf("Wi-Fi connected\n");
}

esp_netif_t *wifi_init(char *ssid, char *passwd)
{
	esp_event_loop_create_default();
	esp_netif_init();

	sprintf(wifi_data.ssid, ssid);
	sprintf(wifi_data.passwd, passwd);

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	cfg.nvs_enable = 0;
	esp_wifi_init(&cfg);
	
	esp_netif_config_t netif_config = ESP_NETIF_DEFAULT_WIFI_STA();
	netif = esp_netif_new(&netif_config);

	esp_netif_attach_wifi_station(netif);
	esp_wifi_set_default_wifi_sta_handlers();

	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &on_wifi_disconnect, NULL);
	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &on_wifi_connect, NULL);
	esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_START, &on_wifi_start, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &on_got_ip, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_LOST_IP, &on_lost_ip, NULL);
	esp_wifi_set_storage(WIFI_STORAGE_RAM);

	esp_wifi_config_80211_tx_rate(WIFI_IF_STA, WIFI_PHY_RATE_1M_L);

	wifi_connect();

	return netif;
}