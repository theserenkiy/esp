#include <stdio.h>

int main()
{
    int age;

    printf("Введите свой возраст: ");

    scanf("%d", &age);

    if(age < 18)
    {
        printf("Тебе еще ничего нельзя, иди в школу!");
    }
    else
    {
        printf("Дед, ты как?");
    }
}


