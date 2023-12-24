#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


static int lvl0_msgs[] = {1,  3,           15, 19, 23, 24, 25,   29};
static int lvl1_msgs[] = {  2,   4, 8, 17,             26, 27,  30, 32, 33};
static int lvl2_msgs[] = {         5, 9, 10, 13, 14, 16, 28, 29, 31, 33};
static int lvl3_msgs[] = {		 5, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 28, 29, 31, 32, 33};

static int *lvlmsgs[] = {lvl0_msgs, lvl1_msgs, lvl2_msgs, lvl3_msgs};

void test1()
{
	srand(time(NULL));
	int used_msgs[64];
	int used_msgs_ptr = 0;
	memset(used_msgs, 0, sizeof(used_msgs));
	int msg;
	//distance_reaction_run();
	for(int lvl = 0; lvl < 4; lvl++) {
		msg = 0;
		while(1)
		{
			for(int k=0; k<16; k++)
			{
				msg = lvlmsgs[lvl][rand()%sizeof(lvlmsgs[lvl])];
				for(int j = 0; j < used_msgs_ptr; j++)
				{
					if(used_msgs[j]==msg)
					{
						msg = 0;
						break;
					}
				}
				if(msg)
					break;
			}
			if(msg)break;
			used_msgs_ptr = 0;
		}
		used_msgs[used_msgs_ptr++]=msg;
		if(used_msgs_ptr >= 32)
			used_msgs_ptr = 0;
		
		//last_said = nupdates;
		printf("lvl: %d; msg: %d\n", lvl, msg);	
	}
}

int fifo[16];
void fifo_add(int *fifo, int fifo_size, int val)
{
	for(int i = fifo_size-1; i > 0; i--)
	{
		fifo[i] = fifo[i-1];
	}
	fifo[0] = val;
}

int main()
{
	// printf("fifo size %d",sizeof(fifo));
	// //return 0;
	// for(int i=0; i<20; i++)
	// {
	// 	fifo_add(fifo,16,i);
	// 	for(int j=0; j<16; j++)
	// 		printf("%d, ",fifo[j]);
	// 	printf("\n");
	// }


}