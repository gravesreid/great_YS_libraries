#include <stdio.h>
#include <ysclass.h>

// For more information of YsMatrix4x4, please refer to the
// online help.

// You may also find interesting features in YsAtt3, YsRotation

void ShowMatrix(YsMatrix4x4 &m,char caption[]);

int main(void)
{
	YsMatrix4x4 a,b,c;

	a.Initialize();
	a.RotateXZ(YsDegToRad(30.0));
	a.RotateZY(YsDegToRad(45.0));
	a.RotateXY(YsDegToRad(60.0));

	b=a;
	b.Invert();

	c=a*b;

	ShowMatrix(a,"Matrix A");
	ShowMatrix(b,"Matrix B is inverse of Matrix A");
	ShowMatrix(c,"Matrix A x Matrix B must be an identity matrix");

	return 0;
}

void ShowMatrix(YsMatrix4x4 &m,char caption[])
{
	int r,c;
	double v;

	printf("[%s]\n",caption);

	for(r=1; r<=4; r++)
	{
		for(c=1; c<=4; c++)
		{
			v=m.v(r,c);
			printf(" %+.4lf",v);
		}
		printf("\n");
	}
}

