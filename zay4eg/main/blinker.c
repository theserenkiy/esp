#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

#include "driver/ledc.h"

int qqq;
void blinker_init()
{
    

	ledc_channel_config_t ledc_channel_0 = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 22,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_0);

	ledc_channel_config_t ledc_channel_1 = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_1,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 23,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_1);
}

int led_duties[] = {0,0};

void setLedDuty(int led,int duty,int steps)
{
    int step = 1000/steps;
    duty = duty%(steps*2);
    if(duty > steps)duty = (steps*2)-duty;
    int chan = led ? LEDC_CHANNEL_1 : LEDC_CHANNEL_0;
    ledc_set_duty(LEDC_LOW_SPEED_MODE, chan, duty*step);
    // Update duty to apply the new value
    ledc_update_duty(LEDC_LOW_SPEED_MODE, chan);
}

void blink(int steps)
{
    for(int i=0;i < steps*2;i++)
    {
        setLedDuty(0,i,steps);
        setLedDuty(1,i+steps,steps);
        vTaskDelay(10/portTICK_PERIOD_MS);
    }
}




