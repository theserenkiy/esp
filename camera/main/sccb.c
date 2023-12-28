#include <unistd.h>
#include "driver/gpio.h"

#define SLEEP_LONG 10
#define SLEEP_SHORT 5

static uint32_t sda_is_output = 0;

void sccb_init()
{
	gpio_reset_pin(PIN_SCL);
	gpio_set_direction(PIN_SCL, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_SCL, 1);

	gpio_reset_pin(PIN_SDA);
	gpio_set_direction(PIN_SDA, GPIO_MODE_INPUT);
	gpio_set_pull_mode(PIN_SDA, GPIO_PULLUP_ONLY);
}

uint8_t sccb_send_byte(uint8_t data)
{
	uint8_t mask = 0x80;
    uint8_t ack;
    gpio_set_direction(PIN_SDA, GPIO_MODE_OUTPUT);

    for(int i=0; i < 8; i++)
	{
        usleep(SLEEP_SHORT);
		gpio_set_level(PIN_SDA, data & mask);
        usleep(SLEEP_SHORT);
        gpio_set_level(PIN_SCL, 1);
        usleep(SLEEP_LONG);
        gpio_set_level(PIN_SCL, 0);
		mask = mask >> 1;
	}

    gpio_set_direction(PIN_SDA, GPIO_MODE_INPUT);
    usleep(SLEEP_SHORT);
    gpio_set_level(PIN_SCL, 1);
    usleep(SLEEP_LONG);
    ack = gpio_get_level(PIN_SDA);  //ACK
    gpio_set_level(PIN_SCL, 0);
    usleep(SLEEP_SHORT);
    return ack; 
}

uint8_t sccb_recv_byte()
{
    uint8_t result = 0;
    gpio_set_direction(PIN_SDA, GPIO_MODE_INPUT);
	for(int i=0; i < 8; i++)
	{
		usleep(SLEEP_SHORT);
		gpio_set_level(PIN_SCL, 1);
		usleep(SLEEP_LONG);
        result = (result | (gpio_get_level(PIN_SDA) ? 1 : 0)) << 1;
		gpio_set_level(PIN_SCL, 0);
		usleep(SLEEP_SHORT);
	}
    usleep(SLEEP_SHORT);
    gpio_set_level(PIN_SCL, 1);
    usleep(SLEEP_LONG);
    gpio_get_level(PIN_SDA);  //ACK
    gpio_set_level(PIN_SCL, 0);
    usleep(SLEEP_SHORT);
    return result;
}

uint8_t sccb_transaction(uint8_t id, uint8_t is_read, uint8_t addr, uint8_t data)
{
	uint8_t result = 0;
	id |= is_read; 
	gpio_set_direction(PIN_SDA,GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_SDA,0);                          //SDA --\__
	usleep(SLEEP_LONG);
	gpio_set_level(PIN_SCL, 0);                         //SCL ----\__
	usleep(SLEEP_LONG);

	printf("ACK: %d\n",sccb_send_byte(id));
    printf("ACK: %d\n",sccb_send_byte(addr));
	//sccb_send_byte(addr);
	if(is_read)
	{
		result = sccb_recv_byte();
	}
	else
		sccb_send_byte(data);

	gpio_set_level(PIN_SCL, 1);                         //SCL __/--
	usleep(SLEEP_LONG);
	gpio_set_level(PIN_SDA,1);                          //SDA ____/--
    gpio_set_direction(PIN_SDA,GPIO_MODE_INPUT);
    usleep(SLEEP_LONG);
	return result;
}