#include <stdio.h>

int main()
{
    int sum = 0, number;



    printf("Вводите по одному ПОЛОЖИТЕЛЬНОМУ числу, для завершения введите 0.\n");
    for(;;)
    {
        scanf("%d", &number);
        if(!number)
        {
            break;
        }
        if(number < 0)
        {
            continue;
        }
        sum = sum + number;
    }
    printf("Сумма: %d\n", sum);
}


