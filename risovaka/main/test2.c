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
    printf("absy: %.3f; v: %.3f; A: %.3f; B: %.3f\n", absy, v, alpha*RAD, beta*RAD);
    angles[0] = (M_PI/2-beta-alpha)*RAD;
    angles[1] = (M_PI-2*beta-angles[0])*RAD;
}

int main()
{
    float coord[2], angles[2];
    while(1)
    {
        printf("Enter coords (float, 0...1): ");
        scanf("%f%f",&coord[0],&coord[1]);
        coord2angles(coord,angles);
        printf("Angles: %.3f   %.3f\n-----------\n",angles[0], angles[1]);
    }
    
    return 0;
}