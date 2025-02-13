
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"


#define PIN_A 21
#define PIN_B 19
#define PIN_C 23
#define PIN_D 22

#define PIN_R1 15
#define PIN_R2 17

#define PIN_G1 5
#define PIN_G2 16

#define PIN_B1 2
#define PIN_B2 18

#define PIN_LAT 13
#define PIN_CLK 12
#define PIN_OE 14

int pins[] = {PIN_A,PIN_B,PIN_C,PIN_D,PIN_LAT,PIN_OE};

spi_device_handle_t spi;

uint8_t row[] = {0xff,0x21,0x0c,0xff,0x21,0x0c,0xfd,0x30,0x0c,0xfd,0x30,0x0c,0xfd,0x30,0x0c,0xff,0x42,0x0b,0xff,0x42,0x0b,0xff,0x57,0x0a,0xff,0x57,0x0a,0xfd,0x6f,0x07,0xfd,0x6f,0x07,0xff,0x85,0x03,0xff,0x85,0x03,0xff,0x85,0x03,0xff,0x9e,0x02,0xff,0x9e,0x02,0xfc,0xb2,0x04,0xfc,0xb2,0x04,0xec,0xc4,0x0b,0xec,0xc4,0x0b,0xec,0xc4,0x0b,0xd7,0xd3,0x16,0xd7,0xd3,0x16,0xb9,0xde,0x24,0xb9,0xde,0x24,0x93,0xe5,0x35,0x93,0xe5,0x35,0x60,0xea,0x49,0x60,0xea,0x49,0x60,0xea,0x49,0x02,0xeb,0x5f,0x02,0xeb,0x5f,0x00,0xe9,0x77,0x00,0xe9,0x77,0x00,0xe3,0x91,0x00,0xe3,0x91,0x00,0xe3,0x91,0x00,0xd9,0xaa,0x00,0xd9,0xaa,0x00,0xca,0xc0,0x00,0xca,0xc0,0x00,0xb6,0xd2,0x00,0xb6,0xd2,0x01,0xa0,0xdf,0x01,0xa0,0xdf,0x01,0xa0,0xdf,0x00,0x86,0xe7,0x00,0x86,0xe7,0x11,0x6e,0xe9,0x11,0x6e,0xe9,0x4b,0x58,0xe8,0x4b,0x58,0xe8,0x4b,0x58,0xe8,0x67,0x44,0xe2,0x67,0x44,0xe2,0x80,0x36,0xdf,0x80,0x36,0xdf,0x95,0x2b,0xd9,0x95,0x2b,0xd9,0xa7,0x24,0xd2,0xa7,0x24,0xd2,0xa7,0x24,0xd2,0xb6,0x20,0xcf,0xb6,0x20,0xcf};

void spi_tx(uint8_t *buf, int nbits)
{
	esp_err_t ret;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	//memset(&t, 0, sizeof(t));     //Zero out the transaction
	t.length=nbits;                    //Len is in bytes, transaction length is in bits.
	t.tx_buffer = buf;
	t.rx_buffer = NULL;
	t.user=(void*)0;
	t.flags = (SPI_TRANS_MODE_QIO);
	ret=spi_device_polling_transmit(spi, &t);  //Transmit!
	assert(ret==ESP_OK);            //Should have had no issues.
}

void setLineAddr(int addr)
{
	gpio_set_level(PIN_OE,1);
	gpio_set_level(PIN_D,addr & 0b1000 ? 1 : 0);
	gpio_set_level(PIN_C,addr & 0b0100 ? 1 : 0);
	gpio_set_level(PIN_B,addr & 0b0010 ? 1 : 0);
	gpio_set_level(PIN_A,addr & 0b0001 ? 1 : 0);
	gpio_set_level(PIN_LAT,1);
	gpio_set_level(PIN_LAT,0);
	gpio_set_level(PIN_OE,0);
	usleep(1);
}

void app_main(void)
{
	for(int i=0;i < 6;i++)
	{
		gpio_reset_pin(pins[i]);
		gpio_set_direction(pins[i],GPIO_MODE_OUTPUT);
		gpio_set_level(pins[i],0);
	}

	esp_err_t ret;
	spi_bus_config_t buscfg = {
		.mosi_io_num=PIN_R1,
		.miso_io_num=PIN_G1,
		.quadwp_io_num=PIN_B1,
		.quadhd_io_num=PIN_R2,
		// .data0_io_num=PIN_R1,
		// .data1_io_num=PIN_G1,
		// .data2_io_num=PIN_B1,
		// .data3_io_num=PIN_R2,
		.data4_io_num=PIN_G2,
		.data5_io_num=PIN_B2,
		.data6_io_num=0,
		.data7_io_num=1,
		.sclk_io_num=PIN_CLK,
		.max_transfer_sz=4092,
		.flags=(SPICOMMON_BUSFLAG_MASTER | SPICOMMON_BUSFLAG_QUAD)
	};
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz=1000000,
		.mode=0,                                //SPI mode 0
		.spics_io_num=-1,					//CS pin
		.queue_size=7,                          //We want to be able to queue 7 transactions at a time
		.flags = SPI_DEVICE_HALFDUPLEX
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

	uint8_t buf[64];

	for(int i=0; i < 64; i++)
	{
		buf[i] = i;//(row[i*3] >> 7) | ((row[i*3+1] & 0x80) >> 6) | ((row[i*3+2] & 0x80) >> 5);
	}

	while(1){
		for(int i=0; i < 16; i++)
		{
			spi_tx(buf,64);
			setLineAddr(i);
		}
		gpio_set_level(PIN_OE,1);
		vTaskDelay(10/portTICK_PERIOD_MS);
	}
	
}
