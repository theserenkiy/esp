#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "driver/ledc.h"

#define SERVO_PWM_BITS	14


//430 = -90
//1860 = +90
//1120 = 0

//1060 = 90
//400 = -90
//1820 = +90

int angles[] = {
	400,	//0
	460,	//10
	530,	//20
	600,	//30
	670,	//40
	745,	//50
	820,	//60
	900,	//70
	970,	//80
	1070,	//90
	1150,	//100
	1235,	//110
	1320,	//120
	1400,	//130
	1490,	//140
	1570,	//150
	1660,	//160
	1740,	//170
	1830,	//180

};


volatile int val = 0;//(1.5/20)*(1 << SERVO_PWM_BITS);


static void IRAM_ATTR encoder_irq(void *arg)
{
	gpio_intr_disable(21);
	usleep(500);
	if(!gpio_get_level(21))
	{
		if(gpio_get_level(19))
			val--;
		else
			val++;
	}

	gpio_intr_enable(21);
}

void init_servo()
{
	ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE, 
        .timer_num        = 0,
        .duty_resolution  = SERVO_PWM_BITS,
        .freq_hz          = 50,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel_0 = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = 0,
        .timer_sel      = 0,
        .intr_type      = 0,
        .gpio_num       = 13,
        .duty           = val, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_0);
}

//-90 ... 90
int servo_get_angle_duty(int ang)
{
	if(ang < -90)ang = -90;
	if(ang > 90)ang = 90;
	ang += 90;
	int grp = ang/10;
	if(grp==18)
		return angles[18];

	float deg_cost = (angles[grp+1]-angles[grp])/10;
	printf("ang: %d; grp: %d; cost: %.3f\n",ang, grp, deg_cost);

	return angles[grp]+(int)(deg_cost*(ang%10));
}

void servo_set_angle(int ang)
{
	int duty = servo_get_angle_duty(ang);
	ledc_set_duty(LEDC_LOW_SPEED_MODE, 0, duty);
	ledc_update_duty(LEDC_LOW_SPEED_MODE, 0);
}

void app_main()
{
	gpio_set_direction(21,GPIO_MODE_INPUT);
	gpio_set_pull_mode(21,GPIO_PULLUP_ONLY);

	gpio_set_direction(17,GPIO_MODE_OUTPUT);

	gpio_set_direction(19,GPIO_MODE_INPUT);
	gpio_set_pull_mode(19,GPIO_PULLUP_ONLY);

	gpio_install_isr_service(0);
	gpio_intr_enable(21);
	gpio_set_intr_type(21, GPIO_INTR_NEGEDGE);
	gpio_isr_handler_add(21, encoder_irq, NULL);

	init_servo();

	int lastval = val;
	while(1)
	{
		//if(esp_timer_get_time()-encoder_last_event > 10000)
		if(lastval != val)
		{
			if(val < -90)val = -90;
			if(val > 90)val = 90;
			
			//if(val > (1 << SERVO_PWM_BITS)-1)val = (1 << SERVO_PWM_BITS)-1;

			printf("%d\n",val);
			servo_set_angle(val);
			
			lastval = val;
		}
			
		vTaskDelay(50/portTICK_PERIOD_MS);
	}
}