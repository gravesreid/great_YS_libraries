#include "ysclass.h"

// Implementation //////////////////////////////////////////
YsColor::YsColor(double rr,double gg,double bb)
{
	col[0]=(unsigned char)(YsBound(unsigned(rr*255.0),0,255));
	col[1]=(unsigned char)(YsBound(unsigned(gg*255.0),0,255));
	col[2]=(unsigned char)(YsBound(unsigned(bb*255.0),0,255));
}

void YsColor::Set15BitRGB(int c15bit)
{
	unsigned c;
	c=c15bit & 32767;
	col[1]=(unsigned char)(((c>>10)&31)*255/31);
	col[0]=(unsigned char)(((c>> 5)&31)*255/31);
	col[2]=(unsigned char)(((c    )&31)*255/31);
}

void YsColor::SetIntRGB(int rr,int gg,int bb)
{
	col[0]=(unsigned char)(YsBound(rr,0,255));
	col[1]=(unsigned char)(YsBound(gg,0,255));
	col[2]=(unsigned char)(YsBound(bb,0,255));
}

void YsColor::SetDoubleRGB(double rr,double gg,double bb)
{
	col[0]=(unsigned char)(YsBound(unsigned(rr*255.0),0,255));
	col[1]=(unsigned char)(YsBound(unsigned(gg*255.0),0,255));
	col[2]=(unsigned char)(YsBound(unsigned(bb*255.0),0,255));
}

void YsColor::SetRainbowColor(const double &t)
{
	if(t<0.0)
	{
		SetIntRGB(0,0,255);
	}
	else if(t<0.25)
	{
		double inten;
		inten=t/0.25;
		SetDoubleRGB(0.0,inten,1.0);
	}
	else if(t<0.5)
	{
		double inten;
		inten=(t-0.25)/0.25;
		SetDoubleRGB(0.0,1.0,1.0-inten);
	}
	else if(t<0.75)
	{
		double inten;
		inten=(t-0.5)/0.25;
		SetDoubleRGB(inten,1.0,0.0);
	}
	else if(t<1.0)
	{
		double inten;
		inten=(t-0.75)/0.25;
		SetDoubleRGB(1.0,1.0-inten,0.0);
	}
	else
	{
		SetIntRGB(255,0,0);
	}
}

int YsColor::Get15BitColor(void) const
{
	int g,r,b;
	r=int(col[0]);
	g=int(col[1]);
	b=int(col[2]);

	r=YsBound((r+4)/8,0,31);
	g=YsBound((g+4)/8,0,31);
	b=YsBound((b+4)/8,0,31);

	return ((g<<10)+(r<<5)+b);
}

void YsColor::GetIntRGB(int &rr,int &gg,int &bb) const
{
	rr=int(col[0]);
	gg=int(col[1]);
	bb=int(col[2]);
}

void YsColor::GetDoubleRGB(double &rr,double &gg,double &bb) const
{
	rr=double(col[0])/255.0;
	gg=double(col[1])/255.0;
	bb=double(col[2])/255.0;
}

