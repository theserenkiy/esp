/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define PIN_MISO 25
#define PIN_MOSI 23
#define PIN_CLK  19
#define PIN_CS   22

#define PIN_DC   21
#define PIN_RST  18
#define PIN_BCKL 5

#define NDISP 4


void tx(spi_device_handle_t spi, uint8_t *buf, int nbits)
{
	esp_err_t ret;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	//memset(&t, 0, sizeof(t));     //Zero out the transaction
	t.length=nbits;                    //Len is in bytes, transaction length is in bits.
	t.tx_buffer = buf;               //Data
	t.user=(void*)0;                //D/C needs to be set to 1
	ret=spi_device_polling_transmit(spi, &t);  //Transmit!
	assert(ret==ESP_OK);            //Should have had no issues.
}

void bitbang_spi_setup()
{
	gpio_set_direction(PIN_MOSI, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_CLK, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_CS, GPIO_MODE_OUTPUT);

	gpio_set_level(PIN_CS, 1);
	gpio_set_level(PIN_MOSI, 0);
	gpio_set_level(PIN_CLK, 0);
}

void bitbang_tx(uint8_t *buf,int bits)
{
	gpio_set_level(PIN_CS, 0);
	//printf("CS: %d\n",0);
	int lvl;
	for(int i=0;i < bits;i++)
	{
		//printf("%d\n",i);
		lvl = buf[(int)(i/8)] & (0x1 << (7-(i%8))) ? 1 : 0;
		gpio_set_level(PIN_MOSI, lvl);
		//printf("MOSI: %d\n",lvl);
		gpio_set_level(PIN_CLK, 1);
		//printf("CLK: %d\n",1);
		gpio_set_level(PIN_CLK, 0);
		//printf("CLK: %d\n",0);
	}
	gpio_set_level(PIN_CS, 1);
	//printf("CS: %d\n",1);
}

void app_main(void)
{
	printf("Entering main\n");
	//return;

	esp_err_t ret;
	spi_device_handle_t spi;
	spi_bus_config_t buscfg = {
		.miso_io_num=PIN_MISO,
		.mosi_io_num=PIN_MOSI,
		.sclk_io_num=PIN_CLK,
		.quadwp_io_num=-1,
		.quadhd_io_num=-1,
		.max_transfer_sz= 16*32
	};
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz=1000,
		.mode=0,                                //SPI mode 0
		.spics_io_num=PIN_CS,					//CS pin
		.queue_size=7,                          //We want to be able to queue 7 transactions at a time
		//.pre_cb=lcd_spi_pre_transfer_callback,//Specify pre-transfer callback to handle D/C line
	};
	//Initialize the SPI busz
	printf("Init SPI bus\n");
	ret=spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
	printf("ret=%d\n", ret);
	ESP_ERROR_CHECK(ret);
	//Attach the LCD to the SPI bus
	printf("Add SPI device\n");
	ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
	printf("ret=%d\n", ret);
	ESP_ERROR_CHECK(ret);

	int freq = spi->real_clk_freq_hz;
	//printf("SPI clk= %d\n", freq);

	//uint8_t digits[][] = {{}}


	uint8_t data[][2] = {
		{0x0C,0x01},
		{0x0A,0x01},
		{0x0B,0x07},
		//{0x0F,0x01},
		{0x01,0x08},
		{0x02,0x07},
		{0x03,0x06},
		{0x04,0x05},
		{0x05,0x04},
		{0x06,0x03},
		{0x07,0x02},
		{0x08,0x01},
		{0xff,0x00}
	};

	uint8_t packet[NDISP][2];

	bitbang_spi_setup();
	
	for(int i=0;; i++)
	{
		if(data[i][0]==0xff)
			break;
		for(int j=0; j < NDISP; j++)
		{
			packet[j][0] = data[i][0];
			packet[j][1] = data[i][1];
		}
		tx(spi,(uint8_t *)packet,NDISP*2*8);
	}
	
	for(int i=0;i < 8; i++)
	{
		for(int j=0; j < NDISP; j++)
		{
			packet[j][0] = i+1;
			packet[j][1] = 1 << j | 1 << (j+1);
		}
		tx(spi,(uint8_t *)packet,NDISP*2*8);
	}
}
