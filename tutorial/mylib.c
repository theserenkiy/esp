#include <stdio.h>

int readIntegersFromFile(const char *filename, int *arr, int maxlen)
{
    FILE *file = fopen(filename, "r");
    int i;

    for(i = 0; i < maxlen; i++)
    {
        if(EOF == fscanf(file, "%d", &arr[i]))
            break;
    }
    return i;
}