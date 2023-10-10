#include <stdio.h>

int addTwoNumbers(int a, int b)
{
    int result;
    result = a + b;
    return result;
}

int getCount()
{
    static int count = 0;
    return count++;
}

int main()
{
    int op1, op2, res;
    printf("Введите два числа: ");
    scanf("%d %d", &op1, &op2);

    res = addTwoNumbers(op1, op2);

    printf("Результат: %d\n", res);
    return 0;
}


