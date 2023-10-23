#include <stdio.h>
#include <arpa/inet.h>


int main()
{
    char ip[] = "127.0.0.1";
    int addr = inet_addr(ip);
    char aparts[4];
    sscanf(ip,"%u.%u.%u.%u",&aparts[0],&aparts[1],&aparts[2],&aparts[3]);
    int addr2 = aparts[3] << 24 | aparts[2] << 16 | aparts[1] << 8 | aparts[0];
    printf("%x  %x",addr,addr2);
    return 0;
}