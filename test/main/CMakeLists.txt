idf_component_register(SRCS "test_main.c"
                    INCLUDE_DIRS "." "C:/esplib")

        
fatfs_create_spiflash_image(files files FLASH_IN_PROJECT)