#include <stdio.h>
#include <inttypes.h>
#include "gamma.c"
#include "image.c"

#define MASK_BIT(v,n) (v & (1 << n))
#define REMAP_BIT_MASKED(v,s,d) (s==d ? v : ((s < d) ? (v << (d-s)) : (v >> (s-d)))) 
#define REMAP_BIT(v,s,d) REMAP_BIT_MASKED(MASK_BIT(v,s),s,d)
#define REMAP_BIT_N(v,s,d) REMAP_BIT_MASKED(MASK_BIT(v,s) ^ (1 << s),s,d)

#define HALFSCR_PIX 64*16
#define HALFSCR_SUBPIX	HALFSCR_PIX*3

typedef struct {
	uint16_t R1;
	uint16_t G1;
	uint16_t B1;
	uint16_t R2;
	uint16_t G2;
	uint16_t B2;
} pixel_t;

pixel_t im[HALFSCR_PIX];

void prepare_image(uint8_t *image, pixel_t *out)
{
	int p, p2;
	for(int i=0; i < HALFSCR_PIX; i++)
	{
		p = i*3;
		p2 = p + HALFSCR_SUBPIX;

		out[i].R1 = gamma[image[p]];
		out[i].G1 = gamma[image[p+1]];
		out[i].B1 = gamma[image[p+2]];
		out[i].R2 = gamma[image[p2]];
		out[i].G2 = gamma[image[p2+1]];
		out[i].B2 = gamma[image[p2+2]];
	}
}
int main()
{
	prepare_image(image,im);

	FILE *fp = fopen("imdump.bin","wb");
	fwrite(im, 12, HALFSCR_PIX, fp);
	fclose(fp);
	// for(int i=0; i < HALFSCR_PIX; i++)
	// {

	// }
}