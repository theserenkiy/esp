#include <stdio.h>
#include <inttypes.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
//#include "driver/dedic_gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include "soc/gpio_reg.h"

#include "common.h"
#include "esp_heap_caps.h"
#include "sccb.c"

//#define FRAME_BUFFER_SIZE 10000

//uint8_t frame[100000];

#define FRAME_DETECT_DELAY_US 20000	

static QueueHandle_t vsync_queue = NULL;

volatile static uint64_t last_pclk = 0;
volatile static uint64_t curtime;
volatile static uint64_t delay;
volatile static uint32_t pixel_cnt = 0;
volatile static uint32_t row_cnt = 0;
volatile static uint32_t pixel_cnt_last = 0;
volatile static uint64_t frame_updated = 0;
static uint8_t line_active = 0;
static uint8_t frame_captured = 0;

volatile static uint8_t video[240][315]; 
volatile static uint16_t pcounts[240]; 

volatile static uint32_t qqq;

static void IRAM_ATTR on_pclk(void *arg)
{
	pixel_cnt++;
}

static void IRAM_ATTR on_hsync(void *arg)
{
	row_cnt++;
}

static void IRAM_ATTR on_vsync(void *arg)
{
	pixel_cnt_last = pixel_cnt;
	pixel_cnt = 0;
}

void vsync_task(void *arg)
{
	uint64_t time;
	while(1)
	{
		if(xQueueReceive(vsync_queue, &time, portMAX_DELAY))
		{
			printf("new frame @ %llu\n",time);
			printf("pixels read: %lu\n",pixel_cnt_last);
			pixel_cnt = 0;
		}
	}
}


void dumpreg(uint8_t addr)
{
	uint8_t result = sccb_read(SCCB_ID,addr);
	printf("Reg 0x%x = 0x%x\n",addr,result);
}

int calcPixels()
{
	vTaskSuspendAll();
	int pval,plastval = 1,hval,hlastval = 0, pcnt=0, hcnt=0;
	
	while(!GPIO_READ(PIN_VSYNC));
	while(GPIO_READ(PIN_VSYNC));

	while(hcnt < 240)
	{
		while(!GPIO_READ(PIN_HSYNC));
		while(GPIO_READ(PIN_HSYNC))
		{
			pval = GPIO_READ(PIN_PCLK);
			if(pval && !plastval)
			{
				if((pcnt%2))
					video[hcnt][pcnt >> 1] = (uint8_t)(REG_READ(GPIO_IN1_REG) << 3);
				pcnt++;
			}
			plastval = pval;
		}
		// if(!plastval)
		// 	pcnt++;
		// else
		plastval = 0;
		pcounts[hcnt] = pcnt;
		
		pcnt = 0;
		hcnt++;
	}
	
	// char s[1261];
	// s[1260] = 0;
	//s[1261] = 0;

	// for(int i = 0; i < 240; i++)
	// {
	// 	for(int j=0;j < 315; j++)
	// 	{
	// 		fwrite("%02x",video[i][j*2]);
	// 	}	
	// 	printf(" ");
	// }

	// printf("%x%x",1234,5678);
	// printf("%x%x",1234,5678);
	// printf("%x%x",1234,5678);
	// printf(" ");
	//printf(" ");

	video[239][314] = 0xff;
	fwrite(video,1,240*315,stdout);
	fflush(stdout);
	xTaskResumeAll();
	return 0;
}

void read_bench(int iters)
{
	uint32_t ioreg = 0,delay;
	uint64_t start = esp_timer_get_time();
	for(int i=0; i < iters; i++)
	{
		//ioreg+= 1 + qqq;
		ioreg = GPIO_READ(19);
	}
	delay = esp_timer_get_time()-start;
	printf("Total delay: %lu; iter delay: %.5f, ioreg: %lu\n",delay,(float)delay/iters,ioreg);
}

