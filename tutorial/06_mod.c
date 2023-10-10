#include <stdio.h>

int main()
{
    int seconds, minutes;

    printf("Введите количество секунд: ");

    scanf("%d", &seconds);

    minutes = seconds / 60;
    seconds = seconds % 60;

    printf("Время: %dмин %dсек", minutes, seconds);
} 


