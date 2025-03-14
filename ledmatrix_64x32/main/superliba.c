#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "inttypes.h"

typedef struct {
    uint8_t mosi;
    uint8_t miso;
    uint8_t clk;
    uint8_t cs;
    uint8_t d0;
    uint8_t d1;
    uint8_t d2;
    uint8_t d3;
} spi_pins_t;   

typedef struct {

} spi_conf_t;

int spi_init(uint8_t num, int freq, uint8_t pin_clk, uint8_t pin_cs, uint8_t pin_mosi, uint8_t pin_miso)
{
    esp_err_t ret;
	spi_bus_config_t buscfg = {
        .miso_io_num = pin_miso,
        .mosi_io_num = pin_mosi,
        .sclk_io_num = pin_clk,
		.max_transfer_sz=4092,
		//.flags=(SPICOMMON_BUSFLAG_MASTER)
	};
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz=100000,
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
