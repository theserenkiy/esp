#include <stdio.h>
#include <unistd.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <inttypes.h>
#include <string.h>

void app_main()
{
    char s[] = "AZazАЯая";

    for(int i=0; i < strlen(s); i++)
        printf("%u (0x%x)", s[i], s[i]);

}

