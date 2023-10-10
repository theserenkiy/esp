#include <stdio.h>
#include <windows.h>

#define LED_COUNT 8

int drawLeds(int *state)
{
	char str[3*LED_COUNT];
	printf("\r");
	for(int i=0; i < LED_COUNT; i++)
	{
		printf("[%c]",state[i] ? '+' : ' ');
	}
	fflush(stdout);
}

int getLedState(int *state)
{
	FILE *fp = fopen("ledstate.txt","r");
	int scanned;
	if(fp==NULL)
	{
		printf("Не могу открыть ledstate.txt. Сначала запустите сервер\n");
		return -1;	
	}
	for(int i=0; i < LED_COUNT; i++)
	{
		scanned = fscanf(fp,"%d",&state[i]);
		if(scanned <= 0)
			state[i] = 0;
	}
	fclose(fp);
}

int main()
{
	int state[LED_COUNT];
	while(1)
	{
		if(getLedState(state) < 0)
			break;
		drawLeds(state);
		Sleep(1000);
		//break;
	}
	return 0;
}