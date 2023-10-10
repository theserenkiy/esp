#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    // char filename[] = "myfile.txt";
    // if(access(filename,F_OK))
    // {
    //     printf("Файл %s не найден :(\n",filename);
    //     return -1;
    // }

    // char filename[] = "myfile.txt";
    // FILE *fp = fopen(filename, "w");
    // char text[] = "Hello from file!";
    // fwrite(text,1,16,fp);
    // fclose(fp);

    char filename[] = "myfile2.txt";
    FILE *fp = fopen(filename, "r");
    if(!fp)
    {
        printf("Файл %s не найден\n",filename);
        return -1;
    }
    char buffer[128];
    fread(buffer,1,128,fp);
    // while(!feof(fp))
    // {
    //     fgets(buffer,128,fp);
    //     printf("Прочитана строка: %s\n",buffer);
    // }
    
    printf("Прочитана строка: %s\n",buffer);
    fclose(fp);


    return 0;
}