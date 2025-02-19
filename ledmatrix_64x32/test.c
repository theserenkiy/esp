#include <stdio.h>

#define MASK_BIT(v,n) (v & (1 << n))
#define REMAP_BIT_MASKED(v,s,d) (s==d ? v : ((s < d) ? (v << (d-s)) : (v >> (s-d)))) 
#define REMAP_BIT(v,s,d) REMAP_BIT_MASKED(MASK_BIT(v,s),s,d)
#define REMAP_BIT_N(v,s,d) REMAP_BIT_MASKED(MASK_BIT(v,s) ^ (1 << s),s,d)

int main()
{
	// printf("%x\n",REMAP_BIT(7,2,2));
	// printf("%x\n",REMAP_BIT_N(7,3,3));

	char* str = "Preved";
	for(int i=0; ;i++)
	{
		if(str[i]==0)break;
		printf("Char: %c; ASCII: %d\n",str[i],str[i]);
	}

	int a = 123;
	int b = 5;
	float k = b / 10.0;
	printf("%.3f\n",k);
	a = (int)((float)a * k);
	printf("%d\n",a);
}