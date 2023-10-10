#include <stdio.h>

char wifi_ssid[] = "radiogeeks";
char wifi_password[] = "multivibrator";

int connect_wifi()
{
    printf("Подключаемся к wifi %s с паролем %s\n", wifi_ssid, wifi_password);   
    //дальше идет какая-то черная магия...
    // .....
    //магия закончилась
    return 0;
}

int main()
{
    int err;
    err = connect_wifi();
    // .....
    return 0;
}


