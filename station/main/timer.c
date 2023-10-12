#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "driver/gptimer.h"
#include "esp_log.h"

typedef struct {
    uint64_t event_count;
} qelem_t;


static bool IRAM_ATTR onTimerAlarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *userdata)
{
    BaseType_t high_task_awoken = pdFALSE;
    QueueHandle_t queue = (QueueHandle_t)userdata;
    // Retrieve the count value from event data
    qelem_t qelem = {
        .event_count = edata->count_value
    };
    // Optional: send the event data to other task by OS queue
    xQueueSendFromISR(queue, &qelem, &high_task_awoken);
    //reconfigure alarm value
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = edata->alarm_value + 1000000, // alarm in next 1s
    };
    gptimer_set_alarm_action(timer, &alarm_config);

	//printf("Timer alarm @ %llu\n", edata->alarm_value);
    // return whether we need to yield at the end of ISR
    return high_task_awoken == pdTRUE;
}



void timer_test()
{
    qelem_t qelem;
    QueueHandle_t queue = xQueueCreate(10, sizeof(qelem_t));
    if (!queue) {
        printf("Creating queue failed\n");
        return;
    }

    gptimer_handle_t gptimer = NULL;
	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
	};
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

	gptimer_alarm_config_t alarm_config = {
		.alarm_count = 1000000, // initial alarm target = 1s @resolution 1MHz
	};
	ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));

	gptimer_event_callbacks_t cbs = {
		.on_alarm = onTimerAlarm, // register user callback
	};
	ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, queue));

	gptimer_enable(gptimer);
	gptimer_start(gptimer);

	while (1) {
        if (xQueueReceive(queue, &qelem, pdMS_TO_TICKS(500))) {
            printf("Timer alarmed, count=%llu\n", qelem.event_count);

        } else {
            printf("Missed one count event\n");
        }
    }
}