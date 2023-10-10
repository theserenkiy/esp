#include <stdio.h>

int main()
{
    int months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int num;
    int *ptr;

    printf("Номер месяца: ");
    scanf("%u", &num);

    if(num < 1 || num > 12)
    {
        printf("Такого месяца нет!");
        return 1;
    }

    ptr = months + (num-1);

    printf("В %d-м месяце %d дней\n", num, *ptr);
    return 0;
}