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

	int nVtx;
	YsVec3 vtx[8];

	int button,lastX,lastY;
	int mouseMode;  // 0:Viewing Rotation 1:Viewing Scroll
	int key;

	void DrawSeparatablePair(int nVtx,YsVec3 vtx[]);
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

void InitYsClass(void)
{
	YsCoordSysModel=YSLEFT_ZPLUS_YPLUS;  // Same as YSBLUEIMPULSE
}

void YsClassSample::MainLoop(void)
{
	int i;
	int lb,mb,rb;
	long mx,my;
	double v[]=
	{
		 0.0, 5.0, 0.0,
		 0.0, 0.0, 0.0,
		 5.0, 0.0, 0.0,
		 5.0,-5.0, 0.0,
		-3.0,-5.0, 0.0,
		-3.0,-3.0, 0.0,
		-5.0,-3.0, 0.0,
		-5.0, 5.0, 0.0
	};

	eyeDistance=10.0;
	eyeLookAt.Set(0.0,0.0,0.0);
	eyeAtt.Set(0.0,0.0,0.0);

	nVtx=8;
	for(i=0; i<nVtx; i++)
	{
		vtx[i].Set(&v[i*3]);
	}

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

void YsClassSample::DrawSeparatablePair(int nVtx,YsVec3 vtx[])
{
	int i,j;
	BIPOINT p1,p2;

	BiSetPoint(&p2,vtx[nVtx-1].x(),vtx[nVtx-1].y(),vtx[nVtx-1].z());
	for(i=0; i<nVtx; i++)
	{
		p1=p2;
		BiSetPoint(&p2,vtx[i].x(),vtx[i].y(),vtx[i].z());
		BiInsLine(&p1,&p2,&BiWhite);
	}

	for(i=0; i<nVtx; i++)
	{
		for(j=i+1; j<nVtx; j++)
		{
			if(YsCheckSeparatability3(nVtx,vtx,i,j)==YSTRUE)
			{
				BiSetPoint(&p1,vtx[i].x(),vtx[i].y(),vtx[i].z());
				BiSetPoint(&p2,vtx[j].x(),vtx[j].y(),vtx[j].z());
				BiInsLine(&p1,&p2,&BiGreen);
			}
		}
	}
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

	DrawSeparatablePair(nVtx,vtx);

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

