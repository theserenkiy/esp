#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"

#define IN1 26
#define IN2 25
#define IN3 33
#define IN4 32

#define BT_L 2
#define BT_R 16

uint8_t driverPins[2][2] = {{IN1,IN2},{IN3,IN4}};
uint8_t driverSignals[3][2] = {{0,1},{0,0},{1,0}};
int fullstep[4][4] = {{1,0,1,0},{1,0,0,1},{0,1,0,1	},{0,1,1,0}};
int halfstep[8][4] = {{1,0,0,0},{1,0,0,1},{0,0,0,1},{0,1,0,1},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0}};
int *steps = halfstep;
uint8_t curstep = 0;

int buttons[] = {BT_L, BT_R};

void setDriverMode(int num, int mode)
{
	uint8_t *pins = driverPins[num];
	uint8_t *signals = driverSignals[mode+1];

	gpio_set_level(pins[0],signals[0]);
	gpio_set_level(pins[1],signals[1]);
}

void step(int dir)
{
	curstep = (curstep+dir)%4;
	uint8_t *stepdata = steps[curstep];
	
	setDriverMode(0,stepdata[0]);
	setDriverMode(1,stepdata[1]);
	vTaskDelay(100/portTICK_PERIOD_MS);
	//setDriverMode(0,0);
	//setDriverMode(1,0);
}

void motorSignal(int in1,int in2,int in3,int in4)
{
	//printf("%d %d %d %d\n",in1,in2,in3,in4);
	gpio_set_level(IN1,in1);
	gpio_set_level(IN2,in2);
	gpio_set_level(IN3,in3);
	gpio_set_level(IN4,in4);
}

void app_main()
{
	gpio_set_direction(IN1,GPIO_MODE_OUTPUT);
	gpio_set_direction(IN2,GPIO_MODE_OUTPUT);
	gpio_set_direction(IN3,GPIO_MODE_OUTPUT);
	gpio_set_direction(IN4,GPIO_MODE_OUTPUT);

	gpio_set_direction(BT_L,GPIO_MODE_INPUT);
	gpio_set_direction(BT_R,GPIO_MODE_INPUT);
	gpio_set_pull_mode(BT_L,GPIO_PULLUP_ONLY);
	gpio_set_pull_mode(BT_R,GPIO_PULLUP_ONLY);


	
	int bprev[] = {0,0};
	int cmd = -1;
	int bval;
	int step = 0;
	int delay = 100;
	int *sig;
	while(1)
	{
		cmd = 0;
		for(int i=0; i < 2; i++)
		{
			bval = !gpio_get_level(buttons[i]);
			if(bval && !bprev[i])
			{
				cmd = i+1;
				bprev[i] = bval;
				break;
			}
			bprev[i] = bval;
		}		

		if(cmd)
		{
			step = (step+8+(cmd > 1 ? 1 : -1)) % 8;
			printf("cmd: %d; step: %d\n",cmd,step);
			sig = halfstep[step];
			motorSignal(sig[0],sig[1],sig[2],sig[3]);
		}

		// if(cmd)
		// {
		// 	delay += (cmd > 1 ? -1 : 1);
		// 	if(delay < 5)delay = 5;
		// 	if(delay > 100)delay = 100;
			
		// }

		// step = (step+1)%4;
		// sig = steps[step];
		// motorSignal(sig[0],sig[1],sig[2],sig[3]);
		
		vTaskDelay(delay/portTICK_PERIOD_MS);		
	}

}

