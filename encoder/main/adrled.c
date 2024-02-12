#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/rmt_tx.h"



rmt_channel_handle_t rmt_init()
{
	int err;
    rmt_channel_handle_t tx_chan = NULL;
    rmt_tx_channel_config_t tx_chan_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,   // select source clock
        .gpio_num = 12,                    // GPIO number
        .mem_block_symbols = 64,          // memory block size, 64 * 4 = 256 Bytes
        .resolution_hz = 10 * 1000 * 1000, // 1 MHz tick resolution, i.e., 1 tick = 1 µs
        .trans_queue_depth = 16,           // set the number of transactions that can pend in the background
        .flags.invert_out = false,        // do not invert output signal
        .flags.with_dma = false,          // do not need DMA backend
    };
	err = rmt_new_tx_channel(&tx_chan_config, &tx_chan);
    if(err)
		printf("RMT init error %d",err);
	rmt_enable(tx_chan);
    return tx_chan;
}

void app_main()
{
    rmt_channel_handle_t tx_chan = rmt_init();

    rmt_symbol_word_t word0 = {
		.duration0 = 3,
		.level0 = 1,
		.duration1 = 9,
		.level1 = 0
    };

    rmt_symbol_word_t word1 = {
        .duration0 = 9,
		.level0 = 1,
		.duration1 = 3,
		.level1 = 0
    };

	rmt_bytes_encoder_config_t bytes_encoder_config = {
        .bit0 = word0,
        .bit1 = word1,
        .flags.msb_first = 1
    };
    rmt_encoder_handle_t encoder_handle;
    rmt_new_bytes_encoder(&bytes_encoder_config, &encoder_handle);

    rmt_transmit_config_t tx_config = {
        .loop_count = 0,
        .flags.eot_level = 0
    };

    uint8_t payload[] = {255,0,0,0,255,0};

	while(1)
	{
		rmt_transmit(tx_chan, encoder_handle, payload, 6,&tx_config);
		vTaskDelay(10/portTICK_PERIOD_MS);	
	}
    

}