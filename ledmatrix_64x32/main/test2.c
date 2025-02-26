#include <stdio.h>
#include <inttypes.h>

uint16_t rawsym[] = {0x0123, 0x4567};

int main()
{
    uint32_t q = *((uint32_t *) rawsym);
    printf("%x\n",q);
}