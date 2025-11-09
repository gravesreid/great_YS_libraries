//cl sample??.cpp /DWIN32 ysclass.lib kernel32.lib user32.lib gdi32.lib advapi32.lib impls32d.lib

#include <impulse.h>

#include <stdio.h>
#include <stdlib.h>

#include <ysclass.h>

void InitYsClass(void);

class YsClassSample
{
protected:
	YsVec2 pointOfInterest;
	YsVec2 p[6];
	int lastX,lastY;

	void PreparePolygon(YsVec2 p[]);
	void Display(void);
	void Motion(int mx,int my);
public:
	void MainLoop(void);
};

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int argc, char** argv)
{
	BIPROJ prj;

	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("YsClass Sample Application");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	YsClassSample sam;
	sam.MainLoop();

	BiCloseWindow();

	return 0;
}

void YsClassSample::MainLoop(void)
{
	int key;
	int lb,mb,rb;
	long mx,my;

	pointOfInterest.Set(0.0,0.0);
	PreparePolygon(p);

	BiUpdateDevice();
	BiMouse(&lb,&mb,&rb,&mx,&my);

	while((key=BiInkey())!=BIKEY_ESC)
	{
		BIPROJ prj;

		BiUpdateDevice();
		BiGetStdProjection(&prj);
		BiSetProjection(&prj);
		Display();

		lastX=mx;
		lastY=my;
		BiMouse(&lb,&mb,&rb,&mx,&my);
		if(lb==BI_ON && (lastX!=mx || lastY!=my))
		{
			Motion(mx,my);
		}
	}

}

void YsClassSample::PreparePolygon(YsVec2 v[])
{
	v[0].Set( 0.0, 5.0);
	v[1].Set( 5.0, 5.0);
	v[2].Set( 5.0, 0.0);
	v[3].Set( 0.0,-5.0);
	v[4].Set(-5.0,-5.0);
	v[5].Set(-5.0, 0.0);
}

////////////////////////////////////////////////////////////

void YsClassSample::Display(void)
{
	long mx,my;
	int lb,mb,rb;

	int i;
	BIPOINT plg[256],poi;

	BiMouse(&lb,&mb,&rb,&mx,&my);

	BiClearScreen();
	BiStartBuffer(&BiOrgPA);

	for(i=0; i<6; i++)
	{
		BiSetPoint(&plg[i],p[i].x(),p[i].y(),10.0);
	}
	BiOvwPolyg(6,plg,&BiWhite);


	BiSetPoint(&poi,pointOfInterest.x(),pointOfInterest.y(),10.0);
	switch(YsCheckInsidePolygon2(pointOfInterest,6,p))
	{
	case YSINSIDE:
		BiOvwMarker(&poi,&BiGreen,BIMK_CROSS);
		break;
	case YSOUTSIDE:
		BiOvwMarker(&poi,&BiBlue,BIMK_CROSS);
		break;
	case YSBOUNDARY:
		BiOvwMarker(&poi,&BiYellow,BIMK_CROSS);
		break;
	case YSUNKNOWNSIDE:
		BiOvwMarker(&poi,&BiRed,BIMK_CROSS);
		break;
	}

	BiFlushBuffer();
	BiSwapBuffers();
}

void YsClassSample::Motion(int x,int y)
{
	double dx,dy;
	YsVec2 vec;

	dx=double(x-lastX);
	dy=double(lastY-y);

	vec.Set(dx/30.0,dy/30.0);
	pointOfInterest=pointOfInterest+vec;
}

