#include <stdio.h>

void foo(int arg1)
{
    arg1 += 1;
}

int main()
{
    int a = 1;
    foo(a);
    printf("a = %d\n", a);  // выведет "a = 1"
    return 0;
}


