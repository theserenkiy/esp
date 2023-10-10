#include <stdio.h>

int main()
{
    int numbers[32];
    int count = 0, num, sum = 0;

    printf("Введите несколько чисел (не больше 32), для завершения введите 0:\n");

    //читаем числа из пользовательского ввода, пишем их в массив
    for(int i=0; i<32; i++)
    {
        scanf("%d", &num);
        if(!num)
            break;
        
        numbers[i] = num;   //сохраняем число в массив
        count++;            //инкрементим счетчик прочитанных чисел
    }
    
    //суммируем числа из массива
    for(int i=0; i < count; i++)
    {
        sum += numbers[i];
    }

    printf("Сумма: %d\n",sum);
    return 0;
}


