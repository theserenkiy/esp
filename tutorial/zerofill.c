#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

char *zerofill(int val, int len)
{
	int val_digits = val ? log10(val)+1 : 1;
	int reslen = val_digits >= len ? val_digits : len;
	
	char *res = malloc(reslen+1);
	int start = reslen-val_digits;
	sprintf(res+start, "%d", val);
	if(len > val_digits)
		memset(res, '0', reslen-val_digits);
	return res;
}

int main()
{
	// time_t utime = time(NULL);
	// struct tm *tm = gmtime(&utime);
	// printf("Current time: %d:%d:%d\n",tm->tm_hour,tm->tm_min,tm->tm_sec);
	//int i = 10;

	for(int i = 0; i < 110; i+=4)
	{
		printf("%d  %s\n", i, zerofill(i,3));	
	}
}

