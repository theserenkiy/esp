#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#define PIN_HDATA	36
#define PIN_SCL		13
#define PIN_SDA		12
#define PIN_VS		25
#define PIN_HS		14
#define PIN_PCLK	27
#define PIN_MCLK	26


void init_sccb()
{
	gpio_reset_pin(PIN_SCL);
	gpio_set_direction(PIN_SCL, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_SCL, 1);

	gpio_reset_pin(PIN_SDA);
	gpio_set_direction(PIN_SDA, GPIO_MODE_INPUT);
	gpio_set_pull_mode(PIN_SDA, GPIO_PULLUP_ONLY);
}

void sccb_send(uint8_t cmd, uint8_t data)
{
	gpio_set_level(PIN_SCL, 0);
	gpio_set_direction(PIN_SDA,GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_SDA,0);

	usleep(5);
	uint8_t mask = 1;
	for(int i=0; i < 8; i++)
	{
		
	}


}

void app_main()
{

}