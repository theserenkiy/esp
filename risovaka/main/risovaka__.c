#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include "esp_timer.h"
#include "math.h"

#define L 90
#define h 110
#define xmax 200
#define ymax 200


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
#define BUTTON_ANGLE 8.0
#define HANDLEN 110
#define AREA_SIDE 100
#define TOP_POS 80

int motor_buttons[2] = {23,22};
int motor_pins[2][4] = {{5,17,16,4},{26,25,33,32}};

int stepmap[8][4] = {{1,0,0,0},{1,0,0,1},{0,0,0,1},{0,1,0,1},{0,1,0,0},{0,1,1,0},{0,0,1,0},{1,0,1,0}};
int curstep[2] = {0,0};			
int positions[2] = {100,100};
float fpositions[2] = {100,100};

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

float absf(float val)
{
	return val < 0 ? -val : val;
}

//returns 0 on success
// 1 or 2 if motor button were pressed (1 = motor 0, 2 = motor 1)
int manyStepsArr(float* steps,int delay)
{
	printf("Manysteps: %.3f, %.3f\n",steps[0],steps[1]);
	
	vTaskDelay(10/portTICK_PERIOD_MS);

	float targets[2];
	for(int i=0; i < 2;i++)
		targets[i] = fpositions[i]+steps[i];

	float fstep[2] = {1,1};
	//float fpos[2] = {positions[0],positions[1]};
	float asteps[2] = {absf(steps[0]),absf(steps[1])};

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
	printf("target: %.3f,  %.3f\n",targets[0], targets[1]);

	int pos, pressed = 0;
	int done[2] = {0,0};
	while(!(done[0] && done[1]))
	{
		for(int m=0; m < 2; m++)
		{
			if(!done[m])
			{
				fpositions[m] += fstep[m];
				pos = (int)round(fpositions[m]);
				
				if(positions[m] != pos)
				{
					if(dirs[m] > 0 || gpio_get_level(motor_buttons[m]))
						mkstep(m,dirs[m]);

					if(dirs[m] < 0 && !gpio_get_level(motor_buttons[m]))
						return m+1;	
				}

				if(pos == (int)round(targets[m]))
				{
					fpositions[m] = targets[m];
					done[m] = 1;
				}
			}			
		}
		// printf("done: %d, %d\n",done[0],done[1]);
		// printf("fpos: %.3f, %.3f\n",fpos[0],fpos[1]);
		// printf("pos: %d, %d\n-------------------\n",positions[0],positions[1]);
		delayMs(delay);
	}
	return 0;
}

int manySteps(float step0, float step1,int delay)
{
	float steps[2] = {step0, step1};
	return manyStepsArr(steps,delay);
}

int moveToAngle(float ang0, float ang1, int delay)
{
	printf("Move to angle: %.3f, %.3f\n",ang0,ang1);
	float angs[2] = {ang0,ang1};
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
	float destpos[2] = {angs[0]/HSTEP, angs[1]/HSTEP};
	printf("destpos: %.3f, %.3f\n",destpos[0],destpos[1]);
	return manySteps(destpos[0]-fpositions[0], destpos[1]-fpositions[1],delay);
}

// void MoveToPoint(float x, float y, float h){

// 	float v = sqrtf(x*x+y*y);
// 	float at = atanf(y/x);
// 	float as = asinf(v/(2*h));
// 	float beta = ((at - as) / (3.1415f) - 0.5f) * 2000.0f;
// 	float gamma = ((at + as) / (3.1415f) - 0.5f) * 2000.0f;
// 	printf("v=%f;at=%f;as=%f;beta=%f;gamma=%f\n",v,at,as,-beta,gamma);
// 	moveToAngle(-beta,gamma,5);
// 	// currStateA -= beta;
// 	// currStateB += gamma;
// 	// printf("st after A=%d;B=%d\n", currStateA, currStateB);
// }



void penUp()
{

}

void penDown()
{
	
}

void calibrate()
{
	penUp();

	int delay1 = 20; //5
	int delay2 = 40;


	manySteps(70,70,delay1);
	manySteps(-30,-30,delay1);

	manySteps(-500,500,delay1);
	manySteps(10,-10,delay1);

	if(manySteps(-50,50,delay2))
	{
		printf("Motor 0 calibrated\n");
		fpositions[0] = -(BUTTON_ANGLE/HSTEP);
		positions[0] = (int)fpositions[0];
	}
	
	manySteps(500,-500,delay1);
	manySteps(-10,10,delay1);

	if(manySteps(50,-50,delay2))
	{
		printf("Motor 1 calibrated\n");
		fpositions[1] = -(BUTTON_ANGLE/HSTEP);
		positions[1] = (int)fpositions[1];
	}
}

void searchAngle(int x, int y, float *angles)
{
	x = (float) x;
	y = (float) y;
	float AB = x-(xmax*0.5);
	float AO = L+y;
	float v = sqrtf(pow(AO,2)+pow(AB,2));
	float alpha = asinf(AB/v);
	printf("%.3f\n", alpha*57.3);
	
	printf("len = %f\n", v);

	float EH = sqrt(pow(h,2)-pow(v/2,2));
	float beta = acosf((v/2)/h);
	printf("%.3f\n", beta*57.3);
	float fi_1 = alpha + beta;			//НЕАКТУАЛЬНАчет хрень какаято оно неправильно считает фи, но правильно считает альфу, бету - х3.
	float fi_2 = beta - alpha;		
	
	angles[0] = 90 - (fi_1 * 57.3);
    angles[1] = 90 - (fi_2 * 57.3);

}

void app_main()
{
	delayMs(2000);
	gpio_init();
	calibrate();

	moveToAngle(45,45,20);

	return;


    //float fi_1, fi_2;
    //float angles[2];

	

	while(1){
		
		float fi_1, fi_2;
    	float angles[2];
		int crdx = 50;
		
		for (int i = 0; i < 10; i++){

			searchAngle(crdx, 20, angles);
			crdx += 15;
			printf("fi_1 = %f, fi_2 = %f\n", angles[0], angles[1]);
			moveToAngle(angles[0], angles[1], 50);

			vTaskDelay(100/portTICK_PERIOD_MS);

		}
		delayMs(1000);
		calibrate();
	}

    //searchAngle(150,20,angles);
    //printf("fi_1 = %f, fi_2 = %f\n", angles[0]*57.3, angles[1]*57.3);
   

	// MoveToPoint(10.0f, 10.0f, 10.0f);

    // while(1)
    // {
    //     moveToAngle(20,20,15);
    //     delayMs(1000);
    //     moveToAngle(45,45,15);
    //     delayMs(1000);
    // }

	// while(1)
	// {
	// 	moveToAngle(0,90,5);
	// 	delayMs(1000);
	// 	moveToAngle(120,0,5);
	// 	delayMs(1000);
	// }
	

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