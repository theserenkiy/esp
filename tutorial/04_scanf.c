#include <stdio.h>

int main()
{
    int age;
    float height;

    printf("Введите свой возраст и рост (в метрах) через пробел: ");

    scanf("%d %f", &age, &height);

    printf("Вам %d лет, и ваш рост - %f м!\n", age, height);
}


