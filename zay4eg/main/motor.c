#include <stdio.h>

int cur_forward[] = {1,1};

void motor_init()
{
	gpio_reset_pin(MOTOR_0_IN2_PIN);
	gpio_set_direction(MOTOR_0_IN2_PIN,GPIO_MODE_OUTPUT);

	gpio_reset_pin(MOTOR_1_IN2_PIN);
	gpio_set_direction(MOTOR_1_IN2_PIN,GPIO_MODE_OUTPUT);

	gpio_reset_pin(MOTOR_0_IN1_PIN);
	gpio_set_direction(MOTOR_0_IN1_PIN,GPIO_MODE_OUTPUT);

	gpio_reset_pin(MOTOR_1_IN1_PIN);
	gpio_set_direction(MOTOR_1_IN1_PIN,GPIO_MODE_OUTPUT);

	ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE, 
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_10_BIT,
        .freq_hz          = 15000,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

	ledc_channel_config_t ledc_channel_2 = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_2,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_0_IN1_PIN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_2);

	ledc_channel_config_t ledc_channel_3 = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_3,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = MOTOR_1_IN1_PIN,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ledc_channel_config(&ledc_channel_3);

	
}

void setMotorPower(int motor_num, float power, int forward)
{
	printf("Motor %d set power: %f; fwd: %d\n", motor_num,power, forward);
    int chan = motor_num ? LEDC_CHANNEL_3 : LEDC_CHANNEL_2;
	int in1 = motor_num ? MOTOR_1_IN1_PIN : MOTOR_0_IN1_PIN;
	int in2 = motor_num ? MOTOR_1_IN2_PIN : MOTOR_0_IN2_PIN;
	
	if(forward != cur_forward[motor_num])
	{
		ledc_set_pin(forward ? in1 : in2, LEDC_LOW_SPEED_MODE, chan);
		gpio_set_level(forward ? in2 : in1,1);
	}
	cur_forward[motor_num] = forward;

    ledc_set_duty(LEDC_LOW_SPEED_MODE, chan, (int)(1023*power));
    // Update duty to apply the new value
    ledc_update_duty(LEDC_LOW_SPEED_MODE, chan);
}

void speed_change(){

}

void move(){


}