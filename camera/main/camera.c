#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

#include "common.h"
#include "sccb.c"


void app_main()
{
	sccb_init();
	while(1)
	{
		sccb_transaction(SCCB_ID,0,0x12,0x80);
		for(int addr = 0x0A; addr <= 0x10; addr++)
		{
			printf("Send command %x\n",addr);
			uint8_t result = sccb_transaction(SCCB_ID,1,addr,0);
			printf("result: 0x%x\n", result);

			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
		
	}
}