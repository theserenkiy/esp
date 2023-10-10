#include <stdio.h>

int main()
{
    struct rec {
        char name[20];
        int note;
    };

    struct rec record;
    FILE *file = fopen("database1.txt", "r");

    while(EOF != fscanf(file,"%20s %d", record.name, &record.note))
    {
        printf("Имя: %s, оценка: %d\n", record.name, record.note);
    }

    return 0;
}