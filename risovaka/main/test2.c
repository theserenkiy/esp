#include <stdio.h>
#include <math.h>

#define HANDLEN 110
#define AREA_SIDE 100
#define TOP_POS 80
#define AREA_CENTER ((float)AREA_SIDE)/2
#define RAD (float)180/M_PI

void coord2angles(float *coord, float *angles)
{
    coord[0] *= AREA_SIDE;
    coord[1] *= AREA_SIDE;

    float absy = (coord[1]+TOP_POS);
    float v = sqrtf(powf(absy,2) + powf((coord[0]-AREA_CENTER),2));
    float alpha = acosf(absy/v);
    float beta = acosf((v/2)/HANDLEN);
    angles[0] = (M_PI/2-beta-alpha)*RAD;
    angles[1] = (M_PI-2*beta-angles[0])*RAD;
}

int main()
{
    float coord, angles;
    while(1)
    {
        
    }
    printf("Angle: %.3f\n",ang*180/M_PI);
    return 0;
}