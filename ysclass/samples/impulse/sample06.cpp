//cl sample??.cpp /DWIN32 ysclass.lib kernel32.lib user32.lib gdi32.lib advapi32.lib impls32d.lib

#include <impulse.h>

#include <stdio.h>
#include <stdlib.h>

#include <ysclass.h>

void InitYsClass(void);

class YsClassSample
{
protected:
	double eyeDistance;
	YsVec3 eyeLookAt,eyePos;
	YsAtt3 eyeAtt;

	YsVec3 orbiter;
	YsVec3 axisOfOrbit;
	YsRotation rot;

	int button,lastX,lastY;
	int mouseMode;  // 0:Viewing Rotation 1:Viewing Scroll
	int key;

	void DrawAxis(double length);
	void Display(void);
	void Key(int key);
	void Mouse(int btn,int sta,int x,int y);
	void Motion(int x,int y);

public:
	void MainLoop(void);
};

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int argc, char** argv)
{
	InitYsClass();

	printf("Keys\n");
	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("Z....Zoom\n");
	printf("M....Mooz\n");
	printf("----------\n");

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

void DrawCursor(void)
{
	BIPOINT p[]=
	{
		{ 1.0, 0.0, 0.0},
		{ 0.0, 0.0, 1.0},
		{-1.0, 0.0, 0.0},
		{ 0.0, 0.0,-1.0},

		{ 1.0, 0.0, 0.0},
		{ 0.0, 1.0, 0.0},
		{-1.0, 0.0, 0.0},
		{ 0.0,-1.0, 0.0},

		{ 0.0, 1.0, 0.0},
		{ 0.0, 0.0, 1.0},
		{ 0.0,-1.0, 0.0},
		{ 0.0, 0.0,-1.0}
	};

	BiInsPolyg(4,&p[0],&BiBlue);
	BiInsPolyg(4,&p[4],&BiBlue);
	BiInsPolyg(4,&p[8],&BiBlue);
}

void InitYsClass(void)
{
	YsCoordSysModel=YSLEFT_ZPLUS_YPLUS;  // Same as YSBLUEIMPULSE
}

void YsClassSample::MainLoop(void)
{
	int lb,mb,rb;
	long mx,my;

	eyeDistance=10.0;
	eyeLookAt.Set(0.0,0.0,0.0);
	eyeAtt.Set(0.0,0.0,0.0);

	axisOfOrbit.Set(1.0,1.0,1.0);
	axisOfOrbit.Normalize();

	orbiter=axisOfOrbit.GetArbitraryParpendicularVector();
	orbiter.Normalize();

	rot.Set(axisOfOrbit,0.0);

	BiMouse(&lb,&mb,&rb,&mx,&my);

	mouseMode=0;
	while((key=BiInkey())!=BIKEY_ESC)
	{
		BIPROJ prj;

		BiUpdateDevice();
		BiGetStdProjection(&prj);
		BiSetProjection(&prj);

		Key(key);
		Display();

		lastX=mx;
		lastY=my;
		BiMouse(&lb,&mb,&rb,&mx,&my);
		if(lb==BI_ON && (mx!=lastX || my!=lastY))
		{
			Motion(mx,my);
		}

		double a;

		a=rot.GetAngle();
		a+=YsDegToRad(10.0);
		rot.SetAngle(a);
	}

}

void YsClassSample::DrawAxis(double length)
{
	BIPOINT p1,p2;
	BiSetPoint(&p1,0.0,0.0,0.0);

	BiSetPoint(&p2,length,0.0,0.0);
	BiInsLine(&p1,&p2,&BiRed);
	BiInsString(&p2,"X",&BiRed,BIAL_CENTER,BIAL_CENTER);

	BiSetPoint(&p2,0.0,length,0.0);
	BiInsLine(&p1,&p2,&BiGreen);
	BiInsString(&p2,"Y",&BiGreen,BIAL_CENTER,BIAL_CENTER);

	BiSetPoint(&p2,0.0,0.0,length);
	BiInsLine(&p1,&p2,&BiBlue);
	BiInsString(&p2,"Z",&BiBlue,BIAL_CENTER,BIAL_CENTER);
}

////////////////////////////////////////////////////////////

void YsClassSample::Display(void)
{
	BIPOSATT biEyePos;

	eyePos.Set(0.0,0.0,-eyeDistance);
	eyePos=eyeAtt.GetMatrix()*eyePos;
	eyePos=eyeLookAt+eyePos;

	BiSetPoint(&biEyePos.p,eyePos.x(),eyePos.y(),eyePos.z());
	BiSetAngleDeg(&biEyePos.a,
	              YsRadToDeg(eyeAtt.h()),
	              YsRadToDeg(eyeAtt.p()),
	              YsRadToDeg(eyeAtt.b()));

	BiClearScreen();
	BiStartBuffer(&biEyePos);

	DrawAxis(5.0);


	BIPOINT p1,p2;
	BiSetPoint(&p1,0,0,0);
	BiSetPoint(&p2,axisOfOrbit.x()*5.0,axisOfOrbit.y()*5.0,axisOfOrbit.z()*5.0);
	BiInsLine(&p1,&p2,&BiWhite);


	YsVec3 p;
	BIPOSATT pa;
	rot.RotatePositive(p,orbiter);
	BiSetPoint(&pa.p,p.x()*5.0,p.y()*5.0,p.z()*5.0);
	BiSetAngle(&pa.a,0,0,0);
	BiPushMatrix(&pa);
	DrawCursor();
	BiPopMatrix();


	BiFlushBuffer();
	BiSwapBuffers();
}

void YsClassSample::Key(int key)
{
	switch(key)
	{
	case BIKEY_A:
		printf("A....Viewing Rotation Mode\n");
		mouseMode=0;
		break;
	case BIKEY_B:
		printf("B....Viewing Translation (Scroll) Mode\n");
		mouseMode=1;
		break;
	case BIKEY_M:
		if(eyeDistance<40.0)
		{
			eyeDistance=eyeDistance+2.0;
		}
		break;
	case BIKEY_Z:
		if(eyeDistance>2.0)
		{
			eyeDistance=eyeDistance-2.0;
		}
		break;
	}
}

void YsClassSample::Motion(int x,int y)
{
	double dx,dy;
	YsVec3 vec;

	dx=double(x-lastX);
	dy=double(lastY-y);
	switch(mouseMode)
	{
	case 0:
		eyeAtt.NoseUp(dy/300.0);
		eyeAtt.YawLeft(-dx/300.0);
		break;
	case 1:
		vec.Set(-dx/30.0,-dy/30.0,0.0);
		vec=eyeAtt.GetMatrix()*vec;
		eyeLookAt=eyeLookAt+vec;
		break;
	}
	lastX=x;
	lastY=y;
}

