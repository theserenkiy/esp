idf_component_register(SRCS "adrled.c"
                    INCLUDE_DIRS ""
                    #REQUIRES esp_driver_rmt
                    )

                    

#fatfs_create_spiflash_image(files files FLASH_IN_PROJECT)