#include <stdio.h>


int display_init[][2] = {
	{0x0C,0x01},	//Shutdown: off
	{0x0A,0x01},	//Intensity: 1/32
	{0x0B,0x05},	//Scan limit: all
};


int main()
{
    printf("%d\n",sizeof(display_init));
}