#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "code.c"
#include "gamma.c"
typedef unsigned char uint8;


#define G1 5
#define G2 16
#define B 19
#define D 22
#define LAT 13

#define R1 15
#define B1 2
#define R2 17
#define B2 18
#define A 21
#define C 23
#define CLK 12
#define OE 14

int pins[] = {G1,G2,R1,R2,B1,B2,A,B,C,D,LAT,CLK,OE};

void lat(int linenum,int x)
{
	gpio_set_level(OE, 1);
	//usleep(100);
	if(1)
	{
		gpio_set_level(LAT, 1);
		//usleep(10);
		gpio_set_level(LAT, 0);
		
	}
	gpio_set_level(A, linenum & 0b0001);	
	gpio_set_level(B, linenum & 0b0010);
	gpio_set_level(C, linenum & 0b0100);
	gpio_set_level(D, linenum & 0b1000);
	usleep(4096-x);
	gpio_set_level(OE, 0);
	usleep(x);
}

void clk()
{
	gpio_set_level(CLK, 0);
	gpio_set_level(CLK, 1);
}

void point(int colorr,int colorg,int colorb, int mask)
{
	

		gpio_set_level(R1, gamma[colorr] & mask);
		gpio_set_level(G1, gamma[colorg] & mask);
		gpio_set_level(B1, gamma[colorb] & mask);
		// clk();

	//	usleep(10);
}
void point1(int colorr,int colorg,int colorb)
{
	

		gpio_set_level(R2, colorr & 0b10000000);
		gpio_set_level(G2, colorg & 0b10000000);
		gpio_set_level(B2, colorb & 0b10000000);
		// clk();

	//	usleep(10);
}

void app_main()
{
	for(int i=0; i < 13; i++)
	{
		gpio_reset_pin(pins[i]);
		gpio_set_direction(pins[i], GPIO_MODE_OUTPUT);
	}

	gpio_set_level(LAT, 0);	
	gpio_set_level(OE, 1);
	gpio_set_level(G1, 0);
	gpio_set_level(G2, 0);
	gpio_set_level(B1, 0);
	gpio_set_level(B2, 0);
	gpio_set_level(R1, 0);
	gpio_set_level(R2, 0);
	gpio_set_level(A, 0);
	gpio_set_level(B, 0);
	gpio_set_level(C, 0);
	gpio_set_level(D, 0);
	gpio_set_level(CLK, 0);
	

	while(1)
	{	
		int d =0;
		
		for(int f=0;f<16;f++)
		{
			for(int bit=11; bit >=0; bit--)
			{
				for(int i=0; i<(64*3);i = i+3)
				{	
					point(image[f*64*3+i],image[f*64*3+i+1],image[f*64*3+i+2],1 << bit);
					//point1(image[f*64*3+i+(16*64*3)],image[f*64*3+i+(16*64*3)+1],image[f*64*3+i+(16*64*3)+2]);
					clk();
				}
				lat(f,1<<(bit));
			}
			
		}
	}


   
}