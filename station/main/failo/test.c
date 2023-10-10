#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fp = fopen("test2_u8.raw", "r");
    char buf[1024];
    int bytes, total = 0;
    while(!feof(fp))
    {
        bytes = fread(buf, 1, 1024, fp);
        total += bytes;
        printf("%d bytes read; total: %d\n", bytes, total);
    }
    fclose(fp);

    return 0;
}