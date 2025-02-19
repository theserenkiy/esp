
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "soc/gpio_struct.h"
#include "driver/gptimer.h"
#include "image.c"
#include "gamma.c"

#define PIN_A 21
#define PIN_B 19
#define PIN_C 23
#define PIN_D 22

#define PIN_R1 15
#define PIN_R2 17

#define PIN_G1 5
#define PIN_G2 16

#define PIN_B1 2
#define PIN_B2 18

#define PIN_LAT 13
#define PIN_CLK 12
#define PIN_OE 14

#define GET_BIT(v,n) ((v >> n) & 1)
#define REMAP_BIT(v,s,d) (GET_BIT(v,s) << d)
#define REMAP_BIT_N(v,s,d) ((!GET_BIT(v,s)) << d)

// #define MASK_BIT(v,n) (v & (1 << n))
// #define REMAP_BIT_MASKED(v,s,d) (s==d ? v : ((s < d) ? (v << (d-s)) : (v >> (s-d)))) 
// #define REMAP_BIT(v,s,d) REMAP_BIT_MASKED(MASK_BIT(v,s),s,d)
// #define REMAP_BIT_N(v,s,d) REMAP_BIT_MASKED(MASK_BIT(v,s) ^ (1 << s),s,d)

int pins[] = {
	PIN_A,PIN_B,PIN_C,PIN_D,PIN_LAT,PIN_OE,
	PIN_R1,PIN_R2,PIN_B1,PIN_B2,PIN_G1,PIN_G2,PIN_CLK
};
spi_device_handle_t spi0, spi1;

uint8_t row[] = {0xff,0x21,0x0c,0xff,0x21,0x0c,0xfd,0x30,0x0c,0xfd,0x30,0x0c,0xfd,0x30,0x0c,0xff,0x42,0x0b,0xff,0x42,0x0b,0xff,0x57,0x0a,0xff,0x57,0x0a,0xfd,0x6f,0x07,0xfd,0x6f,0x07,0xff,0x85,0x03,0xff,0x85,0x03,0xff,0x85,0x03,0xff,0x9e,0x02,0xff,0x9e,0x02,0xfc,0xb2,0x04,0xfc,0xb2,0x04,0xec,0xc4,0x0b,0xec,0xc4,0x0b,0xec,0xc4,0x0b,0xd7,0xd3,0x16,0xd7,0xd3,0x16,0xb9,0xde,0x24,0xb9,0xde,0x24,0x93,0xe5,0x35,0x93,0xe5,0x35,0x60,0xea,0x49,0x60,0xea,0x49,0x60,0xea,0x49,0x02,0xeb,0x5f,0x02,0xeb,0x5f,0x00,0xe9,0x77,0x00,0xe9,0x77,0x00,0xe3,0x91,0x00,0xe3,0x91,0x00,0xe3,0x91,0x00,0xd9,0xaa,0x00,0xd9,0xaa,0x00,0xca,0xc0,0x00,0xca,0xc0,0x00,0xb6,0xd2,0x00,0xb6,0xd2,0x01,0xa0,0xdf,0x01,0xa0,0xdf,0x01,0xa0,0xdf,0x00,0x86,0xe7,0x00,0x86,0xe7,0x11,0x6e,0xe9,0x11,0x6e,0xe9,0x4b,0x58,0xe8,0x4b,0x58,0xe8,0x4b,0x58,0xe8,0x67,0x44,0xe2,0x67,0x44,0xe2,0x80,0x36,0xdf,0x80,0x36,0xdf,0x95,0x2b,0xd9,0x95,0x2b,0xd9,0xa7,0x24,0xd2,0xa7,0x24,0xd2,0xa7,0x24,0xd2,0xb6,0x20,0xcf,0xb6,0x20,0xcf};

typedef struct {
    uint64_t event_count;
} example_queue_element_t;

static bool example_timer_on_alarm_cb(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx)
{
    BaseType_t high_task_awoken = pdFALSE;
    gptimer_stop(timer);
	gpio_set_level(PIN_OE,1);
    return high_task_awoken == pdTRUE;
}



void spi_tx(uint8_t *buf, int nbits)
{
	esp_err_t ret;
	spi_transaction_t t;
	memset(&t, 0, sizeof(t));
	//memset(&t, 0, sizeof(t));     //Zero out the transaction
	t.length=nbits;                    //Len is in bytes, transaction length is in bits.
	t.addr=-1;
	t.cmd=-1;
	t.tx_buffer = buf;
	t.rx_buffer = NULL;
	t.user=(void*)0;
	t.flags = (SPI_TRANS_MODE_QIO);
	spi_transaction_t * pt = &t;
	spi_device_queue_trans(spi0, &t, portMAX_DELAY);
	spi_device_queue_trans(spi1, &t, portMAX_DELAY);
	spi_device_get_trans_result(spi0, &pt, portMAX_DELAY);
	usleep(100);
	// ret=spi_device_polling_transmit(spi, &t);  //Transmit!
	// assert(ret==ESP_OK);            //Should have had no issues.
}



