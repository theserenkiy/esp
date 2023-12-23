#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t dac_queue = NULL;
volatile int dac_active = 0;

#include "dac.c"

#define DEVICE_TEST


#include "distance_reaction.c"

#define TX_PIN 26
#define RX_PIN 27

#define DIST_FILTER_SIZE 8


volatile uint64_t sonar_tx_time;
volatile float distance;
static int filenum = 1;

uint64_t time_us()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000000+tv.tv_usec;
}

static void IRAM_ATTR sonar_on_rx(void *arg)
{
	static uint32_t time;
	static uint64_t curtime;
	curtime = time_us();
	if(gpio_get_level(RX_PIN))
	{
		sonar_tx_time = curtime;
		return;
	}
	time = (uint32_t)(curtime-sonar_tx_time);
	xQueueSendFromISR(gpio_evt_queue, &time, NULL);
}

void measurement_task(void *arg)
{
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

	uint32_t time;
	float dist;
	float dist_filter[DIST_FILTER_SIZE];
	uint8_t	df_ptr = 0;
	float df_min = 0;
	uint64_t curtime;
	int pinval;
	while(1)
	{
		gpio_set_level(TX_PIN,1);
		usleep(9);
		gpio_set_level(TX_PIN,0);

		if(xQueueReceive(gpio_evt_queue, &time, portMAX_DELAY))
		{
			dist = ((float)time)/5882;
			dist_filter[(df_ptr++) % DIST_FILTER_SIZE] = dist;
			df_min = 5;
			for(int i=0; i < DIST_FILTER_SIZE; i++)
			{
				if(dist_filter[i] < df_min)
					df_min = dist_filter[i];
			}
			distance = df_min;

			//printf("%.3f	%.3f\n", dist, df_min);

			if(dist < 4)
			 	vTaskDelay(20/portTICK_PERIOD_MS);
		}
	}
}



int dac_task(void *arg)
{
	dac_queue = xQueueCreate(10, sizeof(uint32_t));

	dac_continuous_handle_t dac = dac_continious_init(16000);
	FILE *fp;
	char fname[32];
	int bytes;
	int first_read = 0;
	float step;
	int ramp_startpos;
	int filenum;
	for(;;)
	{
		if(xQueueReceive(dac_queue, &filenum, portMAX_DELAY))
		{
			dac_active = 1;
			sprintf(fname,"/files/%02d.wav",filenum);
			printf("Filename %s\n",fname);
			fp = fopen(fname,"r");
			if(!fp)
			{
				printf("Cannot open audio file :(\n");
				return -1;
			}
			first_read = 1;

			while(!feof(fp))
			{
				bytes = fread(&buf, 1, 2048, fp);
				//ramping begin of the file
				if(first_read)
				{
					step = ((float)buf[256]-128)/256;
					for(int j=0;j < 256;j++)
					buf[j] = 128+(int)(j*step);
					memset(buf, 128, 256);
				}
				first_read=0;
				//printf("Bytes read: %d\n",bytes);

				//filling the rest of buffer with zeros if necessary
				if(bytes < 2048)
				{
					ramp_startpos = bytes > 256 ? bytes-256 : 0;
					step = ((float)buf[ramp_startpos]-128)/256;
					for(int j=0;j < 256;j++)
					buf[j] = 128+(int)((256-j)*step);
					//memset(buf, 128, 256);
					memset(buf+bytes,128,2048-bytes);
				}

				// DAC write
				dac_continuous_write(dac, buf, 2048, NULL, -1);
			}
			fclose(fp);
			dac_active = 0;
		}
	}
	
	return 0;
}


void app_main(void)
{
	printf("Hello world!\n");

	distance_reaction_init();

	if(mount_partition("files","/files") < 0)
	{
		printf("Failed to mount partition\n");
		return;
	}

	gpio_install_isr_service(0);

	gpio_reset_pin(TX_PIN);
	gpio_set_direction(TX_PIN, GPIO_MODE_OUTPUT);

	gpio_reset_pin(RX_PIN);
	gpio_set_direction(RX_PIN, GPIO_MODE_INPUT);
	gpio_intr_enable(RX_PIN);
	gpio_set_intr_type(RX_PIN, GPIO_INTR_ANYEDGE);
	gpio_isr_handler_add(RX_PIN, sonar_on_rx, NULL);

	xTaskCreate(measurement_task,"measurement_task",2048,NULL,10,NULL);
	xTaskCreate(dac_task,"dac_task",4096,NULL,10,NULL);
	
	while(1)
	{
		if(!dac_active)
		{
			//xQueueSend(dac_queue, &filenum, NULL);
			filenum = filenum==35 ? 1 : filenum+1;
		}

		printf("%.2f\n",distance);

		upd_distance(distance, dac_queue);
		fflush(stdout);	
		
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
