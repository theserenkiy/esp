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
	float dx = coord[0]-AREA_CENTER;
	float adx = dx < 0 ? -dx : dx;
	float v = sqrtf(powf(absy,2) + powf(dx,2));
	float alpha = atanf(adx/absy);
	float beta = acosf((v/2)/HANDLEN);
	float gamma = (M_PI/2) + ((dx > 0) ? -alpha : alpha);

	printf("absy: %.3f; v: %.3f; A: %.3f; B: %.3f; G: %.3f\n", absy, v, alpha*RAD, beta*RAD, gamma*RAD);
	angles[0] = (gamma-beta)*RAD;
	angles[1] = (M_PI-gamma-beta)*RAD;
}

int main()
{
	float coord[2], angles[] = {0,0};
	while(1)
	{
		printf("Enter coords (float, 0...1): ");
		scanf("%f%f",&coord[0],&coord[1]);
		coord2angles(coord,angles);
		printf("Angles: %.3f   %.3f\n-----------\n",angles[0], angles[1]);
	}
	
	return 0;
}


