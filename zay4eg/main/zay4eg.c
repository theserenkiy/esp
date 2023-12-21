/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define TX_PIN 26
#define RX_PIN 27

uint64_t time_us()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return tv.tv_sec*1000000+tv.tv_usec;
}

uint64_t sonar_tx_time;
volatile float last_dist = 0;
void IRAM_ATTR sonar_on_rx(float *dist)
{
	uint32_t time = (uint32_t)(time_us()-sonar_tx_time);
	*dist = ((float)time)/58824;
	//printf("Q\n");
	//printf("Sonar time: %lu, distance: %.2f m\n", time, dist);
	//fflush(stdout);
}

void app_main(void)
{
	printf("Hello world!\n");

	gpio_install_isr_service(ESP_INTR_FLAG_LOWMED);

	gpio_reset_pin(TX_PIN);
	gpio_set_direction(TX_PIN, GPIO_MODE_OUTPUT);

	gpio_reset_pin(RX_PIN);
	gpio_set_direction(RX_PIN, GPIO_MODE_INPUT);
	gpio_intr_enable(RX_PIN);
	gpio_set_intr_type(RX_PIN, GPIO_INTR_NEGEDGE);
	gpio_isr_handler_add(RX_PIN, sonar_on_rx, &last_dist);

	while(1)
	{
		gpio_set_level(TX_PIN,1);
		usleep(9);
		gpio_set_level(TX_PIN,0);

		// printf("The time is %llu\n",time_us());
		
		printf("Last dist: %.2f\n",last_dist);
		fflush(stdout);


		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
