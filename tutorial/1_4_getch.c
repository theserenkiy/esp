#include <stdio.h>

int main()
{
    //объявляем символьную переменную user_input
    unsigned char user_input;            

    //печатаем приглашение пользователю
    printf("Введите любой символ и нажмите Enter...\n");

    //получаем символ, введенный пользователем
    user_input = getchar();

    //выводим результат
    printf("Вы ввели %c (%u) \n", user_input, user_input);
}