#include <stdio.h>
#include <stdlib.h>

int main()
{
    int *data;
    int data_size = 3;

    //выделяем data_size пачек по 4 байта
    //(4 байта - размер переменной типа int)
    data = (int *)malloc(data_size * 4);

    printf("Введите 3 числа: ");

    //обращаемся к выделенной памяти 
    //используя арифметику указателей
    scanf("%d %d %d", data, data+1, data+2);

    for(int i=0; i < data_size; i++)
    {
        //обращаемся с выделенной памятью как с массивом
        printf("%d\n", data[i]);
    }

    return 0;
}


