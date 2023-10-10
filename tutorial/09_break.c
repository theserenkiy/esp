#include <stdio.h>

int main()
{
    int sum = 0, number;

    printf("Вводите по одному числу, для завершения введите 0.\n");
    for(;;)
    {
        scanf("%d", &number);
        if(!number)
        {
            break;
        }
        sum = sum + number;
    }
    printf("Сумма: %d\n", sum);
}


