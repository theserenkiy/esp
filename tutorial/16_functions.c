#include <stdio.h>

//объявляем функцию 
//возвращаемое значение - типа int
//принимает два аргумента: a и b
int addTwoNumbers(int a, int b)
{
    int result;
    result = a + b;
    return result;
}

int main()
{
    int op1, op2, res;
    printf("Введите два числа: ");
    scanf("%d %d", &op1, &op2);

    //вызываем функцию, объявленную выше
    res = addTwoNumbers(op1, op2);

    printf("Результат: %d\n", res);
    return 0;
}


