#include "stdio.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "fat.c"
#include "wifi.c"

void app_main(void)
{
	mount_partition("files","/storage");
	char buf[1024];

	FILE *fp = fopen("/storage/test.txt","r");
	int sz = fread(&buf, sizeof(buf), 1, fp);
	buf[sz] = '\0';
	fclose(fp);

	printf("Contents of file: %s\n", buf);

	while(1){
		printf("Another boooring iteration........\n");
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}
}
