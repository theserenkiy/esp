#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"

//#include "common.h"

static QueueHandle_t gpio_evt_queue = NULL;
static QueueHandle_t dac_queue = NULL;
static QueueHandle_t blink_queue = NULL;
volatile int dac_active = 0;

#include "dac.c"
#include "blinker.c"

//#define DEVICE_TEST

#define TX_PIN 26
#define RX_PIN 27
#define SERVO_PIN 15
#define DIST_FILTER_SIZE 8
#define MOTOR_0_IN1_PIN	17
#define MOTOR_0_IN2_PIN	5
#define MOTOR_1_IN1_PIN	18
#define MOTOR_1_IN2_PIN	19

#define MOTOR_ACCEL_STEP 10

#include "distance_reaction.c"
#include "motor.c"


volatile uint64_t sonar_tx_time;
volatile float distance;
static int filenum = 1;


static void IRAM_ATTR sonar_on_rx(void *arg)
{
	static uint32_t time;
	static uint64_t curtime;
	curtime = esp_timer_get_time();
	if(gpio_get_level(RX_PIN))
	{
		sonar_tx_time = curtime;
		return;
	}
	time = (uint32_t)(curtime-sonar_tx_time);
	xQueueSendFromISR(gpio_evt_queue, &time, NULL);
}

void sonar_task(void *arg)
{
	gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

	uint32_t time;
	float dist;
	float dist_filter[DIST_FILTER_SIZE];
	uint8_t	df_ptr = 0;
	float df_min = 0;
	while(1)
	{
		gpio_set_level(TX_PIN,1);
		usleep(9);
		gpio_set_level(TX_PIN,0);

		///printf("TX sent\n");

		if(xQueueReceive(gpio_evt_queue, &time, portMAX_DELAY))
		{
			//("RX received\n");
			dist = ((float)time)/5882;
			dist_filter[(df_ptr++) % DIST_FILTER_SIZE] = dist;
			df_min = 5;
			for(int i=0; i < DIST_FILTER_SIZE; i++)
			{
				if(dist_filter[i] < df_min)
					df_min = dist_filter[i];
			}
			distance = df_min;	

			//printf("%.3f	%.3f\n", dist, df_min);

			if(dist < 4)
			 	vTaskDelay(20/portTICK_PERIOD_MS);
		}
	}
}


int dac_task(void *arg)
{
	dac_queue = xQueueCreate(10, sizeof(uint32_t));

	dac_continuous_handle_t dac = dac_continious_init(16000);
	FILE *fp;
	char fname[8];
	int bytes;
	int first_read = 0;
	float step;
	int ramp_startpos;
	int filenum;
	for(;;)
	{
		if(xQueueReceive(dac_queue, &filenum, portMAX_DELAY))
		{
			dac_active = 1;
			sprintf(fname,"/files/%02d.wav",filenum);
			printf("Filename %s\n",fname);
			fp = fopen(fname,"r");
			if(!fp)
			{
				printf("Cannot open audio file :(\n");
				return -1;
			}
			first_read = 1;

			while(!feof(fp))
			{
				bytes = fread(&buf, 1, 2048, fp);
				//ramping begin of the file
				if(first_read)
				{
					step = ((float)buf[256]-128)/256;
					for(int j=0;j < 256;j++)
					buf[j] = 128+(int)(j*step);
					memset(buf, 128, 256);
				}
				first_read=0;
				//printf("Bytes read: %d\n",bytes);

				//ramping end of the file
				//and filling the rest of buffer with '128' if necessary
				if(bytes < 2048)
				{
					ramp_startpos = bytes > 256 ? bytes-256 : 0;
					step = ((float)buf[ramp_startpos]-128)/256;
					for(int j=0;j < 256;j++)
					buf[j] = 128+(int)((256-j)*step);
					memset(buf+bytes,128,2048-bytes);
				}

				// DAC write 
				dac_continuous_write(dac, buf, 2048, NULL, -1);
			}
			fclose(fp);
			dac_active = 0;
		}
	}
	
	return 0;
}

void blink_task(void *arg)
{
	while(1)
    {
        blink(is_session_started() ? 20 : 100);
		vTaskDelay(10/portTICK_PERIOD_MS);
    }
}


volatile int motor_control[] = {0,0};
volatile int motor_state[] = {0,0};
void motor_task(void *arg)
{
	int diff;
	int absdiff;
	int sign;
	while(1)
	{
		for(int motor = 0; motor < 2; motor++)
		{
			diff = motor_control[motor] - motor_state[motor];
			printf("diff = %d\n", diff);
			absdiff = diff < 0 ? -diff : diff;
			sign = diff < 0 ? -1 : 1;
			if(diff != 0)
			{
				motor_state[motor] += sign*(absdiff < MOTOR_ACCEL_STEP ? absdiff : MOTOR_ACCEL_STEP);
			}
			setMotorPower(motor, motor_state[motor]);
		}

		vTaskDelay(50/portTICK_PERIOD_MS);
	}
}


void app_main(void)
{
	printf("Hello world!\n");

	distance_reaction_init();

	if(mount_partition("files","/files") < 0)
	{
		printf("Failed to mount partition\n");
		return;
	}

	ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE, 
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .freq_hz          = 15000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);
	blinker_init();

	xTaskCreate(dac_task,"dac_task",4096,NULL,10,NULL);
	xTaskCreate(blink_task,"blink_task",2048,NULL,10,NULL);

	//servo pin
	gpio_reset_pin(SERVO_PIN);
	gpio_set_direction(SERVO_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(SERVO_PIN, 0);

	gpio_install_isr_service(0);

	gpio_reset_pin(TX_PIN);
	gpio_set_direction(TX_PIN, GPIO_MODE_OUTPUT);

	gpio_reset_pin(RX_PIN);
	gpio_set_direction(RX_PIN, GPIO_MODE_INPUT);
	gpio_intr_enable(RX_PIN);
	gpio_set_intr_type(RX_PIN, GPIO_INTR_ANYEDGE);
	gpio_isr_handler_add(RX_PIN, sonar_on_rx, NULL);

	xTaskCreate(sonar_task,"sonar_task",2048,NULL,10,NULL);

	motor_init();
	//xTaskCreate(motor_task,"motor_task",2048,NULL,10,NULL);
	
	int cnt = 0;
	int motor_controls[5][2] = {{100,100},{100,-100},{-100,-100},{-100,100},{0,100}};
	int key;
	while(1)
	{
		if(!dac_active)
		{
			//xQueueSend(dac_queue, &filenum, NULL);
			filenum = filenum==35 ? 1 : filenum+1;
		}

		printf("dist %.2f\n",distance);

		upd_distance(distance, dac_queue);
		printf("cnt: %d\n",cnt);

		// key = ((int)cnt/5)%5;
		// motor_control[0] = motor_controls[key][0];
		// motor_control[1] = motor_controls[key][1];
		
		cnt++;
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
