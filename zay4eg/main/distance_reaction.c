#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#ifdef TESTING

#define HYSTERESIS_ADD	0.3
#define CANSAY_DELAY	1
#define CANSAY_RAND		1
float approach_steps[] = {3, 2, 1, 0.5};

#elif defined DEVICE_TEST

float approach_steps[] = {0.6, 0.45, 0.3, 0.2};
#define HYSTERESIS_ADD	0.15
#define CANSAY_DELAY	6
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
} session_t;

static int nupdates;
static int direction = 0;
static session_t session;
static int cur_step = -1, last_step = -1;
static int idle_counter = 0;
static int last_said = 0;

static int lvl0_msgs = {1,  3,           15, 19, 23, 24, 25,   29};
static int lvl1_msgs = {  2,   4, 8, 17,             26, 27,  30, 32, 33};
static int lvl2_msgs = {         5, 9, 10, 13, 14, 16, 28, 29, 31, 33};
static int lvl3_msgs = {		 5, 9, 10, 11, 12, 13, 14, 16, 18, 20, 22, 28, 29, 31, 32, 33};

static int *lvlmsgs[] = {lvl0_msgs, lvl1_msgs, lvl2_msgs, lvl3_msgs};

int canSay()
{
	session_messages++;
	return !last_said || (nupdates-last_said > CANSAY_DELAY && rand()%CANSAY_RAND == 0);
}

void say(char *msg)
{
	printf("		%s\n", msg);
	last_said = nupdates;
}

void new_session()
{
	printf("new session\n");
	if(!canSay())return;
	say("Privet!");
	session.introduced = 1;
}

void close_session()
{
	if(canSay())
		say("Bye!");
}

void approach(int lvl)
{
	if(lvl==1)
		say("Do not approach to me!");
	else if(lvl==2)
		say("Did you hear me? STOP!");
	else if(lvl==3)
		say("You're fearless son of bitch! Go away!");
}

void depart(int lvl)
{
	if(lvl==0)
		say("Good boy!");
	else if(lvl==1)
		say("You should be further away!");
	else if(lvl==2)
		say("Do not stop going away!");
}

void idle(int lvl)
{
	say("Do you still here? Go away I said!!!");
}

void upd_distance(float dist)
{
	nupdates++;
	cur_step = -1;
	float threshold;
	for(int i=0; i < 4; i++)
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
		session.destroy_countdown = 10;
	if(last_step == cur_step)
	{
		idle_counter++;
		if(cur_step < 0)return;

		if(canSay())
		{
			if(cur_step > 0)
				idle(cur_step);
			else if(!session.introduced)
				new_session();
		}
		
		return;
	}

	if(!session.started && cur_step >= 0)
	{
		session = (session_t) {
			.started = nupdates,
			.messages = 0,
			.introduced = 0,
			.approached = 0,
			.departured = 0,
			.destroy_countdown = 10 //5 sec
		};
		new_session();
	}
	else if(session.started && cur_step < 0)
	{
		if(destroy_countdown-- <= 0)
		{
			session.started = 0;
			// if(last_step > 0)
			// 	depart(0);
			// else if(session.introduced)
			// 	close_session();
		}
	}
	else if(cur_step > last_step)
	{
		approach(cur_step);
	}
	else
	{
		depart(cur_step);
		session.approached = 1;
	}

	last_step = cur_step;
}

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