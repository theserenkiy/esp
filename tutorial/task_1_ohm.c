#include <stdio.h>

int main()
{
    float u;
    float r;
    printf("Введите напряжение (В) и сопротивление (Ом) через пробел: ");

    scanf("%f %f", &u, &r);

    float i = u/r;
    float p = u*i;

    printf("Ответ:\nТок = %f А. Рассеиваемая мощность = %f Вт\n", i, p);
}