#include <stdio.h>
#include <ysclass.h>

void ShowMatrix(YsMatrix &m,char caption[]);

int main(void)
{
	YsMatrix a,b,c;
	double matTemplate[9]=
	{
		1.0,2.0,3.0,
		4.0,6.0,5.0,
		7.0,8.0,9.0
	};

	a.Create(3,3,matTemplate);

	b=a;
	b.Invert();

	c=a*b;

	ShowMatrix(a,"Matrix A");
	ShowMatrix(b,"Matrix B is inverse of Matrix A");
	ShowMatrix(c,"Matrix A x Matrix B must be an identity matrix");

	return 0;
}

void ShowMatrix(YsMatrix &m,char caption[])
{
	int r,c;
	double v;

	printf("[%s]\n",caption);

	for(r=1; r<=3; r++)
	{
		for(c=1; c<=3; c++)
		{
			v=m.v(r,c);
			printf(" %+.4lf",v);
		}
		printf("\n");
	}
}

