#include <stdio.h>

int getCount()
{
    static int count = 0;
    return count++;
}

int main()
{
    printf("%d\n", getCount());
    printf("%d\n", getCount());
    printf("%d\n", getCount());
    printf("%d\n", getCount());
    return 0;
}


