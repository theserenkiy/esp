#include <stdio.h>

int main()
{
    char name[256];

    printf("Привет! Как тебя зовут?\n");
    fgets(name, 256, stdin);
    printf("Здравствуй, %s\n", name);
}


