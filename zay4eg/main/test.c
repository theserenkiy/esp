#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>


static int lvl0_msgs[] = {1,  3,           15, 19, 23, 24, 25,   29};
static int lvl1_msgs[] = {  2,   4, 8, 17,             26, 27,  30, 32, 33};
static int lvl2_msgs[] = {         5, 9, 10, 13, 14, 16, 28, 29, 31, 33};
static int lvl3_msgs[] = {		 5, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 28, 29, 31, 32, 33};

static int *lvlmsgs[] = {lvl0_msgs, lvl1_msgs, lvl2_msgs, lvl3_msgs};

int main()
{
	srand(time(NULL));
	int used_msgs[64];
	memset(used_msgs, 0, sizeof(used_msgs));
	int msg;
	//distance_reaction_run();
	for(int i = 0; i < 4; i++) {
		while(1)
		{
			msg = lvlmsgs[i][rand()%sizeof(lvlmsgs[i])];
			for(int j = 0; j < 64; j++)
			{
				if(used_msgs[j]==msg)
					continue;
			}
			break;
		}
		printf("lvl: %d; msg: %d\n", i, msg);	
	}
	
}