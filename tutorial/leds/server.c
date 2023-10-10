#include <stdio.h>

#define LED_COUNT 8

int leds[LED_COUNT];

int writeLeds(int *leds, int num)
{
	FILE *fp = fopen("ledstate.txt", "w");
	for(int i=0; i<num; i++)
	{
		fprintf(fp, "%d ", leds[i]);
	}
	fclose(fp);
}

int getLed(int num)
{
	printf("LED %d is %s\n", num, leds[num] ? "on" : "off");
	return 0;
}

int setLed(int num,int state)
{
	leds[num] = state;
	printf("LED %d set to %d\n", num, state);
	writeLeds(leds,LED_COUNT);
	return 0;
}



int readLeds(int *leds, int count)
{
	FILE *fp; 
	fp = fopen("ledstate.txt", "r");
	if(fp==NULL)
		fp = fopen("ledstate.txt", "w+");
	
	for (int i = 0; i < count; i++)
	{
		fscanf(fp, "%d", &leds[i]);
	}
	fclose(fp);
	return 1;
}

int main()
{
	int nled, state, nscanned;
	char cmd;
	char str[20];

	readLeds(leds, LED_COUNT);

	while(1)
	{
		printf("Enter command: ");
		gets(str);
		nscanned = sscanf(str,"%c %d %d", &cmd, &nled, &state);	
		
		if(nscanned < 2 || nscanned > 3)
		{
			printf("Incorrect number of arguments\n");
			continue;
		}

	    if(nled < 0 || nled >= LED_COUNT)
		{
			printf("Invalid LED number %d (should be 0 ... %d)\n",nled,LED_COUNT-1);
            continue;
		}

		if(cmd == 'g')
			getLed(nled);
		else if(cmd =='s')
            setLed(nled, state);
		else if(cmd == 'q')
			break;
		else
		{
			printf("Incorrect command %c\n", cmd);
			continue;
		}

	}
}