#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/queue.h"

#ifdef TESTING

#define HYSTERESIS_ADD	0.3
#define CANSAY_DELAY	1
#define CANSAY_RAND		1
float approach_steps[] = {3, 2, 1, 0.5};

#elif defined DEVICE_TEST

float approach_steps[] = {0.6, 0.45, 0.3};
#define HYSTERESIS_ADD	0.15
#define CANSAY_DELAY	18
#define CANSAY_RAND		3

#else 

#define HYSTERESIS_ADD	0.3
#define CANSAY_DELAY	6
#define CANSAY_RAND		3
float approach_steps[] = {3, 2, 1, 0.5};

#endif

typedef struct {
	int started;
	int messages;
	int introduced;
	int approached;
	int departured;
	int destroy_countdown;
	int used_msgs[64];
	int used_msgs_ptr;
} session_t;

static int nupdates;
static int direction = 0;
static session_t session;
static int cur_step = -1, last_step = -1;
static int idle_counter = 0;
static int last_said = 0;

static int lvl0_msgs[] = {1,  3,           15, 19, 25,   29, 36};
static int lvl1_msgs[] = {  2,   4, 7, 8, 17, 23, 24,            26, 27,  30, 32, 33, 34};
static int lvl2_msgs[] = {         5, 6, 7, 9, 10, 13, 14, 16, 17, 18, 28, 29, 31, 33, 34, 35};
static int lvl3_msgs[] = {		 5, 6, 7, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 28, 29, 31, 32, 33, 34, 35};

static int *lvlmsgs[] = {lvl0_msgs, lvl1_msgs, lvl2_msgs, lvl3_msgs};
static int msg;

void distance_reaction_init()
{
	srand(time(NULL));
}

int canSay()
{
	return !last_said || (nupdates-last_said > CANSAY_DELAY && rand()%CANSAY_RAND == 0);
}

void say(int lvl)
{
	printf("Say lvl %d\n",lvl);
	msg=0;
	while(1)
	{
		for(int k=0; k<16; k++)
		{
			msg = lvlmsgs[lvl][rand()%sizeof(lvlmsgs[lvl])];
			for(int j = 0; j < session.used_msgs_ptr; j++)
			{
				if(session.used_msgs[j]==msg)
				{
					msg = 0;
					break;
				}
			}
			if(msg)
				break;
		}
		if(msg)break;
		session.used_msgs_ptr = 0;
	}
	session.used_msgs[session.used_msgs_ptr++]=msg;
	if(session.used_msgs_ptr >= 32)
		session.used_msgs_ptr = 0;
	
	last_said = nupdates;
	session.messages++;
	xQueueSend(dac_queue, &msg, NULL);
}

void new_session()
{
	printf("new session\n");
	if(!canSay())return;
	say(0);
	session.introduced = 1;
}

void close_session()
{
	if(canSay())
		say("Bye!");
}

void sayNext()
{
	say(session.messages < 4 ? session.messages : 3);
}


void upd_distance(float dist, QueueHandle_t dac_queue)
{
	nupdates++;
	cur_step = -1;
	float threshold;
	for(int i=0; i < 3; i++)
	{
		threshold = approach_steps[i];
		if(last_step >= i)
			threshold += HYSTERESIS_ADD;
		if(dist <= threshold)
		{
			cur_step = i;
			//printf("Thr: %.3f\n",threshold);
		}
		else
		{
			break;
		}
	}

	printf("Cur step: %d\n", cur_step);
	
	//if idle

	if(cur_step >= 0)
	{
		session.destroy_countdown = 10;
		if(last_step == cur_step)
		{
			idle_counter++;

			if(canSay())
			{
				if(cur_step > 0)
					sayNext();
			}
			
			return;
		}
	}

	if(!session.started && cur_step >= 0)
	{
		session = (session_t) {
			.started = nupdates,
			.messages = 0,
			.introduced = 0,
			.approached = 0,
			.departured = 0,
			.destroy_countdown = 10, //5 sec
			.used_msgs_ptr = 0
		};
		new_session();
	}
	else if(session.started && cur_step < 0)
	{
		if(session.destroy_countdown-- <= 0)
		{
			session.started = 0;
			last_said = 0;
			printf("closing session\n");
			// if(last_step > 0)
			// 	depart(0);
			// else if(session.introduced)
			// 	close_session();
		}
		
	}
	else if(cur_step > last_step)
	{
		sayNext();
	}
	// else
	// {
	// 	depart();
	// 	session.approached = 1;
	// }

	last_step = cur_step;
}

#ifdef TESTING
void distance_reaction_run()
{
	session.started=0;
	float input_distance;
	srand(time(NULL));

	while(1)
	{
		printf("Input distance: ");
		scanf("%f",&input_distance);
		upd_distance(input_distance);
	}
}
#endif