#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "dac.c"

#define DEVICE_TEST
#include "distance_reaction.c"

#define TX_PIN 26
#define RX_PIN 27

#define DIST_FILTER_SIZE 8




static QueueHandle_t gpio_evt_queue = NULL;
volatile uint64_t sonar_tx_time;
volatile float distance;

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



void app_main(void)
{
	printf("Hello world!\n");
	dac_test();
	return;

	gpio_install_isr_service(0);

	gpio_reset_pin(TX_PIN);
	gpio_set_direction(TX_PIN, GPIO_MODE_OUTPUT);

	gpio_reset_pin(RX_PIN);
	gpio_set_direction(RX_PIN, GPIO_MODE_INPUT);
	gpio_intr_enable(RX_PIN);
	gpio_set_intr_type(RX_PIN, GPIO_INTR_ANYEDGE);
	gpio_isr_handler_add(RX_PIN, sonar_on_rx, NULL);

	xTaskCreate(measurement_task,"measurement_task",2048,NULL,10,NULL);
	
	
	while(1)
	{
		printf("%.2f\n",distance);

		upd_distance(distance);

		fflush(stdout);	
		
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
