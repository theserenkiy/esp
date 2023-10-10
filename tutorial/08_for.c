#include <stdio.h>

int main()
{
    int count;

    printf("Введите до скольки сосчитать: ");

    scanf("%d", &count);

    for(int i = 0; i < count; i++)
    {
        printf("%d\n", i);
    }
    
    printf("Ура, мы сосчитали до %d!\n", count);

}



