#include <stdio.h>
#include <ysclass.h>

char vScreen[24][75];

void ClearVirtualScreen(void);
void DrawLine(int x1,int y1,int x2,int y2,char dot);
void ExposeVirtualScreen(void);

YsDrawLineByDDA ddaDriver;

int main(void)
{
	ClearVirtualScreen();
	DrawLine(3,5,60,20,'A');
	DrawLine(15,3,5,20,'B');
	DrawLine(0,23,50,0,'C');
	DrawLine(60,5,70,15,'D');
	DrawLine(70,5,60,15,'E');
	DrawLine(60,5,70,5,'F');
	DrawLine(70,15,60,15,'G');
	DrawLine(60,5,60,15,'H');
	DrawLine(70,15,70,5,'I');
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

void DrawLine(int x1,int y1,int x2,int y2,char dot)
{
	ddaDriver.Set(x1,y1,x2,y2);
	vScreen[y1][x1]=dot;
	while(ddaDriver.ReachedToTheEnd()!=YSTRUE)
	{
		int x,y;
		ddaDriver.MoveOneStep();
		ddaDriver.GetPosition(x,y);
		vScreen[y][x]=dot;
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
