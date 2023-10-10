#include <stdio.h>

int main()
{
    float r1, r2, result;

    printf("Введите сопротивления двух резисторов через пробел: ");

    scanf("%f %f", &r1, &r2);

    result = 1 / ((1/r1) + (1/r2));

    printf("Rэ = %.2f",result);
}


