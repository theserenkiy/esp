#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

int zerofill(int val, int len, char *res)
{
	int val_digits = val ? log10(val)+1 : 1;
	int reslen = val_digits >= len ? val_digits : len;
	if(reslen > 127)
		return -1;
	
	int start = reslen-val_digits;
	sprintf(res+start, "%d", val);
	if(len > val_digits)
		memset(res, '0', reslen-val_digits);
	return 0;
}

int main()
{
	int a = 1234;
	int *p = &a;

	printf("a = %d\n", p[1]);
}


