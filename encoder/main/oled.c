#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/ledc.h"
#include "font.c"

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
	ack2 = i2c_send_byte(1 << 7);
	ack3 = i2c_send_byte(cmd);
	// if(arg >= 0)
	// 	ack3 = i2c_send_byte(arg);

	i2c_stop();
	printf("Cmd: 0x%x; Acks: %u %u %u %u\n",cmd,ack1,ack2,ack3,ack4);
}

void oled_data(uint8_t *buf, uint16_t len)//, uint8_t arg)
{
	uint8_t ack1,ack2,ack3,ack4=1;
	i2c_start();
	i2c_send_byte(0x78);
	i2c_send_byte(0b01 << 6);
	for(uint16_t i=0; i < len; i++)
		i2c_send_byte(buf[i]);
	// if(arg >= 0)
	// 	ack3 = i2c_send_byte(arg);

	i2c_stop();
	printf("Data sent\n");
}

void oled_clear()
{
	int buf[128];
	int i;
	for(i=0;i < 128; i++)
	{
		buf[i] = 0;
	}

	oled_cmd(0x21);
	oled_cmd(0);
	oled_cmd(127);

	for(i=0; i < 8; i++)
	{
		oled_cmd(0x22);
		oled_cmd(i);
		oled_cmd(i);
		oled_data(buf,128);
	}
}

void oled_write_range(int x0, int x1, int y0, int y1, uint8_t *buf)
{
	oled_cmd(0x21);
	oled_cmd(x0);
	oled_cmd(x1);

	for(int i=0; i <= (y1-y0); i++)
	{
		oled_cmd(0x22);
		oled_cmd(i+y0);
		oled_cmd(i+y0);
		oled_data(buf+((x1-x0+1)*i),x1-x0+1);
	}
}

int oled_write_glyph(int x, int y, int height, uint32_t* gl)
{
	uint8_t glbuf[256];
	//row is 8-bit height 
	int rows = (int)(height/8)+(height%8 ? 1 : 0);
	int yoffs, ymax, bufoffs, bufaddr;
	for(int row=0;row < rows; row++)
	{
		yoffs = row*8;
		ymax = row < rows-1 ? 8 : height%8;
		bufoffs = gl[0]*row;
		for(int col=0; col < gl[0]; col++)
		{
			bufaddr = bufoffs+col;
			glbuf[bufaddr] = 0;
			for(int i=0; i < ymax; i++)
			{
				if(gl[i+yoffs+1] & (1 << (31-col)))
					glbuf[bufaddr] |= 1 << i;
			}
		}
	}

	oled_write_range(x,x+gl[0]-1,y,y+rows-1,glbuf);
	return gl[0];
}

void oled_write_char(int x, int y, int height, char c)
{
	uint8_t found = 0;
	for(int i=0; i < sizeof(font2_symlist);i++)
	{
		
	}
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
	oled_cmd(0xA4);	//entire display ON 
	
	oled_clear();
	
	uint8_t buf[16];

	for(int i=0; i < 16; i++)
	{
		buf[i] = 0xff;
	}

	oled_write_glyph(64,1,10,font2_glyph_1);





	// for(int i=0;;i++)
	// {
	// 	//oled_cmd((i & 2) ? 0xA4 : 0xA5);	//entire display ON
	// 	//oled_cmd((i & 1) ? 0xA6 : 0xA7);	//set inverse
	// 	oled_data(buf,128);
	// 	//inverse = inverse ? 0 : 1;
	// 	vTaskDelay(1000/portTICK_PERIOD_MS);
	// }
}