void spi_init(spi_device_handle_t * spi, spi_host_device_t host, int d0, int d1, int d2, int clk)
{
	esp_err_t ret;
	spi_bus_config_t buscfg = {
		.data0_io_num=d0,
		.data1_io_num=d1,
		.data2_io_num=d2,
		.data3_io_num=-1,
		.data4_io_num=-1,
		.data5_io_num=-1,
		.data6_io_num=-1,
		.data7_io_num=-1,
		.sclk_io_num=clk,
		.max_transfer_sz=4092,
		//.flags=(SPICOMMON_BUSFLAG_MASTER)
	};
	spi_device_interface_config_t devcfg = {
		.clock_speed_hz=100000,
		.mode=1,
		.spics_io_num=-1,
		.queue_size=1,
		.flags = SPI_DEVICE_HALFDUPLEX
	};
	ret=spi_bus_initialize(host, &buscfg, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(ret);
	ret=spi_bus_add_device(host, &devcfg, spi);
	ESP_ERROR_CHECK(ret);
}

void bb_tx(uint8_t * buf)
{

	for(int i = 0; i < 32; i++)
	{
		GPIO.out_w1ts = (
			REMAP_BIT(buf[i],4,PIN_R1) |
			REMAP_BIT(buf[i],5,PIN_G1) |
			REMAP_BIT(buf[i],6,PIN_B1) |
			REMAP_BIT(buf[i],4,PIN_R2) |
			REMAP_BIT(buf[i],5,PIN_G2) |
			REMAP_BIT(buf[i],6,PIN_B2) 
		);
		GPIO.out_w1tc = (
			REMAP_BIT_N(buf[i],4,PIN_R1) |
			REMAP_BIT_N(buf[i],5,PIN_G1) |
			REMAP_BIT_N(buf[i],6,PIN_B1) |
			REMAP_BIT_N(buf[i],4,PIN_R2) |
			REMAP_BIT_N(buf[i],5,PIN_G2) |
			REMAP_BIT_N(buf[i],6,PIN_B2) 
		);
		gpio_set_level(PIN_CLK,1);
		gpio_set_level(PIN_CLK,0);
		// GPIO.out_w1tc = (
		// 	1 << PIN_CLK
		// );
		GPIO.out_w1ts = (
			REMAP_BIT(buf[i],0,PIN_R1) |
			REMAP_BIT(buf[i],1,PIN_G1) |
			REMAP_BIT(buf[i],2,PIN_B1) |
			REMAP_BIT(buf[i],0,PIN_R2) |
			REMAP_BIT(buf[i],1,PIN_G2) |
			REMAP_BIT(buf[i],2,PIN_B2) 
		);
		GPIO.out_w1tc = (
			REMAP_BIT_N(buf[i],0,PIN_R1) |
			REMAP_BIT_N(buf[i],1,PIN_G1) |
			REMAP_BIT_N(buf[i],2,PIN_B1) |
			REMAP_BIT_N(buf[i],0,PIN_R2) |
			REMAP_BIT_N(buf[i],1,PIN_G2) |
			REMAP_BIT_N(buf[i],2,PIN_B2) 
		);
		gpio_set_level(PIN_CLK,1);
		gpio_set_level(PIN_CLK,0);
		// GPIO.out_w1tc = (
		// 	1 << PIN_CLK
		// );
	}
}

void bb_tx_truecolor(uint8_t* row, int rownum, int bit)
{
	int pix[3], pixl[3];
	float k = rownum/32.0;
	float kl = (rownum+16)/32.0;
	for(int i=0; i < 64*3; i+=3)
	{
		pix[0] =  gamma[(int)(row[i]*k)];
		pix[1] =  gamma[(int)(row[i+1]*k)];
		pix[2] =  gamma[(int)(row[i+2]*k)];
		pixl[0] = gamma[(int)(row[i]*kl)];
		pixl[1] = gamma[(int)(row[i+1]*kl)];
		pixl[2] = gamma[(int)(row[i+2]*kl)];
		GPIO.out_w1ts = (
			REMAP_BIT(pix[0],bit,PIN_R1) |
			REMAP_BIT(pixl[0],bit,PIN_R2) |
			REMAP_BIT(pix[1],bit,PIN_G1) |
			REMAP_BIT(pixl[1],bit,PIN_G2) |
			REMAP_BIT(pix[2],bit,PIN_B1) |
			REMAP_BIT(pixl[2],bit,PIN_B2) 
		);

		GPIO.out_w1tc = (
			REMAP_BIT_N(pix[0],bit,PIN_R1) |
			REMAP_BIT_N(pixl[0],bit,PIN_R2) |
			REMAP_BIT_N(pix[1],bit,PIN_G1) |
			REMAP_BIT_N(pixl[1],bit,PIN_G2) |
			REMAP_BIT_N(pix[2],bit,PIN_B1) |
			REMAP_BIT_N(pixl[2],bit,PIN_B2) 
		);
		gpio_set_level(PIN_CLK,1);
		gpio_set_level(PIN_CLK,0);
	}
}

void setLineAddr(int addr)
{
	gpio_set_level(PIN_OE,1);
	GPIO.out_w1ts = (
		REMAP_BIT(addr,3,PIN_D) |
		REMAP_BIT(addr,2,PIN_C) |
		REMAP_BIT(addr,1,PIN_B) |
		REMAP_BIT(addr,0,PIN_A)
	);

	GPIO.out_w1tc = (
		REMAP_BIT_N(addr,3,PIN_D) |
		REMAP_BIT_N(addr,2,PIN_C) |
		REMAP_BIT_N(addr,1,PIN_B) |
		REMAP_BIT_N(addr,0,PIN_A)
	);
	gpio_set_level(PIN_LAT,1);
	gpio_set_level(PIN_LAT,0);
	
}

void app_main(void)
{
	for(int i=0;i < 13;i++)
	{
		gpio_reset_pin(pins[i]);
		gpio_set_direction(pins[i],GPIO_MODE_OUTPUT);
		gpio_set_level(pins[i],0);
	}

	gptimer_handle_t gptimer = NULL;
	gptimer_config_t timer_config = {
		.clk_src = GPTIMER_CLK_SRC_DEFAULT,
		.direction = GPTIMER_COUNT_UP,
		.resolution_hz = 30 * 1000 * 1000, // 1 tick = 25ns
	};
	ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &gptimer));

	gptimer_alarm_config_t alarm_config = {
		.alarm_count = 100, // initial alarm target = 1s @resolution 1MHz
	};
	ESP_ERROR_CHECK(gptimer_set_alarm_action(gptimer, &alarm_config));
	
	gptimer_event_callbacks_t cbs = {
		.on_alarm = example_timer_on_alarm_cb, // register user callback
	};
	ESP_ERROR_CHECK(gptimer_register_event_callbacks(gptimer, &cbs, NULL));

	gptimer_enable(gptimer);
	
	//spi_init(&spi0, SPI2_HOST, PIN_R1, PIN_G1, PIN_B1, PIN_CLK);
	//spi_init(&spi1, SPI3_HOST, PIN_R2, PIN_G2, PIN_B2, -1);

	uint8_t data[] = {
		0b00010011, 0b00100110, 0b01000101, 0b01110000
	};
	uint8_t buf[64];

	// for(int i=0; i < 32; i++)
	// {
	// 	buf[i] = (
	// 		REMAP_BIT(row[i*6],)
	// 	);//data[i%4];
	// 			// ((row[i*6] & 0x80) >> 3) | ((row[i*6+1] & 0x80) >> 2) | ((row[i*6+2] & 0x80) >> 1) 
	// 			// | ((row[i*6+3] & 0x80) >> 7) | ((row[i*6+4] & 0x80) >> 6) | ((row[i*6+5] & 0x80) >> 5);
	// }

	gptimer_alarm_config_t aconfs[12];

	for(int i=0; i < 12; i++)
	{
		aconfs[i].alarm_count = 1 << i;
	}

	uint64_t count;
	int measured = 0;
	while(1){
		for(int i=0; i < 16; i++)
		{
			for(int j=0; j < 12; j++)
			{
				// if(!count)
				// 	gptimer_set_raw_count(gptimer,0);
				
				bb_tx_truecolor(row,i,j);
				// if(!count)	
				// 	gptimer_get_raw_count(gptimer, &count);
				setLineAddr(i);
				gptimer_set_raw_count(gptimer,0);
				gptimer_set_alarm_action(gptimer, &aconfs[j]);
				gpio_set_level(PIN_OE,0);
				gptimer_start(gptimer);
			}			
		}
		// if(!measured)
		// {
		// 	printf("Timer: %lld\n",count);
		// 	measured = 1;
		// }
			
		//gpio_set_level(PIN_OE,1);
		//vTaskDelay(10/portTICK_PERIOD_MS);
	}
	
}
