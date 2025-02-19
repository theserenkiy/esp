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
#include "soc/gpio_struct.h"
#include "driver/gptimer.h"
#include "image.c"
#include "gamma.c"

spi_device_handle_t spi0, spi1;

void spi_tx(uint8_t *buf, int nbits)
{
	esp_err_t ret;
	spi_transaction_t t;
    spi_transaction_t * pt;
	memset(&t, 0, sizeof(t));
	//memset(&t, 0, sizeof(t));     //Zero out the transaction
	t.length=nbits;                    //Len is in bytes, transaction length is in bits.
	t.addr=-1;
	t.cmd=-1;
	t.tx_buffer = buf;
	t.rx_buffer = NULL;
	t.user=(void*)0;
	t.flags = (SPI_TRANS_MODE_QIO);
    pt = &t;
	spi_device_queue_trans(spi0, pt, portMAX_DELAY);
	//spi_device_queue_trans(spi1, &t, portMAX_DELAY);
	spi_device_get_trans_result(spi0, &pt, portMAX_DELAY);
	usleep(100);
	// ret=spi_device_polling_transmit(spi, &t);  //Transmit!
	// assert(ret==ESP_OK);            //Should have had no issues.
}

void spi_init(spi_device_handle_t * spi, spi_host_device_t host, int *data_pins, int clk)
{
	esp_err_t ret;
	spi_bus_config_t buscfg = {
		.data0_io_num=data_pins[0],
		.data1_io_num=data_pins[1],
		.data2_io_num=data_pins[2],
		.data3_io_num=data_pins[3],
		.data4_io_num=data_pins[4],
		.data5_io_num=data_pins[5],
		.data6_io_num=data_pins[6],
		.data7_io_num=data_pins[7],
		.sclk_io_num=clk,
		.max_transfer_sz=4092,
		.flags=(SPICOMMON_BUSFLAG_MASTER)
	};
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz=1,
		.mode=1,
		.spics_io_num=-1,
		.queue_size=1,
		.flags = SPI_DEVICE_HALFDUPLEX
	};
	ret=spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(ret);
	ret=spi_bus_add_device(host, &devcfg, spi);
	ESP_ERROR_CHECK(ret);
}

void app_main(void)
{
	// for(int i=0;i < 13;i++)
	// {
	// 	gpio_reset_pin(pins[i]);
	// 	gpio_set_direction(pins[i],GPIO_MODE_OUTPUT);
	// 	gpio_set_level(pins[i],0);
	// }

    uint8_t pattern[] = {
        0b00001001,0b00001001,0b00001001,0b00001001,0b00001001,0b00001001,0b00001001,0b00001001,
        0b00000110,0b00000110,0b00000110,0b00000110,0b00000110,0b00000110,0b00000110,0b00000110
    };

    uint8_t data[64];

    int pins[] = {26,25,33,32,13,14,15,16};
    spi_init(&spi0, SPI3_HOST, pins, -1);


    for(int i=0;i < 64;i++)
    {
        data[i] = pattern[i%16];
    }

    while(1)
    {
        spi_tx(data, 64);
        vTaskDelay(500/portTICK_PERIOD_MS);
    }
}
