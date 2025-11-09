//cl /DWIN32 sample??.cpp ysclass.lib kernel32.lib user32.lib gdi32.lib advapi32.lib opengl32.lib glu32.lib glut32.lib

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>

#include <ysclass.h>

void InitYsClass(void);
void InitOpenGL(void);
void DrawAxis(double length);
void DrawCube(double size);
void Display(void);
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int btn,int sta,int x,int y);
void Motion(int x,int y);
void Idle(void);

// I hate global variables. But, it's glut. What else can I do?
double eyeDistance=20.0;
YsVec3 eyeLookAt(0.0,0.0,0.0);
YsAtt3 eyeAtt(0.0,0.0,0.0);
YsAtt3 objAtt(0.0,0.0,0.0);
YsColor red(1.0,0.0,0.0),blu(0.0,0.0,1.0),grn(0.0,1.0,0.0),yel(1.0,1.0,0.0);

int button,lastX,lastY;
int mouseMode;  // 0:Viewing Rotation 1:Viewing Scroll


const int nVtxSet11=4;  // Easiest case. An edge of a polygon is intersect with another polygon
const int nVtxSet12=4;
double vtxSet11[4][3]=
{
	{-1.5,-1.0, 0.0},
	{ 0.5,-1.0, 0.0},
	{ 0.5, 1.0, 0.0},
	{-1.5, 1.0, 0.0}
};
double vtxSet12[4][3]=
{
	{-0.5, 0.0,-1.0},
	{ 1.5, 0.0,-1.0},
	{ 1.5, 0.0, 1.0},
	{-0.5, 0.0, 1.0}
};

