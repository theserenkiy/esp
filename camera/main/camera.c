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
#include "esp_heap_caps.h"
#include "sccb.c"

//#define FRAME_BUFFER_SIZE 10000

//uint8_t frame[100000];

#define HSYNC_MAX_DELAY_US 1000	

static QueueHandle_t vsync_queue = NULL;

static uint16_t last_hsync = 0;
static uint8_t line_active = 0;
static void IRAM_ATTR on_hsync(void *arg)
{
	static uint64_t curtime;
	curtime = esp_timer_get_time();
	if(curtime > last_hsync + HSYNC_MAX_DELAY_US)
	{
		xQueueSendFromISR(vsync_queue, &curtime, NULL);	
	}
	last_hsync = curtime;
}

void vsync_task(void *arg)
{
	uint64_t time;
	while(1)
	{
		if(xQueueReceive(vsync_queue, &time, portMAX_DELAY))
		{
			//printf("new frame @ %llu\n",time);
		}
	}
}

void memtest()
{
	printf("All heaps free size: %dk\n",(int)(heap_caps_get_free_size(MALLOC_CAP_8BIT)/1024));
	printf("Checking frame buffer...\n");
	uint8_t *ptr;
	for(int i=0;i < 40; i++)
	{
		ptr = malloc(8192);
		printf("Allocated %dk; result: %p\n",i*8,ptr);
		if(!ptr)
			break;
	}
}

void dumpreg(uint8_t addr)
{
	uint8_t result = sccb_read(SCCB_ID,addr);
	printf("Reg 0x%x = 0x%x\n",addr,result);
}

void app_main()
{
	ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_HIGH_SPEED_MODE, 
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_1_BIT,
        .freq_hz          = 10000000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
	ledc_channel_config_t ledc_channel_0 = {
        .speed_mode     = LEDC_HIGH_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN_MCLK,
        .duty           = 1, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_0);

	sccb_init();
	usleep(100);

	//COM7
	sccb_write(SCCB_ID,0x12,(1 << 7));	//register reset
	usleep(1000);

	sccb_write(SCCB_ID,0x11,5);					//Divide CLK by
	//COM15
	sccb_write(SCCB_ID,0x40,(0b11 << 4));			//RGB 555
	//COM7
	sccb_write(SCCB_ID,0x12,(1 << 3));// | (1 << 2));	//QCIF resolution, YUV // RGB



	vsync_queue = xQueueCreate(2, sizeof(uint32_t));
	xTaskCreate(vsync_task,"vsync_task",2048,NULL,10,NULL);

	gpio_install_isr_service(0);
	gpio_reset_pin(PIN_HSYNC);
	gpio_set_direction(PIN_HSYNC,GPIO_MODE_INPUT);
	gpio_set_intr_type(PIN_HSYNC, GPIO_INTR_POSEDGE);
	gpio_isr_handler_add(PIN_HSYNC, on_hsync, NULL);
	

	while(1)
	{
		dumpreg(0x0A);
		dumpreg(0x0B);
		dumpreg(0x11);
		dumpreg(0x12);
		dumpreg(0x40);
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}