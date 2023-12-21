#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#define TX_PIN 26
#define RX_PIN 27

#define DIST_FILTER_SIZE 16

#define APPROACH_STEPS = {0.8, 0.6, 0.3, 0.2}


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
	time = (uint32_t)(time_us()-sonar_tx_time);
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
	while(1)
	{
		gpio_set_level(TX_PIN,1);
		usleep(9);
		gpio_set_level(TX_PIN,0);

		sonar_tx_time = time_us();

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
		}
	}
}

void new_session()
{
	*+printf("Privet!\n");
}

void close_session()
{
	printf("Bye!\n");
}

void approach(int lvl)
{
	if(lvl==1)
		printf("Do not approach to me!\n");
	else if(lvl==2)
		printf("Did you hear me? STOP!\n");
	else if(lvl==3)
		printf("You're fearless son of bitch! Go away!\n");
}

void depart(int lvl)
{
	if(lvl==1)
		printf("You should be further away!\n");
	else if(lvl==2)
		printf("Do not stop going away!\n");
}

void app_main(void)
{
	printf("Hello world!\n");

	gpio_install_isr_service(0);

	gpio_reset_pin(TX_PIN);
	gpio_set_direction(TX_PIN, GPIO_MODE_OUTPUT);

	gpio_reset_pin(RX_PIN);
	gpio_set_direction(RX_PIN, GPIO_MODE_INPUT);
	gpio_intr_enable(RX_PIN);
	gpio_set_intr_type(RX_PIN, GPIO_INTR_NEGEDGE);
	gpio_isr_handler_add(RX_PIN, sonar_on_rx, NULL);

	xTaskCreate(measurement_task,"measurement_task",2048,NULL,10,NULL);
	
	int direction = 0;
	int session = 0;
	int cur_step = -1, last_step = -1;
	int idle_counter = 0;
	while(1)
	{
		printf("Distance: %.2f\n",distance);
		fflush(stdout);	

		cur_step = -1;
		for(int i=0; i < 4; i++)
		{
			if(distance <= APPROACH_STEPS[i])
				cur_step = i;
			else break;
		}

		if(last_step == cur_step)
			idle_counter++;

		if(!session && cur_step >= 0)
		{
			session = 1;
			new_session();
		}

		if(session && cur_step < 0)
		{
			session = 0;
			close_session();
		}

		if

		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
