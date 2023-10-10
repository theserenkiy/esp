#include <stdio.h>

int main()
{
    float a;
    float b;
    char op;
    float res;

    printf("Введите выражение (например 5 * 4): ");

    scanf("%f %c %f", &a, &op, &b);

    if(op=='+')
    {
        res = a + b;
    }
    else if(op=='-')
    {
        res = a - b;
    }
    else if(op=='*')
    {
        res = a * b;
    }
    else if(op=='/')
    {
        res = a / b;
    }
    else
    {
        printf("ОШИБКА! Некорректный знак мат. операции");
        return 1;
    }

    printf("= %f\n", res);
}