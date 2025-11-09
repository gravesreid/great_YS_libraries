// cl ldg.cpp ysclass.lib kernel32.lib user32.lib gdi32.lib impls32o.lib opengl32.lib glu32.lib

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include <ysclass.h>

#include <impulse.h>

#define BIWORKSIZE 0x60000L
char BiWork[BIWORKSIZE];


YSBOOL inPolygon=YSFALSE;
YSBOOL collisionDetection=YSTRUE;


void DrawShell(YsShell &shell,YsColor &col,YSBOOL inPolygon)
{
	int i,j;
	int nVtx,nPlg;
	YsVec3 vtx[256];

	nPlg=shell.GetNumPolygon();
	for(i=0; i<nPlg; i++)
	{
		nVtx=shell.GetNumVertexOfPolygon(i);
		if(nVtx>0)
		{
			shell.GetVertexListOfPolygon(vtx,256,i);
			BIPOINT pnt[256];
			int r,g,b;
			BICOLOR c;
			col.GetIntRGB(r,g,b);
			BiSetColorRGB(&c,r,g,b);
			switch(inPolygon)
			{
			case YSFALSE:
				for(j=0; j<nVtx; j++)
				{
					BIPOINT p1,p2;
					BiSetPoint(&p1,vtx[j].x(),vtx[j].y(),vtx[j].z());
					BiSetPoint(&p2,vtx[(j+1)%nVtx].x(),
					               vtx[(j+1)%nVtx].y(),
					               vtx[(j+1)%nVtx].z());
					BiInsLine(&p1,&p2,&c);
				}
				break;
			case YSTRUE:
				for(j=0; j<nVtx; j++)
				{
					BiSetPoint(&pnt[j],vtx[j].x(),vtx[j].y(),vtx[j].z());
				}
				BiInsPolyg(nVtx,pnt,&c);
				break;
			}
		}
	}
}

void DrawCollidedPolygon(YsShell &shl)
{
	int i,j;
	int nVtx,nPlg;
	YsVec3 vtx[256];
	YsShellPolygon *plg;

	nPlg=shl.GetNumPolygon();
	for(i=0; i<nPlg; i++)
	{
		plg=shl.GetPolygon(i);
		if(plg->freeAttribute3!=0)
		{
			nVtx=shl.GetNumVertexOfPolygon(i);
			if(nVtx>0)
			{
				shl.GetVertexListOfPolygon(vtx,256,i);
				BIPOINT pnt[256];
				for(j=0; j<nVtx; j++)
				{
					BiSetPoint(&pnt[j],vtx[j].x(),vtx[j].y(),vtx[j].z());
				}
				BiInsPolyg(nVtx,pnt,&BiRed);
			}
		}
	}
}

