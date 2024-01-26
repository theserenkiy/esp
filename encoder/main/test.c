#include <stdio.h>

int angles[] = {
	400,	//0
	460,	//10
	530,	//20
	600,	//30
	670,	//40
	745,	//50
	820,	//60
	900,	//70
	970,	//80
	1070,	//90
	1150,	//100
	1235,	//110
	1320,	//120
	1400,	//130
	1490,	//140
	1570,	//150
	1660,	//160
	1740,	//170
	1830,	//180

};

int servo_get_angle_duty(int ang)
{
	if(ang < -90)ang = -90;
	if(ang > 90)ang = 90;
	ang += 90;
	int grp = ang/10;
	if(grp==18)
		return angles[18];

	float deg_cost = (angles[grp+1]-angles[grp])/10;
	printf("ang: %d; grp: %d; cost: %.3f\n",ang, grp, deg_cost);

	return angles[grp]+(int)(deg_cost*(ang%10));
}


int main()
{
	int ang,duty;
	while(1)
	{
		scanf("%d",&ang);
		duty = servo_get_angle_duty(ang);
		printf("duty: %d\n\n",duty);
	}
	return 0;
}