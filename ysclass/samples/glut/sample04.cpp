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
void PrepareSquare(YsVec3 v[]);
void RotatePolygon(YsVec3 p[],double pitch,double yaw);
void DrawAxis(double length);
void DrawPolygon(int nv,YsVec3 v[],YsColor &col,int type);
void DrawSlashedPolygon(void);
void Display(void);
void Reshape(int w, int h);
void Keyboard(unsigned char key, int x, int y);
void Mouse(int btn,int sta,int x,int y);
void Motion(int x,int y);
void Idle(void);

// I hate global variables. But, it's glut. What else can I do?
double eyeDistance=10.0;
YsVec3 eyeLookAt(0.0,0.0,0.0);
YsAtt3 eyeAtt(0.0,0.0,0.0);
YsColor red(1.0,0.0,0.0),blu(0.0,0.0,1.0),grn(0.0,1.0,0.0),yel(1.0,1.0,0.0);
YsColor whi(1.0,1.0,1.0);

int button,lastX,lastY;
int mouseMode;  // 0:Viewing Rotation 1:Viewing Scroll


YsVec3 p[4],q[4],r[4];


int main(int argc, char** argv)
{
	InitYsClass();
	PrepareSquare(p);
	PrepareSquare(q);
	RotatePolygon(q,YsDegToRad(90.0),0.0);
	PrepareSquare(r);
	RotatePolygon(r,0.0,YsDegToRad(90.0));

	printf("Keys\n");
	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("C....Rotate polygon 1(Target Polygon)\n");
	printf("D....Rotate polygon 2(Slasher)\n");
	printf("E....Rotate polygon 3(Slasher)\n");
	printf("Z....Zoom\n");
	printf("M....Mooz\n");
	printf("----------\n");

	mouseMode=3;

	eyeAtt.Set(0.0,0.0,0.0);
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

	glPolygonMode(GL_BACK,GL_FILL);
	glPolygonMode(GL_FRONT,GL_FILL);
}

void PrepareSquare(YsVec3 v[4])
{
	v[0].Set( 0.0, 5.0,0.0);
	v[1].Set( 5.0, 0.0,0.0);
	v[2].Set( 0.0,-5.0,0.0);
	v[3].Set(-5.0, 0.0,0.0);
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

void DrawPolygon(int nv,YsVec3 v[],YsColor &col,int type)
{
	int i;
	double r,g,b;

	col.GetDoubleRGB(r,g,b);
	glColor3d(r,g,b);

	glEnable(GL_LIGHTING);
	glBegin(type);
	for(i=0; i<nv; i++)
	{
		glVertex3dv(v[i].GetValue());
	}
	glEnd();
}

void DrawSlashedPolygon(void)
{
	int i,n;
	YsSword swd;
	YsVec3 v[256];

	YsColor c[5];

	c[0]=red;
	c[1]=blu;
	c[2]=grn;
	c[3]=yel;
	c[4]=whi;

	swd.SetInitialPolygon(4,p);
	swd.Slash(4,q);
	swd.Slash(4,r);

	for(i=0; i<swd.GetNumPolygon(); i++)
	{
		n=swd.GetNumVertexOfPolygon(i);
		swd.GetVertexListOfPolygon(v,256,i);
		DrawPolygon(n,v,c[i%5],GL_POLYGON);
	}
}

void RotatePolygon(YsVec3 p[],double pitch,double yaw)
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

////////////////////////////////////////////////////////////

void Display(void)
{
	double matBuf[4*4];
	YsVec3 eyePos;
	YsMatrix4x4 eyeTfm;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	eyePos.Set(0.0,0.0,eyeDistance);
	eyePos=eyeAtt.GetMatrix()*eyePos;

	eyeTfm.Initialize();
	eyeTfm.Translate(eyePos);
	eyeTfm.Rotate(eyeAtt);
	eyeTfm.Invert();
	eyeTfm.Translate(-eyeLookAt);

	eyeTfm.GetArray(matBuf);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixd(matBuf);
	DrawSlashedPolygon();
	DrawPolygon(4,q,whi,GL_LINE_LOOP);
	DrawPolygon(4,r,whi,GL_LINE_LOOP);
	DrawAxis(5.0);
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
	case 'C':
	case 'c':
		printf("C....Rotate polygon 1 mode\n");
		mouseMode=2;
		break;
	case 'D':
	case 'd':
		printf("D....Rotate polygon 2 mode\n");
		mouseMode=3;
		break;
	case 'E':
	case 'e':
		printf("E....Rotate polygon 3 mode\n");
		mouseMode=4;
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
			RotatePolygon(p,dy/300.0,-dx/300.0);
			break;
		case 3:
			RotatePolygon(q,dy/300.0,-dx/300.0);
			break;
		case 4:
			RotatePolygon(r,dy/300.0,-dx/300.0);
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

