#include <stdio.h>

int positions[2] = {100,100};
int motor_buttons[2] = {0,0};

void mkstep(int m, int dir)
{
	positions[m] += dir;
	printf("step: %d, %d\n",m,dir);
}

int abs(int val)
{
	return val < 0 ? -val : val;
}

void delayMs(int delay){}
int gpio_get_level(int num){return 1;}

int manySteps(int* steps, int delay)
{
	printf("Manysteps: %d, %d\n",steps[0],steps[1]);
	int targets[2];
	for(int i=0; i < 2;i++)
		targets[i] = positions[i]+steps[i];

	float fstep[2] = {1,1};
	float fpos[2] = {positions[0],positions[1]};
	int asteps[2] = {abs(steps[0]),abs(steps[1])};

	if(steps[0] > steps[1])
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
		printf("done: %d, %d\n",done[0],done[1]);
		printf("fpos: %.3f, %.3f\n",fpos[0],fpos[1]);
		printf("pos: %d, %d\n-------------------\n",positions[0],positions[1]);
		delayMs(delay);
	}
	return 0;
}


int main()
{
	int steps[2];
	printf("Enter steps: ");
	scanf("%d%d",&steps[0],&steps[1]);
	manySteps(steps,0);
	return 0;
}