#include <stdio.h>

int main()
{
    int age;
    char gender;

    printf("Введите свой возраст и пол (m/f): ");

    scanf("%d %c", &age, &gender);

    if(age < 18)
    {
        printf("Тебе еще ничего нельзя, иди в школу!\n");
    }
    else    // то есть, если age >= 18
    {
        if(gender == 'm')
        {
            printf("Добро пожаловать в Вооруженные Силы, сынок!\n");
        }
        else if(gender == 'f')
        {
            printf("А замуж то скоро?\n");
        }
        else
        { 
            printf("Какой-то странный у тебя пол...\n");
        }
    }
}


