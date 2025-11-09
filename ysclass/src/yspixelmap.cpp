#include "ysclass.h"

// Implementation //////////////////////////////////////////
void YsDrawLineByDDA::Set(int ix1,int iy1,int ix2,int iy2)
{
	x1=ix1;
	y1=iy1;
	x2=ix2;
	y2=iy2;

	dx=YsAbs(x2-x1);
	dy=YsAbs(y2-y1);
	vx=(x2-x1!=0 ? (x2-x1)/dx : 0);
	vy=(y2-y1!=0 ? (y2-y1)/dy : 0);

	x=x1;
	y=y1;
	k=0;
}

YSRESULT YsDrawLineByDDA::MoveOneStep(void)
{
	// Based on dx*y-dy*x=0

	if(x==x2 && y==y2)
	{
		return YSERR;
	}

	if(dx==0 || dy==0)
	{
		x+=vx;
		y+=vy;
	}
	else
	{
		if(k==0)
		{
			x+=vx;
			k-=dy;
			y+=vy;
			k+=dx;
		}
		else if(k>0)
		{
			x+=vx;
			k-=dy;
		}
		else //if(k<0)
		{
			y+=vy;
			k+=dx;
		}
	}

	return YSOK;
}

void YsDrawLineByDDA::GetPosition(int &ix,int &iy)
{
	ix=x;
	iy=y;
}

YSBOOL YsDrawLineByDDA::ReachedToTheEnd(void)
{
	if(x==x2 && y==y2)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

