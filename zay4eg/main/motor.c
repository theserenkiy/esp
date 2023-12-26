#include <stdio.h>
#include <stdlib.h>

int cur_forward[] = {1,1};

void cfg_pwm_channel(int chan, int gpio)
{
	ledc_channel_config_t ledc_channel = {
		.speed_mode     = LEDC_LOW_SPEED_MODE,
		.channel        = chan,
		.timer_sel      = LEDC_TIMER_0,
		.intr_type      = LEDC_INTR_DISABLE,
		.gpio_num       = gpio,
		.duty           = 0, // Set duty to 0%
		.hpoint         = 0
	};
	ledc_channel_config(&ledc_channel);
}

void motor_init()
{
	gpio_reset_pin(MOTOR_0_IN2_PIN);
	gpio_set_direction(MOTOR_0_IN2_PIN,GPIO_MODE_OUTPUT);
	gpio_set_level(MOTOR_0_IN2_PIN,0);

	gpio_reset_pin(MOTOR_0_IN1_PIN);
	gpio_set_direction(MOTOR_0_IN1_PIN,GPIO_MODE_OUTPUT);
	gpio_set_level(MOTOR_0_IN1_PIN,0);

	gpio_reset_pin(MOTOR_1_IN2_PIN);
	gpio_set_direction(MOTOR_1_IN2_PIN,GPIO_MODE_OUTPUT);
	gpio_set_level(MOTOR_1_IN2_PIN,0);

	gpio_reset_pin(MOTOR_1_IN1_PIN);
	gpio_set_direction(MOTOR_1_IN1_PIN,GPIO_MODE_OUTPUT);
	gpio_set_level(MOTOR_1_IN1_PIN,0);

	cfg_pwm_channel(LEDC_CHANNEL_2, MOTOR_0_IN1_PIN);	
	cfg_pwm_channel(LEDC_CHANNEL_3, MOTOR_1_IN1_PIN);	
}

void setMotorPower(int motor_num, int power)
{
	int forward = power >= 0;
	if(!forward)
		power *= -1;
	//printf("Motor %d set power: %d; fwd: %d\n", motor_num,power, forward);
	int chan = motor_num ? LEDC_CHANNEL_3 : LEDC_CHANNEL_2;
	int in1 = motor_num ? MOTOR_1_IN1_PIN : MOTOR_0_IN1_PIN;
	int in2 = motor_num ? MOTOR_1_IN2_PIN : MOTOR_0_IN2_PIN;
	
	if(forward != cur_forward[motor_num])
	{
		int low_pin = forward ? in2 : in1;
		cfg_pwm_channel(chan, forward ? in1 : in2);
		gpio_reset_pin(low_pin);
		gpio_set_direction(low_pin,GPIO_MODE_OUTPUT);
		gpio_set_level(low_pin,0);
	}
	cur_forward[motor_num] = forward;

	ledc_set_duty(LEDC_LOW_SPEED_MODE, chan, (int)(1023*power/100));
	ledc_update_duty(LEDC_LOW_SPEED_MODE, chan);
}