const int nVtxSet21=4;  // Obvious intersection, but touching by edges
const int nVtxSet22=4;
double vtxSet21[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet22[4][3]=
{
	{-1.0, 0.0,-1.0},
	{ 1.0, 0.0,-1.0},
	{ 1.0, 0.0, 1.0},
	{-1.0, 0.0, 1.0}
};

const int nVtxSet31=4;  // Touching by a point
const int nVtxSet32=3;
double vtxSet31[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet32[3][3]=
{
	{-1.0, 0.0, 1.0},
	{ 1.0, 0.0, 1.0},
	{ 0.5, 0.5, 0.0},
};

const int nVtxSet41=4;  // No edges are penetrating, but two polygons are intersecting
const int nVtxSet42=7;
double vtxSet41[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet42[7][3]=
{
	{ 1.0, 0.0, 1.0},
	{ 0.8, 0.0, 0.0},
	{ 0.6, 0.0, 0.0},
	{ 0.0, 0.0,-1.0},
	{-0.6, 0.0, 0.0},
	{-0.8, 0.0, 0.0},
	{-1.0, 0.0, 1.0}
};

const int nVtxSet51=4;  // two points are lying on the polygon 1 but they are not intersecting
const int nVtxSet52=5;
double vtxSet51[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet52[5][3]=
{
	{-0.3, 0.0, 1.0},
	{ 1.5, 0.0, 1.0},
	{ 1.3, 0.0,-0.5},
	{ 1.0, 0.0, 0.0},
	{ 0.0, 0.0, 0.0},
};

const int nVtxSet61=4;  // Some points are lying on the polygon 1 but they are not intersecting
const int nVtxSet62=7;
double vtxSet61[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet62[7][3]=
{
	{ 1.0, 0.0, 1.0},
	{ 0.8, 0.0, 0.0},
	{ 0.6, 0.0, 0.0},
	{ 0.0, 0.0, 0.5},
	{-0.6, 0.0, 0.0},
	{-0.8, 0.0, 0.0},
	{-1.0, 0.0, 1.0}
};

const int nVtxSet71=4;  // Touching by edges
const int nVtxSet72=4;
double vtxSet71[4][3]=
{
	{-2.0,-1.0, 0.0},
	{ 0.0,-1.0, 0.0},
	{ 0.0, 1.0, 0.0},
	{-2.0, 1.0, 0.0}
};
double vtxSet72[4][3]=
{
	{-0.0, 0.0,-1.0},
	{ 2.0, 0.0,-1.0},
	{ 2.0, 0.0, 1.0},
	{-0.0, 0.0, 1.0}
};

const int nVtxSet81=4;  // Two vtxs of polygon 2 is touching edges of polygon 1
const int nVtxSet82=4;
double vtxSet81[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet82[4][3]=
{
	{ 0.0, 0.0,-1.0},
	{-1.0, 0.0, 0.0},
	{ 0.0, 0.0, 1.0},
	{ 1.0, 0.0, 0.0}
};

const int nVtxSet91=4;  // Mixed
const int nVtxSet92=4;
double vtxSet91[4][3]=
{
	{-1.0,-1.0, 0.0},
	{ 1.0,-1.0, 0.0},
	{ 1.0, 1.0, 0.0},
	{-1.0, 1.0, 0.0}
};
double vtxSet92[4][3]=
{
	{-1.0, 0.0,-1.0},
	{ 1.0, 0.0,-1.0},
	{ 1.0, 0.0, 1.0},
	{-1.0, 0.0, 0.0}
};

const int nVtxSetA1=8;  // Concave Polygon Ugly Case 1
const int nVtxSetA2=8;
double vtxSetA1[8][3]=
{
	{ 0.5, 0.5, 0.0},
	{ 0.5,-0.5, 0.0},
	{ 1.5,-0.5, 0.0},
	{ 1.5, 1.5, 0.0},
	{-1.5, 1.5, 0.0},
	{-1.5,-0.5, 0.0},
	{-0.5,-0.5, 0.0},
	{-0.5, 0.5, 0.0}
};
double vtxSetA2[8][3]=
{
	{ 0.5, 0.0, 0.5},
	{ 0.5, 0.0,-0.5},
	{ 1.5, 0.0,-0.5},
	{ 1.5, 0.0, 1.5},
	{-1.5, 0.0, 1.5},
	{-1.5, 0.0,-0.5},
	{-0.5, 0.0,-0.5},
	{-0.5, 0.0, 0.5}
};

const int nVtxSetB1=8;  // Concave Polygon Ugly Case 2
const int nVtxSetB2=8;
double vtxSetB1[8][3]=
{
	{ 0.5, 0.5, 0.0},
	{ 0.5,-0.5, 0.0},
	{ 1.5,-0.5, 0.0},
	{ 1.5, 1.5, 0.0},
	{-1.5, 1.5, 0.0},
	{-1.5,-0.5, 0.0},
	{-0.5,-0.5, 0.0},
	{-0.5, 0.5, 0.0}
};
double vtxSetB2[8][3]=
{
	{ 1.5, 0.0, 0.5},
	{ 1.5, 0.0,-0.5},
	{ 2.5, 0.0,-0.5},
	{ 2.5, 0.0, 1.5},
	{-0.5, 0.0, 1.5},
	{-0.5, 0.0,-0.5},
	{ 0.5, 0.0,-0.5},
	{ 0.5, 0.0, 0.5}
};






int nVtxSetX1;
int nVtxSetX2;
double (*vtxSetX1)[3];
double (*vtxSetX2)[3];



int main(int argc, char** argv)
{
	InitYsClass();

	printf("Demonstrate YsGetPolygonPenetration()\n");

	printf("Keys\n");
	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("Z....Zoom\n");
	printf("M....Mooz\n");
	printf("SPACE....Switch Vertex Set\n");
	printf("----------\n");


	nVtxSetX1=nVtxSet11;
	nVtxSetX2=nVtxSet12;
	vtxSetX1=vtxSet11;
	vtxSetX2=vtxSet12;


	eyeAtt.Set(YsPi/2.0,0.0,0.0);
	eyeDistance=10.0;

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	InitOpenGL();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutIdleFunc(Idle);
	glutMainLoop();
	return 0;
}

void InitYsClass(void)
{
	YsCoordSysModel=YSRIGHT_ZMINUS_YPLUS;  // Same as YSOPENGL;
}

void InitOpenGL(void)
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

	glClearColor(0.0, 0.0, 0.0, 0.0);
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

void DrawAxis(double length)
{
	glDisable(GL_LIGHTING);

	glColor3d(1.0,0.0,0.0);
	glBegin(GL_LINES);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(length,0.0,0.0);
	glEnd();

	glColor3d(0.0,1.0,0.0);
	glBegin(GL_LINES);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(0.0,length,0.0);
	glEnd();

	glColor3d(0.0,0.0,1.0);
	glBegin(GL_LINES);
	glVertex3d(0.0,0.0,0.0);
	glVertex3d(0.0,0.0,length);
	glEnd();
}

void DrawVertexSet(int nVtxSet,double vtxSet[][3])
{
	int i;
	glColor3d(1.0,1.0,1.0);
	glBegin(GL_LINE_LOOP);
	for(i=0; i<nVtxSet; i++)
	{
		glVertex3dv(vtxSet[i]);
	}
	glEnd();
}

////////////////////////////////////////////////////////////

void Display(void)
{
	double matBuf[4*4],objMatBuf[4*4];
	YsVec3 eyePos;
	YsMatrix4x4 eyeTfm;
	YsMatrix4x4 objTfm;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	eyePos.Set(0.0,0.0,eyeDistance);
	eyePos=eyeAtt.GetMatrix()*eyePos;

	eyeTfm.Initialize();
	eyeTfm.Translate(eyePos);
	eyeTfm.Rotate(eyeAtt);
	eyeTfm.Invert();
	eyeTfm.Translate(-eyeLookAt);

	eyeTfm.GetArray(matBuf);


	objTfm=objAtt.GetMatrix();
	objTfm.GetArray(objMatBuf);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(matBuf);
	DrawAxis(5.0);
	glMultMatrixd(objMatBuf);

	DrawVertexSet(nVtxSetX1,vtxSetX1);
	DrawVertexSet(nVtxSetX2,vtxSetX2);

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

void CheckPolygonPenetration(int nVtx1,double vtx1[][3],int nVtx2,double vtx2[][3])
{
	YsVec3 plg1[32],plg2[32];
	int i;
	for(i=0; i<nVtx1; i++)
	{
		plg1[i].Set(vtx1[i]);
	}
	for(i=0; i<nVtx2; i++)
	{
		plg2[i].Set(vtx2[i]);
	}

	switch(YsGetPolygonPenetration(nVtx1,plg1,nVtx2,plg2))
	{
	case YSINTERSECT:
		printf("YSINTERSECT\n");
		break;
	case YSOVERLAP:
		printf("YSOVERLAP\n");
		break;
	case YSTOUCH:
		printf("YSTOUCH\n");
		break;
	case YSSHAREPOINT:
		printf("YSSHAREPOINT\n");
		break;
	case YSAPART:
		printf("YSAPART\n");
		break;
	}
}

void Keyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
	case 27:
		exit(0);
		break;
	case 'A':
	case 'a':
		printf("A....Viewing Rotation Mode\n");
		mouseMode=0;
		break;
	case 'B':
	case 'b':
		printf("B....Viewing Translation (Scroll) Mode\n");
		mouseMode=1;
		break;
	case 'O':
	case 'o':
		printf("O...Object Rotation Mode\n");
		mouseMode=2;
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
	case ' ':
		if(vtxSetX1==vtxSet11)
		{
			nVtxSetX1=nVtxSet21;
			nVtxSetX2=nVtxSet22;
			vtxSetX1=vtxSet21;
			vtxSetX2=vtxSet22;
		}
		else if(vtxSetX1==vtxSet21)
		{
			nVtxSetX1=nVtxSet31;
			nVtxSetX2=nVtxSet32;
			vtxSetX1=vtxSet31;
			vtxSetX2=vtxSet32;
		}
		else if(vtxSetX1==vtxSet31)
		{
			nVtxSetX1=nVtxSet41;
			nVtxSetX2=nVtxSet42;
			vtxSetX1=vtxSet41;
			vtxSetX2=vtxSet42;
		}
		else if(vtxSetX1==vtxSet41)
		{
			nVtxSetX1=nVtxSet51;
			nVtxSetX2=nVtxSet52;
			vtxSetX1=vtxSet51;
			vtxSetX2=vtxSet52;
		}
		else if(vtxSetX1==vtxSet51)
		{
			nVtxSetX1=nVtxSet61;
			nVtxSetX2=nVtxSet62;
			vtxSetX1=vtxSet61;
			vtxSetX2=vtxSet62;
		}
		else if(vtxSetX1==vtxSet61)
		{
			nVtxSetX1=nVtxSet71;
			nVtxSetX2=nVtxSet72;
			vtxSetX1=vtxSet71;
			vtxSetX2=vtxSet72;
		}
		else if(vtxSetX1==vtxSet71)
		{
			nVtxSetX1=nVtxSet81;
			nVtxSetX2=nVtxSet82;
			vtxSetX1=vtxSet81;
			vtxSetX2=vtxSet82;
		}
		else if(vtxSetX1==vtxSet81)
		{
			nVtxSetX1=nVtxSet91;
			nVtxSetX2=nVtxSet92;
			vtxSetX1=vtxSet91;
			vtxSetX2=vtxSet92;
		}
		else if(vtxSetX1==vtxSet91)
		{
			nVtxSetX1=nVtxSetA1;
			nVtxSetX2=nVtxSetA2;
			vtxSetX1=vtxSetA1;
			vtxSetX2=vtxSetA2;
		}
		else if(vtxSetX1==vtxSetA1)
		{
			nVtxSetX1=nVtxSetB1;
			nVtxSetX2=nVtxSetB2;
			vtxSetX1=vtxSetB1;
			vtxSetX2=vtxSetB2;
		}
		else
		{
			nVtxSetX1=nVtxSet11;
			nVtxSetX2=nVtxSet12;
			vtxSetX1=vtxSet11;
			vtxSetX2=vtxSet12;
		}

		CheckPolygonPenetration(nVtxSetX1,vtxSetX1,nVtxSetX2,vtxSetX2);

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

		YsMatrix4x4 viwMat;
		YsVec3 ev,uv;

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
			viwMat=eyeAtt.GetMatrix();

			ev=objAtt.GetForwardVector();
			uv=objAtt.GetUpVector();
			viwMat.MulInverse(ev,ev,0.0);  // last 0.0 means no translation
			viwMat.MulInverse(uv,uv,0.0);

			ev.RotateYZ(-dy/300.0);
			ev.RotateXZ(-dx/300.0);
			uv.RotateYZ(-dy/300.0);
			uv.RotateXZ(-dx/300.0);

			viwMat.Mul(ev,ev,0.0);
			viwMat.Mul(uv,uv,0.0);

			objAtt.SetTwoVector(ev,uv);

			break;
		}
		glutPostRedisplay();
	}
	lastX=x;
	lastY=y;
}

void Idle(void)
{
	glutPostRedisplay();
}

