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
	int button,lastX,lastY;
	int mouseMode;  // 0:Viewing Rotation 1:Viewing Scroll
	int key;

	YsVec3 pointOfInterest;
	YsVec3 p[4];

	void PrepareSquare(YsVec3 v[]);
	void DrawPolygon(int nv,YsVec3 v[],YsColor &col);
	void DrawAxis(double length);
	void DrawNearestPointInsideOrOutside(void);
	void RotatePolygon(double pitch,double yaw);
	void Display(void);
	void Key(int key);
	void Mouse(int btn,int sta,int x,int y);
	void Motion(int x,int y);

public:
	void MainLoop(void);
};

YsColor red(1.0,0.0,0.0),blu(0.0,0.0,1.0),grn(0.0,1.0,0.0),yel(1.0,1.0,0.0);
YsColor whi(1.0,1.0,1.0);

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];

int main(int argc, char** argv)
{
	InitYsClass();

	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("C....Rotate polygon\n");
	printf("D....Move point of interest\n");
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

	pointOfInterest.Set(0.0,0.0,-1.0);
	PrepareSquare(p);

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
	}

}

void YsClassSample::PrepareSquare(YsVec3 v[])
{
	v[0].Set( 0.0, 5.0,0.0);
	v[1].Set( 5.0, 0.0,0.0);
	v[2].Set( 0.0,-5.0,0.0);
	v[3].Set(-5.0, 0.0,0.0);
}

void YsClassSample::DrawPolygon(int nv,YsVec3 v[],YsColor &col)
{
	int i;
	BIPOINT plg[256];
	double r,g,b;
	BICOLOR c;

	col.GetDoubleRGB(r,g,b);
	BiSetColorRGB(&c,int(r*255.0),int(g*255.0),int(b*255.0));

	for(i=0; i<nv; i++)
	{
		BiSetPoint(&plg[i],v[i].x(),v[i].y(),v[i].z());
	}
	BiInsPolyg(nv,plg,&c);
}

void YsClassSample::DrawNearestPointInsideOrOutside(void)
{
	YsPlane pln;
	YsVec3 nearestPoint;
	YSSIDE sd;
	BIPOINT cursor;

	pln.MakeBestFitPlane(4,p);
	pln.GetNearestPoint(nearestPoint,pointOfInterest);

	sd=YsCheckInsidePolygon3(nearestPoint,4,p);
	BiSetPoint(&cursor,nearestPoint.x(),nearestPoint.y(),nearestPoint.z());
	switch(sd)
	{
	case YSINSIDE:
		BiInsMarker(&cursor,&BiGreen,BIMK_CROSS);
		break;
	case YSOUTSIDE:
		BiInsMarker(&cursor,&BiBlue,BIMK_CROSS);
		break;
	case YSBOUNDARY:
		BiInsMarker(&cursor,&BiYellow,BIMK_CROSS);
		break;
	case YSUNKNOWNSIDE:
		BiInsMarker(&cursor,&BiRed,BIMK_CROSS);
		break;
	}
}

void YsClassSample::RotatePolygon(double pitch,double yaw)
{
	int i;
	YsMatrix4x4 m;

	m.Initialize();
	m.RotateXZ(yaw);
	m.RotateZY(pitch);

	for(i=0; i<4; i++)
	{
		p[i]=m*p[i];
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
	BIPOINT cursor;

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

	BiSetPoint(&cursor,
	           pointOfInterest.x(),
	           pointOfInterest.y(),
	           pointOfInterest.z());
	BiInsMarker(&cursor,&BiWhite,BIMK_CROSS);
	DrawNearestPointInsideOrOutside();
	DrawPolygon(4,p,blu);
	DrawAxis(5.0);

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
	case BIKEY_C:
		printf("C....Rotate polygon mode\n");
		mouseMode=2;
		break;
	case BIKEY_D:
		printf("D....Move point of interest mode\n");
		mouseMode=3;
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
	case 2:
		RotatePolygon(dy/300.0,-dx/300.0);
		break;
	case 3:
		vec.Set(dx/30.0,dy/30.0,0.0);
		vec=eyeAtt.GetMatrix()*vec;
		pointOfInterest=pointOfInterest+vec;
		break;
	}
	lastX=x;
	lastY=y;
}

