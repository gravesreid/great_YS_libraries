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
	YSBOOL inPolygon;

	YsShell sh1,sh2;

	void PrepareShell(YsShell &sh);
	void DrawShell(YsShell &sh,YsColor &col,YSBOOL inPolygon);
	void DrawPolygon(int nv,YsVec3 v[],YsColor &col,YSBOOL inPolygon);
	void DrawAxis(double length);
	void RotatePolygon(YsVec3 p[],double pitch,double yaw);
	void Display(void);
	void Key(int key);
	void Mouse(int btn,int sta,int x,int y);
	void MoveShell(YsShell &sh,YsVec3 &mov);
	void RotateShell(YsShell &sh,double pitch,double yaw);
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

	printf("Keys\n");
	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("C....Rotate Shell 1\n");
	printf("D....Move Shell 1\n");
	printf("E....Rotate Shell 2\n");
	printf("F....Move Shell 2\n");
	printf("O....Or(UNION)\n");
	printf("I....And(INTERSECTION)\n");
	printf("S....Minus(DIFFERENCE)\n");
	printf("P....InPolygon/InWireframe\n");
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

	PrepareShell(sh1);
	PrepareShell(sh2);

	BiMouse(&lb,&mb,&rb,&mx,&my);

	inPolygon=YSTRUE;

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

void YsClassSample::PrepareShell(YsShell &shl)
{
	int idx[8];
	idx[0]=shl.AddVertex(YsVec3(-2.5, 1.0, 1.0));
	idx[1]=shl.AddVertex(YsVec3( 2.5, 1.0, 1.0));
	idx[2]=shl.AddVertex(YsVec3(-2.5, 1.0,-1.0));
	idx[3]=shl.AddVertex(YsVec3( 2.5, 1.0,-1.0));
	idx[4]=shl.AddVertex(YsVec3(-2.5,-1.0,-1.0));
	idx[5]=shl.AddVertex(YsVec3( 2.5,-1.0,-1.0));
	idx[6]=shl.AddVertex(YsVec3(-2.5,-1.0, 1.0));
	idx[7]=shl.AddVertex(YsVec3( 2.5,-1.0, 1.0));

	int plg[4];
	plg[0]=idx[0];
	plg[1]=idx[1];
	plg[2]=idx[3];
	plg[3]=idx[2];
	shl.AddPolygon(4,plg);

	plg[0]=idx[2];
	plg[1]=idx[3];
	plg[2]=idx[5];
	plg[3]=idx[4];
	shl.AddPolygon(4,plg);

	plg[0]=idx[4];
	plg[1]=idx[5];
	plg[2]=idx[7];
	plg[3]=idx[6];
	shl.AddPolygon(4,plg);

	plg[0]=idx[6];
	plg[1]=idx[7];
	plg[2]=idx[1];
	plg[3]=idx[0];
	shl.AddPolygon(4,plg);

	plg[0]=idx[0];
	plg[1]=idx[2];
	plg[2]=idx[4];
	plg[3]=idx[6];
	shl.AddPolygon(4,plg);

	plg[0]=idx[7];
	plg[1]=idx[5];
	plg[2]=idx[3];
	plg[3]=idx[1];
	shl.AddPolygon(4,plg);
}

void YsClassSample::DrawShell(YsShell &shell,YsColor &col,YSBOOL inPolygon)
{
	int i;
	int nVtx,nPlg;
	YsVec3 vtx[256];

	nPlg=shell.GetNumPolygon();
	for(i=0; i<nPlg; i++)
	{
		nVtx=shell.GetNumVertexOfPolygon(i);
		if(nVtx>0)
		{
			shell.GetVertexListOfPolygon(vtx,256,i);
			DrawPolygon(nVtx,vtx,col,inPolygon);
		}
	}
}

void YsClassSample::DrawPolygon(int nv,YsVec3 v[],YsColor &col,YSBOOL inPolygon)
{
	int i;
	BIPOINT plg[256],p1,p2;
	double r,g,b;
	BICOLOR c;

	col.GetDoubleRGB(r,g,b);
	BiSetColorRGB(&c,int(r*255.0),int(g*255.0),int(b*255.0));

	switch(inPolygon)
	{
	case YSTRUE:
		for(i=0; i<nv; i++)
		{
			BiSetPoint(&plg[i],v[i].x(),v[i].y(),v[i].z());
		}
		BiInsPolyg(nv,plg,&c);
		break;
	case YSFALSE:
		for(i=0; i<nv; i++)
		{
			BiSetPoint(&p1,v[i].x(),v[i].y(),v[i].z());
			BiSetPoint(&p2,v[(i+1)%nv].x(),v[(i+1)%nv].y(),v[(i+1)%nv].z());
			BiInsLine(&p1,&p2,&c);
		}
		break;
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

	DrawShell(sh1,blu,inPolygon);
	DrawShell(sh2,grn,YSFALSE);
	DrawAxis(5.0);

	BiFlushBuffer();
	BiSwapBuffers();
}

void YsClassSample::Key(int key)
{
	YsShell tmp;
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
		printf("C....Rotate shell 1 mode\n");
		mouseMode=2;
		break;
	case BIKEY_D:
		printf("D....Move shell 1 mode\n");
		mouseMode=3;
		break;
	case BIKEY_E:
		printf("E....Rotate shell 2 mode\n");
		mouseMode=4;
		break;
	case BIKEY_F:
		printf("F....Move shell 2 mode\n");
		mouseMode=5;
		break;
	case BIKEY_O:
		YsBlendShell(tmp,sh1,sh2,YSBOOLOR);
		sh1=tmp;
		break;
	case BIKEY_I:
		YsBlendShell(tmp,sh1,sh2,YSBOOLAND);
		sh1=tmp;
		break;
	case BIKEY_S:
		YsBlendShell(tmp,sh1,sh2,YSBOOLMINUS);
		sh1=tmp;
		break;
	case BIKEY_P:
		inPolygon=(inPolygon==YSTRUE ? YSFALSE : YSTRUE);
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

void YsClassSample::RotateShell(YsShell &sh,double pitch,double yaw)
{
	YsMatrix4x4 mat,rot,viw,iViw;

	sh.GetMatrix(mat);

	viw=eyeAtt.GetMatrix();
	iViw=viw;
	iViw.Invert();

	rot.Initialize();
	rot.RotateZY(pitch);
	rot.RotateXZ(yaw);

	mat=viw*rot*iViw*mat;

	sh.SetMatrix(mat);
}

void YsClassSample::MoveShell(YsShell &sh,YsVec3 &mov)
{
	YsMatrix4x4 mat,trn,viw,iViw;

	sh.GetMatrix(mat);

	viw=eyeAtt.GetMatrix();
	iViw=viw;
	iViw.Invert();

	trn.Initialize();
	trn.Translate(mov);

	mat=viw*trn*iViw*mat;

	sh.SetMatrix(mat);
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
		RotateShell(sh1,-dy/300.0,dx/300.0);
		break;
	case 3:
		vec.Set(dx/30.0,dy/30.0,0.0);
		//vec=eyeAtt.GetMatrix()*vec;
		MoveShell(sh1,vec);
		break;
	case 4:
		RotateShell(sh2,-dy/300.0,dx/300.0);
		break;
	case 5:
		vec.Set(dx/30.0,dy/30.0,0.0);
		//vec=eyeAtt.GetMatrix()*vec;
		MoveShell(sh2,vec);
		break;
	}
	lastX=x;
	lastY=y;
}

