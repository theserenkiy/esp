#include <stdio.h>
#include <stdlib.h>

int main()
{
    FILE *file = fopen("numbers.txt", "r");
    int numbers[10];

    int i;
    for(i = 0;i < 10;i++)
    {
        if(EOF == fscanf(file, "%d", &numbers[i]))
            break;
    }

    printf("Прочитано %d чисел\n",i);

    for(int j = 0; j < i; j++)
    {
        printf("%d ", numbers[j]);
    }

    return 0;
}