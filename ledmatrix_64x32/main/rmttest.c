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
#include "driver/rmt_tx.h"

void app_main()
{
	rmt_channel_handle_t tx_chan = NULL;
	rmt_tx_channel_config_t tx_chan_config = {
		.clk_src = RMT_CLK_SRC_REF_TICK, //RMT_CLK_SRC_DEFAULT,   // select source clock
		.gpio_num = 27,                    // GPIO number
		.mem_block_symbols = 64,          // memory block size, 64 * 4 = 256 Bytes
		.resolution_hz = 10000, // 1 MHz tick resolution, i.e., 1 tick = 1 µs
		.trans_queue_depth = 1,           // set the number of transactions that can pend in the background
		.flags.invert_out = false,        // do not invert output signal
		.flags.with_dma = false,          // do not need DMA backend
	};
	ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));

    rmt_copy_encoder_config_t encconf = {};
    rmt_encoder_handle_t encoder;
    rmt_new_copy_encoder(&encconf,&encoder);

    rmt_transmit_config_t tx_conf = {
        .loop_count = 0,
        .flags.eot_level = 1,
        .flags.queue_nonblocking = 1,
    };

    rmt_enable(tx_chan);

    rmt_symbol_word_t symbol = {
        .duration0 = 10000,
        .level0 = 1,
        .duration1 = 5000,
        .level1 = 0,
    };
    rmt_symbol_word_t *psym = &symbol;
    uint32_t rawsym = 0x80017002;



    uint64_t step = 0;
    uint64_t maxstep = 8191;
    while(1)
    {
        //printf("Step %d\n",(int)step);
        psym->level0 = 1;
        psym->duration0 = 1000;

        psym->level1 = 0;
        psym->duration1 = 5000;

        printf("%d %d %d %d %d\n",(int)psym->level0,(int)psym->duration0,(int)psym->level1,(int)psym->duration1,(int)psym->val);

        //printf("%d  %d\n",payload[0],payload[1]);
        rmt_transmit(tx_chan, encoder, psym, 4, &tx_conf);
        rmt_tx_wait_all_done(tx_chan,5000);

        // step+=100;
        // if(step > maxstep)
        // {
        //     step=0;
        //     printf("Step = 0\n");
        // }
    }
    
}
