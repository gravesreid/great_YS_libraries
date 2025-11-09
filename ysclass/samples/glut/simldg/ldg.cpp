//cl /DWIN32 ldg2.cpp ysclass.lib kernel32.lib user32.lib gdi32.lib advapi32.lib opengl32.lib glu32.lib glut32.lib

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <stdlib.h>

#include <ysclass.h>

YSBOOL inPolygon=YSFALSE;
YSBOOL collisionDetection=YSTRUE;

////////////////////////////////////////////////////////////

double gearDoorAngle[]=
{
	0.0,0.0,
	10.0,90.0,
	20.0,90.0,
	30.0,0.0,
	-1.0,-1.0
};

double gearAngle[]=
{
	0.0,0.0,
	25.0,90.0,
	40.0,90.0,
	-1.0,-1.0
};

double gearExtension[]=
{
	0.0,0.0,
	20.0,2.0,
	25.0,1.5,
	30.0,2.5,
	35.0,1.5,
	40.0,2.5,
	-1.0,-1.0
};

double GetFunctionValue(double func[],double t)
{
	int i;
	for(i=0; func[(i+1)*2]>=0.0; i++)
	{
		if(func[i*2]<=t && t<func[i*2+2])
		{
			double dx,dy;
			dx=func[i*2+2]-func[i*2];
			dy=func[i*2+3]-func[i*2+1];
			return func[i*2+1]+(t-func[i*2])*dy/dx;
		}
	}
	return func[i*2+1];
};



double eyeDistance=20.0;
YsVec3 eyeLookAt;
YsAtt3 eyeAtt;

YsVec3 gear1pos,gear2pos,lid1pos,lid2pos;
YsAtt3 gear1att,gear2att,lid1att,lid2att;

YsShell fuselage,gear1,gear2,lid1,lid2;
double gearExt;
YsList <YsShell> *collided=NULL,*collPtr=NULL;
YsShell *shlLst[5];

YsColor red,blue,green,yellow;

int button,lastX,lastY;

int target;  // 0:Viewing Rotation 1:Viewing Scroll 2:Door 3:Gear
             // 4:Extension

YSBOOL pause=YSFALSE;
int viewMode=0;
double time=0.0;

void RotateInViewingCoordinate
    (YsAtt3 &target,YsAtt3 &view,double lat,double lng)
{
	YsVec3 ev,uv;
	YsMatrix4x4 toview,rotation;

	ev=target.GetForwardVector();
	uv=target.GetUpVector();

	toview=view.GetMatrix();

	rotation.RotateXZ(lat);
	rotation.RotateZY(lng);

	toview.MulInverse(ev,ev,0.0);
	toview.MulInverse(uv,uv,0.0);
	ev=rotation*ev;
	uv=rotation*uv;
	toview.Mul(ev,ev,0.0);
	toview.Mul(uv,uv,0.0);

	target.SetTwoVector(ev,uv);
}

void DrawShell(YsShell &shell,YsColor &col,YSBOOL inPolygon)
{
	int i,j;
	int nVtx,nPlg;
	YsVec3 vtx[256];

	if(inPolygon==YSTRUE)
	{
		glEnable(GL_LIGHTING);
	}
	else
	{
		glDisable(GL_LIGHTING);
	}

	nPlg=shell.GetNumPolygon();
	for(i=0; i<nPlg; i++)
	{
		nVtx=shell.GetNumVertexOfPolygon(i);
		if(nVtx>0)
		{
			double r,g,b;
			YsVec3 nom;

			shell.GetVertexListOfPolygon(vtx,256,i);
			nom=(vtx[1]-vtx[0])^(vtx[2]-vtx[1]);
			nom.Normalize();

			col.GetDoubleRGB(r,g,b);

			glColor3d(r,g,b);

			switch(inPolygon)
			{
			case YSFALSE:
				glBegin(GL_LINE_LOOP);
				break;
			case YSTRUE:
				glBegin(GL_POLYGON);
				break;
			}
			glNormal3d(nom.x(),nom.y(),nom.z());
			for(j=0; j<nVtx; j++)
			{
				glVertex3d(vtx[j].x(),vtx[j].y(),vtx[j].z());
			}
			glEnd();
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
				YsVec3 nom;

				shl.GetVertexListOfPolygon(vtx,256,i);
				nom=(vtx[1]-vtx[0])^(vtx[2]-vtx[1]);
				nom.Normalize();

				glColor3d(1.0,0.0,0.0);
				glNormal3d(nom.x(),nom.y(),nom.z());
				glBegin(GL_POLYGON);
				for(j=0; j<nVtx; j++)
				{
					glVertex3d(vtx[j].x(),vtx[j].y(),vtx[j].z());
				}
				glEnd();
			}
		}
	}
}


void Init(void)
{
	GLfloat mat_specular[]={1.0F,1.0F,1.0F,1.0F};
	GLfloat mat_shininess[]={50.0F};
	GLfloat light_position[]={1.0F,1.0F,1.0F,0.0F};
	GLfloat light_diffuse[]={1.0F,1.0F,1.0F,1.0F};
	GLfloat light_specular[]={1.0F,1.0F,1.0F,1.0F};
	GLfloat light_ambient[]={0.2F,0.2F,0.2F,0.2F};

	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_diffuse);
	glLightfv(GL_LIGHT0,GL_AMBIENT,light_ambient);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_specular);

	glClearColor(0.0,0.0,0.0,0.0);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT|GL_BACK,GL_SPECULAR,mat_specular);
	glMaterialfv(GL_FRONT|GL_BACK,GL_SHININESS,mat_shininess);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glPolygonMode(GL_BACK,GL_LINE);
	glPolygonMode(GL_FRONT,GL_FILL);
}

