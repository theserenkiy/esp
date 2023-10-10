#include <stdio.h>
#include "mylib.c"

int main()
{
    int numbers[10];

    int count = readIntegersFromFile("numbers.txt",numbers,10);    

    printf("Прочитано %d чисел\n",count);

    for(int j = 0; j < count; j++)
    {
        printf("%d ", numbers[j]);
    }

    return 0;
}
