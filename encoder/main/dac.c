#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/ledc.h"

#define BCK 17
#define WS 5
#define DIN 18


void sendWord(uint16_t val)
{
	uint16_t mask = 0x8000;
	for(int b=15; b >= 0; b--)
	{
		gpio_set_level(DIN, val & mask);
		mask >>=1;
		//usleep(1);
		gpio_set_level(BCK,1);
		//usleep(1);
		gpio_set_level(BCK,0);
		//usleep(1);
	}
}

void app_main()
{
	gpio_set_direction(BCK,GPIO_MODE_OUTPUT);
	gpio_set_direction(WS,GPIO_MODE_OUTPUT);
	gpio_set_direction(DIN,GPIO_MODE_OUTPUT);

	gpio_set_level(BCK,0);

	uint16_t buf[1024];

	for(int i=0; i < 1024;i++)
	{
		buf[i] = i*64;
	}

	uint16_t val, mask;
	while(1)
	{
		for(int i=0; i < 1024; i++)
		{
			gpio_set_level(WS,0);
			//usleep(1);
			sendWord(buf[i]);
			gpio_set_level(WS,1);
			//usleep(1);
			sendWord(buf[i]);
		}
		vTaskDelay(10/portTICK_PERIOD_MS);
	}

}