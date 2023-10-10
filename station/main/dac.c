#include "driver/dac_continuous.h"

dac_continuous_handle_t dac_continious_init(int freq)
{
	dac_continuous_handle_t dac_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_ALL,
        .desc_num = 4,
        .buf_size = 2048,
        .freq_hz = freq,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,   // Using APLL as clock source to get a wider frequency range
        /* Assume the data in buffer is 'A B C D E F'
         * DAC_CHANNEL_MODE_SIMUL:
         *      - channel 0: A B C D E F
         *      - channel 1: A B C D E F
         * DAC_CHANNEL_MODE_ALTER:
         *      - channel 0: A C E
         *      - channel 1: B D F
         */
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    /* Allocate continuous channels */
    dac_continuous_new_channels(&cont_cfg, &dac_handle);
	dac_continuous_enable(dac_handle);
	printf("DAC initialized\n");
	return dac_handle;
}


int dac_test()
{
	FILE *fp = fopen("/storage/test2_u8.raw","r");
	if(!fp)
	{
		printf("Cannot open audio file :(\n");
		return;
	}

	dac_continuous_handle_t dac = dac_continious_init(8000);

	uint8_t buf[2048];
	int bytes;
	while(!feof(fp))
	{
		bytes = fread(&buf, 1, 2048, fp);
		dac_continuous_write(dac, buf, bytes, NULL, -1);
	}
	return 0;
}