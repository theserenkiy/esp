#include <stdio.h>
#include <inttypes.h>

#define PIN_A 21
#define PIN_B 19
#define PIN_C 23
#define PIN_D 22

#define PIN_R1 15
#define PIN_R2 17

#define PIN_G1 5
#define PIN_G2 16

#define PIN_B1 2
#define PIN_B2 18

#define PIN_LAT 13
#define PIN_CLK 12
#define PIN_OE 14


uint8_t portmap[64];

void initPortmap()
{
    for(int i=0; i < 64;i++)
    {
        
    }
}