void app_main()
{
	// gpio_reset_pin(19);
	// gpio_set_direction(19,GPIO_MODE_INPUT);
	// gpio_set_pull_mode(19,GPIO_PULLUP_ONLY);

	// gpio_reset_pin(PIN_PCLK);
	// gpio_set_direction(PIN_PCLK,GPIO_MODE_INPUT);


	// while(1)
	// {
	// 	benchmark_gpio();
	// 	vTaskDelay(10);
	// }
	
	
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

	sccb_write(SCCB_ID,0x11,4);					//Divide CLK by
	//COM15
	//sccb_write(SCCB_ID,0x40,(0b01 << 4));			//RGB 555
	//COM7
	sccb_write(SCCB_ID,0x12,(1 << 4));// | (1 << 2));	//QVGA resolution, YUV // RGB
	//COM10
	sccb_write(SCCB_ID,0x15,(1 << 5));// disable PCLK for hor blank 


	//vsync_queue = xQueueCreate(2, sizeof(uint32_t));
	//xTaskCreate(vsync_task,"vsync_task",2048,NULL,10,NULL);

	gpio_install_isr_service(0);
	
	gpio_reset_pin(PIN_VSYNC);
	gpio_set_direction(PIN_VSYNC,GPIO_MODE_INPUT);
	// gpio_set_intr_type(PIN_VSYNC, GPIO_INTR_POSEDGE);
	// gpio_isr_handler_add(PIN_VSYNC, on_vsync, NULL);

	gpio_reset_pin(PIN_HSYNC);
	gpio_set_direction(PIN_HSYNC,GPIO_MODE_INPUT);
	// gpio_set_intr_type(PIN_HSYNC, GPIO_INTR_POSEDGE);
	// gpio_isr_handler_add(PIN_HSYNC, on_hsync, NULL);


	gpio_reset_pin(PIN_PCLK);
	gpio_set_direction(PIN_PCLK,GPIO_MODE_INPUT);
	//gpio_set_intr_type(PIN_PCLK, GPIO_INTR_POSEDGE);
	//gpio_isr_handler_add(PIN_PCLK, on_pclk, NULL);
	//gpio_set_level();
	
	int scan1, scan2;

	// pcnt_unit_config_t unit_config = {
	// 	.high_limit = 200000,
	// 	.low_limit = 0,
	// };
	// pcnt_unit_handle_t pcnt_unit = NULL;
	// pcnt_new_unit(&unit_config, &pcnt_unit);
	

	// const int bundleA_gpios[] = {0, 1};
	// gpio_config_t io_conf = {
	// 	.mode = GPIO_MODE_OUTPUT,
	// };
	// for (int i = 0; i < sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]); i++) {
	// 	io_conf.pin_bit_mask = 1ULL << bundleA_gpios[i];
	// 	gpio_config(&io_conf);
	// }
	// // Create bundleA, output only
	// dedic_gpio_bundle_handle_t bundleA = NULL;
	// dedic_gpio_bundle_config_t bundleA_config = {
	// 	.gpio_array = bundleA_gpios,
	// 	.array_size = sizeof(bundleA_gpios) / sizeof(bundleA_gpios[0]),
	// 	.flags = {
	// 		.out_en = 1,
	// 	},
	// };
	// ESP_ERROR_CHECK(dedic_gpio_new_bundle(&bundleA_config, &bundleA));

	for(int i=0; i < DATA_WIDTH;i++)
	{
		gpio_reset_pin(PIN_HDATA-i);
		gpio_set_direction(PIN_HDATA-i, GPIO_MODE_DEF_INPUT);
	}

	int last_pclk_state = 0;
	uint32_t ioreg;

	while(1)
	{
		calcPixels();

		// vTaskDelay(5/portTICK_PERIOD_MS);
		// calcPixels();
		// printf("Scan1: %d; Scan2: %d\n",scan1,scan2);
		//printf("Pixels in row: %d; Rows in frame: %d\n",scan1,scan2);
		// dumpreg(0x0A);
		// dumpreg(0x0B);
		// dumpreg(0x11);
		// dumpreg(0x12);
		// dumpreg(0x15);
		// dumpreg(0x40);
		//printf("Pixel cnt last: %lu  @  %llu\n",pixel_cnt_last, frame_updated);

		
		
		// if(pixel_cnt_last)
		// {
		// 	printf("Last frame pixels: %lu\n",pixel_cnt_last);
		// 	pixel_cnt_last = 0;
		// }
		
		// ioreg = REG_READ(GPIO_IN1_REG);
		// printf(""BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n",
		// 	BYTE_TO_BINARY(ioreg >> 24),BYTE_TO_BINARY(ioreg >> 16), BYTE_TO_BINARY(ioreg >> 8),BYTE_TO_BINARY(ioreg)
		// );
		// fflush(stdout);

		//read_bench(1000000);

		vTaskDelay(100/portTICK_PERIOD_MS);
	}
}