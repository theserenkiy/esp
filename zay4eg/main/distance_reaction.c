#include <stdio.h>
#include <string.h>
#include "freertos/queue.h"
#include "esp_timer.h"

#include "sounds_data.c"

#ifdef TESTING

#define HYSTERESIS_ADD	0.3
#define CANSAY_DELAY	1
#define CANSAY_RAND		1
float approach_lvls[] = {3, 2, 1, 0.5};

#elif defined DEVICE_TEST

float approach_lvls[] = {0.8, 0.6, 0.45, 0.3};
#define HYSTERESIS_ADD	0.1
#define CANSAY_DELAY	4
#define CANSAY_RAND		2

#else 

#define HYSTERESIS_ADD	0.2
#define CANSAY_DELAY	5
#define CANSAY_RAND		2
float approach_lvls[] = {4, 3, 2, 1};

#endif

typedef struct {
	int started;
	int lvl_messages[3];
	int destroy_countdown;
	int attack;
} session_t;

#define RECENTS_LEN 24

int recent_msgs[RECENTS_LEN];
void fifo_add(int *fifo, int fifo_size, int val)
{
	for(int i = fifo_size-1; i > 0; i--)
	{
		fifo[i] = fifo[i-1];
	}
	fifo[0] = val;
}

static int nupdates;
static session_t session;
static int cur_lvl = -1, last_lvl = -1, last_act_lvl = -1;
static int last_said = 0;


static int msg;

void distance_reaction_init()
{
	//srand(time(NULL));
	memset((void *)recent_msgs,0,RECENTS_LEN*sizeof(int));
}

int is_session_started()
{
	return session.started;
}

int canSay(int immediately)
{
	int condition = immediately 
		? nupdates > last_said
		: ((nupdates-last_said) > CANSAY_DELAY);// && (rand()%CANSAY_RAND) == 0);
	printf("cansay(%d) condition: %d\n", immediately, condition);
	return session.started && (!last_said || condition);
}

int getRandMsg(int lvl)
{
	int msg = 0;
	int fifolen = RECENTS_LEN;
	int last_msg_found = 0;
	
	int sublevel = session.lvl_messages[lvl-1] ? 1 : 0;
	printf("Sublvl %d\n",sublevel);
	for(int i=0; i < 3; i++)
	{
		for(int j=0; j < lvlsizes[lvl-1][sublevel]*2; j++)
		{
			msg = lvlmsgs[lvl-1][sublevel][esp_timer_get_time() % lvlsizes[lvl-1][sublevel]];
			last_msg_found = msg;
			for(int k=0; k < fifolen; k++)
			{
				if(recent_msgs[k]==msg)
				{
					msg = 0;
					break;
				}
			}
			if(msg)
				break;
			printf("mgs not found l2\n");
		}
		if(msg)
			break;
		printf("mgs not found l1\n");
		fifolen = (int)(fifolen/2);
	}

	if(!msg)
	{
		printf("NO MESSAGE FOUND!!!");
		msg = last_msg_found;
	}
	printf("message found: %d\n", msg);
	printf("message duration: %d\n",msg_durations[msg-1]);
	return msg;
}

void say(int lvl)
{
	printf("Say(%d)\n",lvl);

	for(int i=0; i < lvl; i++)
	{
		if(!session.lvl_messages[i])
		{
			lvl = i+1;
			break;
		}
	}

	printf("Saying lvl %d\n",lvl);

	msg=0;
	// if(lvl>=2 && (session.lvl_messages[1] + session.lvl_messages[2]) > 2 && !session.attack)
	// {
	// 	msg = 21; // to attack
	// 	session.attack = 1;
	// }
	// else if(session.attack == 2)
	// {
	// 	msg = 40; // to attack
	// 	session.attack = 3;
	// }
	// else
		msg = getRandMsg(lvl);
	
	last_said = nupdates+(msg_durations[msg-1]*2);
	session.lvl_messages[lvl-1]++;
	fifo_add(recent_msgs, RECENTS_LEN, msg);
	xQueueSend(dac_queue, &msg, NULL);
}

int attack_steps[] = {10,20,40,70,100,150,200,300,500,700,1000};
void moveServo(int fwd)
{
	printf("moveServo %d\n", fwd);
	for(int i=0; i < 30; i++)
	{
		gpio_set_level(SERVO_PIN,1);
		usleep(900 + (fwd ? 1200 : 0));
		gpio_set_level(SERVO_PIN,0);
		vTaskDelay(20/portTICK_PERIOD_MS); 
	}
	// for(int step=0; step < 11; step++)
	// {
		
	// }
}

void attack()
{
	moveServo(1);
	vTaskDelay(1000/portTICK_PERIOD_MS);
	moveServo(0);
}

void upd_distance(float dist, QueueHandle_t dac_queue)
{
	nupdates++;
	
	// if(!(nupdates % 5))//nupdates >= last_said && session.attack == 1)
	// {
	// 	attack();
	// 	session.attack = 2;
	// }

	cur_lvl = -1;
	float threshold;
	for(int i=0; i < 4; i++)
	{
		threshold = approach_lvls[i];
		if(last_lvl >= i)
			threshold += HYSTERESIS_ADD;
		if(dist <= threshold)
		{
			cur_lvl = i;
			//printf("Thr: %.3f\n",threshold);
		}
		else
		{
			break;
		}
	}

	printf("%d: Cur lvl: %d; Last lvl: %d; ses.started: %d; last_said: %d\n", nupdates, cur_lvl, last_lvl, session.started, last_said);
	

	if(cur_lvl >= 0)
	{
		session.destroy_countdown = 10;
		if(last_act_lvl == cur_lvl)
		{
			if(cur_lvl > 0 && canSay(0))
			{
				say(cur_lvl);
			}
		}
	}

	if(!session.started && cur_lvl >= 0)
	{
		if(nupdates > 10)
		{
			session = (session_t) {
				.started = nupdates,
				.lvl_messages = {0,0,0},
				.destroy_countdown = 10, //5 sec
				.attack = 0
			};
			printf("new session\n");
		}
	}
	
	if(session.started && cur_lvl < 0 && nupdates > last_said)
	{
		if(session.destroy_countdown-- <= 0)
		{
			session.started = 0;
			last_said = 0;
			last_act_lvl = -1;
			printf("closing session\n");
		}
		
	}
	
	if(session.started && cur_lvl > 0 && last_lvl > 0 && cur_lvl != last_act_lvl)
	{
		if(canSay(1))
			say(cur_lvl);
	}

	last_lvl = cur_lvl;
	if(cur_lvl > 0)
		last_act_lvl = cur_lvl;
}
