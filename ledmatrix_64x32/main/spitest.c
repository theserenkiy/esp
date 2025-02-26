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

spi_device_handle_t spi0=NULL, spi1=NULL;

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
	t.flags = 0;//(SPI_TRANS_MODE_QIO);
    pt = &t;
	spi_device_queue_trans(spi0, pt, portMAX_DELAY);
	//spi_device_queue_trans(spi1, &t, portMAX_DELAY);
	spi_device_get_trans_result(spi0, &pt, portMAX_DELAY);
	usleep(100);
	// ret=spi_device_polling_transmit(spi, &t);  //Transmit!
	// assert(ret==ESP_OK);            //Should have had no issues.
}

void spi_init(spi_device_handle_t * spi, spi_host_device_t host, int mode)
{
	esp_err_t ret;
	spi_bus_config_t buscfg = {
		.miso_io_num = -1,
        .mosi_io_num = 27,
        .sclk_io_num = -1,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 1024,
	};
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz=1,
		.mode=mode,
		.spics_io_num=-1,
		.queue_size=1,
		.flags = SPI_DEVICE_HALFDUPLEX
	};
	if(spi)
		spi_bus_remove_device(*spi);
	spi_bus_free(SPI3_HOST);
	vTaskDelay(100/portTICK_PERIOD_MS);
	ret=spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(ret);
	ret=spi_bus_add_device(host, &devcfg, spi);
	ESP_ERROR_CHECK(ret);
}

void app_main(void)
{
    uint8_t data[] = {
        0xff, 0x00, 0xff
    };

	gpio_reset_pin(27);
	gpio_set_direction(27, GPIO_MODE_OUTPUT);
	gpio_set_drive_capability(27,GPIO_DRIVE_CAP_3);
	gpio_set_level(27,1);
    


    while(1)
    {
		for(int mode=0; mode < 4; mode++)
		{
			printf("Mode: %d\n",mode);
			spi_init(&spi0, SPI3_HOST, mode);
			spi_tx(data, 24);
        	vTaskDelay(1000/portTICK_PERIOD_MS);
			spi_tx(data, 24);
        	vTaskDelay(1000/portTICK_PERIOD_MS);
			
		}
        
    }
}
