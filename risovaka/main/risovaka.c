#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"

//0 - upper motor, right hand (view from sheet)
//1 - lower motor, left hand
//connector position looking from edge of board:
// black - green - yellow - red
// [  motor B   ] [  motor A   ]

//Directions (for both motors):
// -1 - to the button
// +1 - to center

//Motor full step = 0.9

/*

          o  <- motor axis 
          |
          |  TOP_POS
     ----------
     |        |
     |  AREA  |
     |        |
     ----------

*/


#define FSTEP 0.9
#define HSTEP (FSTEP/2)
#define BUTTON_ANGLE 8
#define HANDLEN 110
#define AREA_SIDE 100
#define TOP_POS 80

int motor_buttons[2] = {23,22};
int motor_pins[2][4] = {{5,17,16,4},{26,25,33,32}};

int stepmap[8][4] = {{1,0,0,0},{1,0,0,1},{0,0,0,1},{0,1,0,1},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0}};
int curstep[2] = {0,0};
int positions[2] = {100,100};

void delayMs(int delay)
{
	//vTaskDelay(delay/portTICK_PERIOD_MS);
	usleep(delay*1000);
}

void gpio_init()
{
	for(int i=0;i < 8; i++)
	{
		gpio_set_direction(((int*)motor_pins)[i],GPIO_MODE_OUTPUT);
	}

	for(int i=0;i < 2;i++)
	{
		gpio_set_direction(motor_buttons[i],GPIO_MODE_INPUT);
		gpio_set_pull_mode(motor_buttons[i],GPIO_PULLUP_ONLY);
	}
}

void mkstep(int motor, int dir)
{
	curstep[motor] = (curstep[motor]+8+dir)%8;
	int* step = stepmap[curstep[motor]];
	positions[motor] += dir;
	for(int i=0;i < 4;i++)
	{
		gpio_set_level(motor_pins[motor][i],step[i]);
	}
}

void motorOff(int motor)
{
	for(int i=0;i < 4;i++)
	{
		gpio_set_level(motor_pins[motor][i],0);
	}
}

int abs(int val)
{
	return val < 0 ? -val : val;
}

//returns 0 on success
// 1 or 2 if motor button were pressed (1 = motor 0, 2 = motor 1)
int manyStepsArr(int* steps,int delay)
{
	printf("Manysteps: %d, %d\n",steps[0],steps[1]);
	
	vTaskDelay(10/portTICK_PERIOD_MS);

	int targets[2];
	for(int i=0; i < 2;i++)
		targets[i] = positions[i]+steps[i];

	float fstep[2] = {1,1};
	float fpos[2] = {positions[0],positions[1]};
	int asteps[2] = {abs(steps[0]),abs(steps[1])};

	if(asteps[0] > asteps[1])
	{
		fstep[0] = steps[0]/asteps[0];
		fstep[1] = ((float)steps[1])/asteps[0];
	}
	else
	{
		fstep[0] = ((float)steps[0])/asteps[1];
		fstep[1] = steps[1]/asteps[1];
	}

	int dirs[2] = {(fstep[0] > 0 ? 1 : -1), (fstep[1] > 0 ? 1 : -1)};
		
	printf("fstep: %.3f,  %.3f\n",fstep[0], fstep[1]);
	printf("target: %d,  %d\n",targets[0], targets[1]);

	int pos, pressed = 0;
	int done[2] = {0,0};
	while(!done[0] || !done[1])
	{
		for(int m=0; m < 2; m++)
		{
			if(positions[m]==targets[m])
			{
				done[m] = 1;
				continue;
			}
			fpos[m] += fstep[m];
			pos = (int)fpos[m];
			
			if(positions[m] != pos)
			{
				if(dirs[m] > 0 || gpio_get_level(motor_buttons[m]))
					mkstep(m,dirs[m]);

				if(dirs[m] < 0 && !gpio_get_level(motor_buttons[m]))
					return m+1;	
			}
			
		}
		// printf("done: %d, %d\n",done[0],done[1]);
		// printf("fpos: %.3f, %.3f\n",fpos[0],fpos[1]);
		// printf("pos: %d, %d\n-------------------\n",positions[0],positions[1]);
		delayMs(delay);
	}
	return 0;
}

int manySteps(int step0, int step1,int delay)
{
	int steps[2] = {step0, step1};
	return manyStepsArr(steps,delay);
}

int moveToAngle(int ang0, int ang1, int delay)
{
	int angs[2] = {ang0,ang1};
	// for(int i=0;i < 2;i++)
	// {
	// 	if(angs[i] < 0)angs[i] = 0;
	// 	if(angs[i] > 135)angs[i] = 135;
	// }
	// int xang = 180-angs[1]-angs[0];
	// if(xang < 5)
	// {
	// 	angs[1] = 180-angs[0]-5;
	// }
	int destpos[2] = {angs[0]/HSTEP, angs[1]/HSTEP};
	printf("destpos: %d, %d\n",destpos[0],destpos[1]);
	return manySteps(destpos[0]-positions[0], destpos[1]-positions[1],delay);
}

void penUp()
{

}

void penDown()
{
	
}

void calibrate()
{
	penUp();

	manySteps(70,70,5);
	manySteps(-30,-30,5);

	manySteps(-500,500,5);
	manySteps(10,-10,5);

	if(manySteps(-50,50,40))
	{
		printf("Motor 0 calibrated\n");
		positions[0] = -(BUTTON_ANGLE/HSTEP);
	}
	
	manySteps(500,-500,5);
	manySteps(-10,10,5);

	if(manySteps(50,-50,40))
	{
		printf("Motor 1 calibrated\n");
		positions[1] = -(BUTTON_ANGLE/HSTEP);
	}
}

void app_main()
{
	delayMs(2000);
	gpio_init();
	calibrate();

	while(1)
	{
		moveToAngle(0,90,5);
		delayMs(1000);
		moveToAngle(120,0,5);
		delayMs(1000);
	}
	

	// while(1)
	// {
	// 	for(int m=0;m < 2;m++)
	// 	{
	// 		for(int ndir=0;ndir < 2;ndir++)
	// 		{
	// 			int dir = ndir ? 1 : -1;
	// 			printf("Motor: %d; dir: %d\n",m,dir);
	// 			manySteps(m,50,dir,50);
	// 			delayMs(500);
	// 		}
	// 	}
	// }

}