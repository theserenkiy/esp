#include <stdio.h>

int main()
{
    float r1;
    float r2;
    printf("Введите два сопротивления через через пробел: ");

    scanf("%f %f", &r1, &r2);

    float re = 1/(1/r1 + 1/r2);

    printf("Ответ:\nЭквивалентное сопротивление: %f Ом\n", re);
}