void GearCollision
 (YsShell &fuselage,YsShell &gear1,YsShell &gear2,YsShell &lid1,YsShell &lid2)
{
	printf("UP...........Shorten Gear\n");
	printf("DOWN.........Extend Gear\n");
	printf("SHIFT+DOWN...Gear Down\n");
	printf("SHIFT+UP.....Gear Up\n");
	printf("CTRL+DOWN....Lid Open\n");
	printf("CTRL+UP......Lid Close\n");
	printf("C............Collision Detection On/Off\n");
	printf("P............Polygon/Wireframe\n");
	printf("A............Find Interfering Volume.\n");

	int i,j,key;
	long mx,my,pmx,pmy;
	int lb,mb,rb;
	double distance;
	double gearExt;
	YsMatrix4x4 view,tfm1,tfm2;
	YsVec3 eyePos,gear1pos,gear2pos,lid1pos,lid2pos;
	YsAtt3 eyeAtt,gear1att,gear2att,lid1att,lid2att;
	YsColor blue,red,green,yellow;
	YsList <YsShell> *collided,*collPtr;

	blue.Set(0,0,200);
	red.Set(200,0,0);
	green.Set(0,200,0);
	yellow.Set(200,200,0);

	YsShell *shlLst[5];
	shlLst[0]=&fuselage;
	shlLst[1]=&gear1;
	shlLst[2]=&gear2;
	shlLst[3]=&lid1;
	shlLst[4]=&lid2;

	eyeAtt.Set(YsPi*3.0/4.0,-YsPi/10.0,0.0);

	gear1pos.Set(0.0,2.4,-6.5); // Z=-7.0 for no collision
	gear2pos.Set(0.0,0.0,1.1);
	gear1att.Set(0.0,0.0,0.0);
	gearExt=1.1;

	lid1pos.Set( 2.35,-0.1,0.0);
	lid2pos.Set(-2.35,-0.1,0.0);

	lid1att.Set(0.0,0.0,0.0);
	lid2att.Set(0.0,0.0,0.0);

	collided=NULL;

	distance=20.0;
	while((key=BiInkey())!=BIKEY_ESC && BiFatal()==BI_FALSE)
	{
		while(key!=BIKEY_NULL && BiInkey()!=BIKEY_NULL)
		{
			// Just clear key buffer
		}

		switch(key)
		{
		case BIKEY_P:
			inPolygon=(inPolygon==YSTRUE ? YSFALSE : YSTRUE);
			break;
		case BIKEY_C:
			collisionDetection=(collisionDetection==YSTRUE ? YSFALSE : YSTRUE);
			printf("Collision : %s\n",
			    (collisionDetection==YSTRUE ? "ON" : "OFF"));
			break;
		}

		if(key==BIKEY_A)
		{
			YsList <YsShell> *newShell;
			printf("Boolean Operation in progress...\n");
			collided->DeleteList();
			collided=NULL;
			for(i=0; i<5; i++)
			{
				for(j=i+1; j<5; j++)
				{
					if(YsCheckShellCollision(*shlLst[i],*shlLst[j])==YSTRUE)
					{
						printf("%d vs %d\n",i,j);
						newShell=new YsList <YsShell>;
						YsBlendShell
						  (newShell->dat,*shlLst[i],*shlLst[j],YSBOOLAND);
						collided=collided->Append(newShell);
					}
				}
			}
			printf("Done\n");
		}

		BiUpdateDevice();
		BIPROJ prj;
		BiGetStdProjection(&prj);
		BiSetProjection(&prj);
		BiSetShadMode(BI_ON);

		eyePos.Set(0.0,0.0,-distance);
		eyePos=eyeAtt.GetMatrix()*eyePos;

		BIPOSATT eye;
		BiSetAngleDeg(
		    &eye.a,
		    YsRadToDeg(eyeAtt.h()),
		    YsRadToDeg(eyeAtt.p()),
		    YsRadToDeg(eyeAtt.b()));
		BiSetPoint(&eye.p,eyePos.x(),eyePos.y(),eyePos.z());

		BiClearScreen();
		BiStartBuffer(&eye);

		tfm1.Initialize();
		tfm1.Translate(gear1pos);
		tfm1=tfm1*gear1att.GetMatrix();

		tfm2.Initialize();
		gear2pos.Set(0.0,0.0,gearExt);
		tfm2.Translate(gear2pos);
		tfm2=tfm1*tfm2*gear2att.GetMatrix();

		gear1.SetMatrix(tfm1);
		gear2.SetMatrix(tfm2);

		tfm1.Initialize();
		tfm1.Translate(lid1pos);
		tfm1=tfm1*lid1att.GetMatrix();
		lid1.SetMatrix(tfm1);

		tfm1.Initialize();
		tfm1.Translate(lid2pos);
		tfm1=tfm1*lid2att.GetMatrix();
		lid2.SetMatrix(tfm1);

		DrawShell(fuselage,blue,inPolygon);
		DrawShell(gear1,yellow,inPolygon);
		DrawShell(gear2,yellow,inPolygon);
		DrawShell(lid1,green,inPolygon);
		DrawShell(lid2,green,inPolygon);

		for(collPtr=collided; collPtr!=NULL; collPtr=collPtr->Next())
		{
			DrawShell(collPtr->dat,red,YSTRUE);
		}

		if(collisionDetection==YSTRUE)
		{
			for(i=0; i<5; i++)
			{
				for(j=i+1; j<5; j++)
				{
					if(YsCheckShellCollision(*shlLst[i],*shlLst[j])==YSTRUE)
					{
						DrawCollidedPolygon(*shlLst[i]);
						DrawCollidedPolygon(*shlLst[j]);
					}
				}
			}
		}

		BiFlushBuffer();
		BiSwapBuffers();

		pmx=mx;
		pmy=my;
		BiMouse(&lb,&mb,&rb,&mx,&my);
		if(lb==BI_ON)
		{
			double dx,dy;
			dx=double(mx-pmx)*YsPi/1000.0;
			dy=double(my-pmy)*YsPi/1000.0;
			eyeAtt.NoseUp(-dy);
			eyeAtt.YawLeft(-dx);
		}

		if(BiKey(BIKEY_SHIFT)==BI_ON)
		{
			if(BiKey(BIKEY_UP)==BI_ON)
			{
				double p;
				p=gear1att.p();
				p=YsSmaller(p+YsDegToRad(3.0),0.0);
				gear1att.SetP(p);
			}
			if(BiKey(BIKEY_DOWN)==BI_ON)
			{
				double p;
				p=gear1att.p();
				p=YsGreater(p-YsDegToRad(3.0),-YsPi/2.0);
				gear1att.SetP(p);
			}
		}
		else if(BiKey(BIKEY_CTRL)==BI_ON)
		{
			if(BiKey(BIKEY_DOWN)==BI_ON)
			{
				double b;
				b=lid1att.b();
				b=YsSmaller(b+YsDegToRad(3.0),YsPi/2.0);
				lid1att.SetB( b);
				lid2att.SetB(-b);
			}
			else if(BiKey(BIKEY_UP)==BI_ON)
			{
				double b;
				b=lid1att.b();
				b=YsGreater(b-YsDegToRad(3.0),0.0);
				lid1att.SetB( b);
				lid2att.SetB(-b);
			}
		}
		else
		{
			if(BiKey(BIKEY_DOWN)==BI_ON)
			{
				gearExt+=0.1;
			}
			if(BiKey(BIKEY_UP)==BI_ON)
			{
				gearExt-=0.1;
			}
		}
	}
	collided->DeleteList();
}

