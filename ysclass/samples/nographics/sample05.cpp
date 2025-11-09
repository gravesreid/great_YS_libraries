#include <stdio.h>
#include <ysclass.h>

char vScreen[24][75];

void ClearVirtualScreen(void);
void DrawPolygon(int np,int p[],char dot);
void ExposeVirtualScreen(void);

YsScreenPolygon <16,24> plgDriver;

int main(void)
{
	int plg0[]=
	{
		2,2,
		70,2,
		70,21,
		2,21
	};
	int plg1[]=
	{
		10,3,
		60,5,
		30,20
	};
	int plg2[]=
	{
		5,10,
		10,18,
		20,15,
		60,19,
		50,5,
		40,10,
		20,5
	};

	ClearVirtualScreen();
	DrawPolygon(4,plg0,'+');
	DrawPolygon(3,plg1,'*');
	DrawPolygon(7,plg2,'-');
	ExposeVirtualScreen();
	return 0;
}

void ClearVirtualScreen(void)
{
	int i,j;
	for(i=0; i<24; i++)
	{
		for(j=0; j<75; j++)
		{
			vScreen[i][j]='.';
		}
		vScreen[i][74]=0;
	}
}

void DrawPolygon(int np,int p[],char dot)
{
	int i,n,*buf,x,y;
	int miny,maxy;

	plgDriver.MakeHorizontalIntersectionBuffer(np,p);
	plgDriver.GetRangeOfY(miny,maxy);
	for(y=miny; y<=maxy; y++)
	{
		n=plgDriver.GetNumIntersectionAtY(y);
		buf=plgDriver.GetIntersectionBufferAtY(y);
		for(i=0; i<n; i+=2)
		{
			for(x=buf[i]; x<=buf[i+1]; x++)
			{
				vScreen[y][x]=dot;
			}
		}
	}
}

void ExposeVirtualScreen(void)
{
	int i;
	for(i=0; i<24; i++)
	{
		printf("%s\n",vScreen[i]);
	}
}