void Display(void)
{
	double matBuf[4*4];
	int i,j;
	YsMatrix4x4 eyeTfm,tfm1,tfm2;
	YsVec3 eyePos;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	eyePos.Set(0.0,0.0,eyeDistance);
	eyePos=eyeAtt.GetMatrix()*eyePos;

	eyeTfm.Translate(eyePos);
	eyeTfm=eyeTfm*eyeAtt.GetMatrix();
	eyeTfm.Invert();
	eyeTfm.Translate(-eyeLookAt);

	eyeTfm.GetArray(matBuf);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	glMultMatrixd(matBuf);

	glPushMatrix();

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

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0,(GLsizei) w,(GLsizei) h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0,(GLdouble)w/(GLdouble)h,1.5,50.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:
		exit(0);
		break;
	case 'C':
	case 'c':
		collisionDetection=(collisionDetection==YSTRUE ? YSFALSE : YSTRUE);
		switch(collisionDetection)
		{
		case YSTRUE:
			printf("C...Detect Collision\n");
			break;
		case YSFALSE:
			printf("C...Don't Detect Collision\n");
			break;
		}
		glutPostRedisplay();
		break;
	case 'P':
	case 'p':
		inPolygon=(inPolygon==YSTRUE ? YSFALSE : YSTRUE);
		glutPostRedisplay();
		break;
	case 'A':
	case 'a':
		printf("A....Viewing Rotation Mode\n");
		viewMode=0;
		break;
	case 'B':
	case 'b':
		printf("B....Viewing Translation (Scroll) Mode\n");
		viewMode=1;
		break;
	case 'M':
	case 'm':
		if(eyeDistance<40.0)
		{
			eyeDistance=eyeDistance+2.0;
			glutPostRedisplay();
		}
		break;
	case 'Z':
	case 'z':
		if(eyeDistance>2.0)
		{
			eyeDistance=eyeDistance-2.0;
			glutPostRedisplay();
		}
		break;
	case 'S':
	case 's':
		pause=(pause==YSTRUE ? YSFALSE :YSTRUE);
		break;
	case 'N':
	case 'n':
		{
			YsList <YsShell> *newShell;
			int i,j;
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
			glutPostRedisplay();
			printf("Done\n");
		}
		break;
	case 13:
		time=0.0;
		break;
	}
}

void Mouse(int btn,int sta,int x,int y)
{
	button=sta;
	lastX=x;
	lastY=y;
}

void Motion(int x,int y)
{
	if(button==GLUT_DOWN)
	{
		double dx,dy;
		YsVec3 vec;

		dx=double(x-lastX);
		dy=double(lastY-y);
		switch(viewMode)
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
		glutPostRedisplay();
	}
	lastX=x;
	lastY=y;
}

void Idle(void)
{
	if(pause==YSFALSE)
	{
		time=time+0.25;

		double p;
		p=YsDegToRad(GetFunctionValue(gearAngle,time));
		gear1att.SetP(p);

		double b;
		b=YsDegToRad(GetFunctionValue(gearDoorAngle,time));
		lid1att.SetB(b);
		lid2att.SetB(-b);

		double x;
		x=1.1+GetFunctionValue(gearExtension,time);
		gearExt=x;
		glutPostRedisplay();
	}
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

int main(int argc, char** argv)
{
	eyeAtt.Set(YsPi/2.0,0.0,0.0);

	gear1pos.Set(0.0,2.4,-6.5); // Z=-7.0 for no collision
	gear2pos.Set(0.0,0.0,1.1);
	gearExt=1.1;

	lid1pos.Set( 2.35,-0.1,0.0);
	lid2pos.Set(-2.35,-0.1,0.0);

	shlLst[0]=&fuselage;
	shlLst[1]=&gear1;
	shlLst[2]=&gear2;
	shlLst[3]=&lid1;
	shlLst[4]=&lid2;

	collided=NULL;

	eyeDistance=20.0;

	YsCoordSysModel=YSOPENGL;

	LoadTri(fuselage,"fuselage.tri");
	LoadTri(gear1,"gear1.tri");
	LoadTri(gear2,"gear2.tri");
	LoadTri(lid1,"lid1.tri");
	LoadTri(lid2,"lid2.tri");

	blue.Set(0,0,200);
	red.Set(200,0,0);
	green.Set(0,200,0);
	yellow.Set(200,200,0);

	printf("Keys\n");
	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("Z....Zoom\n");
	printf("M....Mooz\n");
	printf("S....Pause/Restart animation.\n");
	printf("P....Draw in polygon/wireframe mode.\n");
	printf("N....Calculate colliding volume.\n");
	printf("Enter.....Back to initial position.\n");
	printf("----------\n");

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	Init();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutIdleFunc(Idle);
	glutMainLoop();
	return 0;
}


