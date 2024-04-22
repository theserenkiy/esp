#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/ledc.h"
#include "font.c"

#define CLK 26
#define DIO 27


void delay(int val)
{
	usleep(val*5);
}

void start()
{
	gpio_set_level(CLK,1);
	gpio_set_level(DIO,1);

	delay(1);
	gpio_set_level(DIO,0);
	delay(1);
	gpio_set_level(CLK,0);
}

void stop()
{
	gpio_set_level(CLK,0);
	gpio_set_level(DIO,0);

	delay(1);
	gpio_set_level(CLK,1);
	delay(1);
	gpio_set_level(DIO,1);
	delay(1);
}

void sendByte(uint8_t data)
{
	for(int i=0; i < 8; i++)
	{
		gpio_set_level(DIO,(data & 0b10000000) ? 1 : 0);
		data <<= 1;
		delay(1);
		gpio_set_level(CLK,1);
		delay(1);
		gpio_set_level(CLK,0);
		delay(1);
	}
	gpio_set_level(DIO,1);
	delay(1);
	gpio_set_level(CLK,1);
	delay(1);
	gpio_set_level(CLK,0);
}

void app_main()
{
	gpio_set_direction(CLK,GPIO_MODE_OUTPUT);
	gpio_set_direction(DIO,GPIO_MODE_OUTPUT);

	while(1)
	{
		start();
		sendByte(0b01000100);
		stop();

		start();
		sendByte(0b11000000);
		sendByte(0b10111011);
		stop();

		start();
		sendByte(0b10001111);
		stop();

		vTaskDelay(10/portTICK_PERIOD_MS);
	}
	
}