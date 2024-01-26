#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/ledc.h"

#define SCL 22
#define SDA 23

void i2c_init()
{
	gpio_set_direction(SCL,GPIO_MODE_INPUT);
	gpio_set_pull_mode(SCL,GPIO_PULLUP_ONLY);
	gpio_set_direction(SDA,GPIO_MODE_INPUT);
	gpio_set_pull_mode(SDA,GPIO_PULLUP_ONLY);
}


void i2c_start()
{
	gpio_set_level(SDA,0);
	gpio_set_direction(SDA,GPIO_MODE_OUTPUT);
	usleep(1);
	gpio_set_level(SCL,0);
	gpio_set_direction(SCL,GPIO_MODE_OUTPUT);
}

void i2c_stop()
{
	gpio_set_level(SDA,0);
	usleep(1);
	gpio_set_level(SCL,0);
	usleep(1);
	gpio_set_direction(SCL,GPIO_MODE_INPUT);
	usleep(1);
	gpio_set_direction(SDA,GPIO_MODE_INPUT);
	usleep(2);
}

int i2c_send_byte(uint8_t data)
{
	for(int i=7; i >= 0; i--)
	{
		gpio_set_level(SDA,data & 1 << i ? 1 : 0);
		usleep(1);
		gpio_set_level(SCL,1);
		usleep(2);
		gpio_set_level(SCL,0);
	}
	gpio_set_direction(SDA,GPIO_MODE_INPUT);
	usleep(1);
	gpio_set_level(SCL,1);
	usleep(1);
	int ack = gpio_get_level(SDA);
	usleep(1);
	gpio_set_level(SCL,0);
	usleep(1);
	gpio_set_direction(SDA,GPIO_MODE_OUTPUT);
	return ack;
}

void oled_cmd(uint8_t cmd)//, uint8_t arg)
{
	uint8_t ack1,ack2,ack3,ack4=1;
	i2c_start();
	ack1 = i2c_send_byte(0x78);
	ack2 = i2c_send_byte((1 << 7));
	ack3 = i2c_send_byte(cmd);
	// if(arg >= 0)
	// 	ack3 = i2c_send_byte(arg);

	i2c_stop();
	printf("Cmd: 0x%x; Acks: %u %u %u %u\n",cmd,ack1,ack2,ack3,ack4);
}


void app_main()
{
	i2c_init();

	//uint8_t inverse = 0;
	//uint8_t contrast = 0x7f;
	oled_cmd(0xAE);
	oled_cmd(0x8D);
	oled_cmd(0x14);	//enabling charge pump
	oled_cmd(0xAF);	//display ON
	for(int i=0;;i++)
	{
		oled_cmd((i & 2) ? 0xA4 : 0xA5);	//entire display ON
		oled_cmd((i & 1) ? 0xA6 : 0xA7);	//set inverse
		//inverse = inverse ? 0 : 1;
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}