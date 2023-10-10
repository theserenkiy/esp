#include <stdio.h>

int main()
{
    int a = 123456;
    void *ptr;
    ptr = &a;       
    
    int b = *(int *)ptr;

    printf("%d\n", b);
    return 0;
}

