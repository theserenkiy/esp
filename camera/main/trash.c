
void memtest()
{
	printf("All heaps free size: %dk\n",(int)(heap_caps_get_free_size(MALLOC_CAP_8BIT)/1024));
	printf("Checking frame buffer...\n");
	uint8_t *ptr;
	for(int i=0;i < 40; i++)
	{
		ptr = malloc(8192);
		printf("Allocated %dk; result: %p\n",i*8,ptr);
		if(!ptr)
			break;
	}
}

void benchmark()
{
	uint64_t delay, time0 = esp_timer_get_time();
	for(int i=0; i < 10000000; i++)
	{
	}
	delay = esp_timer_get_time()-time0;
	printf("Delay: %llu us, %.3f us per iter",delay,(float)delay/10000000);
}


void benchmark_gpio()
{
	int niter = 10000000;
	uint64_t delay, time0 = esp_timer_get_time();
	for(int i=0; i < niter; i++)
	{
		gpio_get_level(PIN_PCLK);
	}
	delay = esp_timer_get_time()-time0;
	printf("Read: %.3f us per iter\n",(float)delay/niter);
	time0 = esp_timer_get_time();
	for(int i=0; i < niter; i++)
	{
		gpio_set_level(PIN_PCLK,i%2);
	}
	delay = esp_timer_get_time()-time0;
	printf("Write: %.3f us per iter\n",(float)delay/niter);
}

int scanPixels()
{
	int val,lastval = 0;
	int npixels = 0;
	uint64_t waitUntil = 0;
	uint64_t bench;
	for(;;)
	{
		if(gpio_get_level(PIN_VSYNC))
			break;
	}
	for(;;)
	{
		if(!gpio_get_level(PIN_VSYNC))
			break;
	}
	for(int i=0;i < 50000000;i++)
	{
		val = gpio_get_level(PIN_PCLK);
		if(val)
		{
			//posedge
			if(!lastval)
			{
				npixels++;
				//waitUntil = esp_timer_get_time()+FRAME_DETECT_DELAY_US;
			}
			// else{
			// 	if(npixels > 0 && waitUntil < esp_timer_get_time())
			// 		return npixels;
			// }
		}
		if(!(i % 100))
			if(gpio_get_level(PIN_VSYNC))
				break;
		lastval = val;
	}
	return npixels;
}

int calcPixels()
{
	for(;;)
	{
		if(!gpio_get_level(PIN_HSYNC))
			break;
	}
	for(;;)
	{
		if(gpio_get_level(PIN_HSYNC))
			break;
	}
	pixel_cnt = 0;
	
	for(;;)
	{
		if(!gpio_get_level(PIN_HSYNC))
			break;
		//vTaskDelay(10/portTICK_PERIOD_MS);
	}
	return pixel_cnt;
}

int calcRows()
{
	for(;;)
	{
		if(gpio_get_level(PIN_VSYNC))
			break;
	}
	for(;;)
	{
		if(!gpio_get_level(PIN_VSYNC))
			break;
	}
	row_cnt = 0;
	
	for(;;)
	{
		if(gpio_get_level(PIN_VSYNC))
			break;
		//vTaskDelay(10/portTICK_PERIOD_MS);
	}
	return row_cnt;
}