YSRESULT LoadTri(YsShell &sh,char fn[])
{
	int nTri,i,j,vtId[3];
	YsVec3 vec;
	FILE *fp;
	double x,y,z;

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		fscanf(fp,"%d",&nTri);
		for(i=0; i<nTri; i++)
		{
			for(j=0; j<3; j++)
			{
				fscanf(fp,"%lf%lf%lf",&x,&y,&z);
				vec.Set(x,y,z);
				vtId[j]=sh.AddVertex(vec);
			}
			sh.AddPolygon(3,vtId);
		}
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

void SubMain(void)
{
	YsShell fuselage,gear1,gear2,lid1,lid2;
	LoadTri(fuselage,"fuselage.tri");
	LoadTri(gear1,"gear1.tri");
	LoadTri(gear2,"gear2.tri");
	LoadTri(lid1,"lid1.tri");
	LoadTri(lid2,"lid2.tri");
	GearCollision(fuselage,gear1,gear2,lid1,lid2);
}

int main(int ac,char *av[])
{
	BIPROJ prj;
	BiMemoryInitialize();
	BiCalcInitialize();
	BiGraphInitialize(BiWork,BIWORKSIZE);
	BiDeviceInitialize();
	BiRenderInitialize();

	BiSetWindowName("Landing Gear Problem");
	BiOpenWindow(640,480);

	BiGetStdProjection(&prj);
	BiSetProjection(&prj);
	BiSetShadMode(BI_ON);

	SubMain();

	YsBaseClass::ClassInfo->ShowHierarchy(0);

	BiCloseWindow();
	return 0;
}
