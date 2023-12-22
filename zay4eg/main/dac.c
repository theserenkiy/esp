#include "driver/dac_continuous.h"
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_system.h"
#include <string.h>

static uint8_t buf[2048];

// не используем выравнивание износа
static wl_handle_t s_wl_handle = WL_INVALID_HANDLE;

//монтирует раздел partname в путь mountpoint
int mount_partition(const char *partname, const char *mountpoint)
{
    const esp_vfs_fat_mount_config_t mount_config = {
		.max_files = 4,					//максимальное кол-во одновременно открытых файлов
		.format_if_mount_failed = true,	//форматировать раздел, если он еще не существует
		.allocation_unit_size = 0		//размер кластера равен размеру сектора
	};
	esp_err_t err;

	//монтируем раздел для чтения и записи
	err = esp_vfs_fat_spiflash_mount_rw_wl(mountpoint, partname, &mount_config, &s_wl_handle);

	if (err != ESP_OK)
	{
		printf("Failed to mount FATFS (%s)\n", esp_err_to_name(err));
		return -1;
	}
    return 0;
}


dac_continuous_handle_t dac_continious_init(int freq)
{
	dac_continuous_handle_t dac_handle;
    dac_continuous_config_t cont_cfg = {
        .chan_mask = DAC_CHANNEL_MASK_CH0,
        .desc_num = 4,
        .buf_size = 2048,
        .freq_hz = freq,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_APLL,   
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };
    /* Allocate continuous channels */
    dac_continuous_new_channels(&cont_cfg, &dac_handle);
	dac_continuous_enable(dac_handle);
	printf("DAC initialized\n");
	return dac_handle;
}


