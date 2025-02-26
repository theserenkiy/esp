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
		.clk_src = RMT_CLK_SRC_REF_TICK,   // select source clock
		.gpio_num = 27,                    // GPIO number
		.mem_block_symbols = 64,          // memory block size, 64 * 4 = 256 Bytes
		.resolution_hz = 10000, // 1 MHz tick resolution, i.e., 1 tick = 1 µs
		.trans_queue_depth = 4,           // set the number of transactions that can pend in the background
		.flags.invert_out = false,        // do not invert output signal
		.flags.with_dma = false,          // do not need DMA backend
	};
	ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &tx_chan));
	
	rmt_enable(tx_chan);

	rmt_copy_encoder_config_t encconf = {};
	rmt_encoder_handle_t enc;
	rmt_new_copy_encoder(&encconf,&enc);

	rmt_transmit_config_t txconf = {
		.loop_count = 0,
		.flags.eot_level = 1,
		.flags.queue_nonblocking = 1,
	};

	uint32_t data = 0xf0f0f1f1;//((5000 << 17) | 1) | ((5000 << 17) | 0);

	while(1)
	{
		rmt_transmit(tx_chan, enc, &data, 4, &txconf);
		rmt_tx_wait_all_done(tx_chan, 5000);
	}
}
