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
void PreparePolygon(YsVec2 v[]);
void DrawPolygon(int nv,YsVec3 v[],YsColor &col,int type);
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

YsVec2 pointOfInterest;
YsVec2 p[6];

int main(int argc, char** argv)
{
	InitYsClass();
	PreparePolygon(p);

	pointOfInterest.Set(0.0,0.0);

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

	glClearColor(0.0,0.0,0.0,0.0);
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

void PreparePolygon(YsVec2 v[])
{
	v[0].Set( 0.0, 5.0);
	v[1].Set( 5.0, 5.0);
	v[2].Set( 5.0, 0.0);
	v[3].Set( 0.0,-5.0);
	v[4].Set(-5.0,-5.0);
	v[5].Set(-5.0, 0.0);
}

void DrawPolygon(int nv,YsVec2 v[],YsColor &col,int type)
{
	int i;
	double r,g,b;

	col.GetDoubleRGB(r,g,b);
	glColor3d(r,g,b);

	glEnable(GL_LIGHTING);
	glBegin(type);
	for(i=0; i<nv; i++)
	{
		glVertex3d(v[i].x(),v[i].y(),-10.0);
	}
	glEnd();
}

////////////////////////////////////////////////////////////

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	DrawPolygon(6,p,whi,GL_LINE_LOOP);

	glClear(GL_DEPTH_BUFFER_BIT);

	switch(YsCheckInsidePolygon2(pointOfInterest,6,p))
	{
	case YSINSIDE:
		glColor3d(0.0,1.0,0.0);
		break;
	case YSBOUNDARY:
		glColor3d(1.0,1.0,0.0);
		break;
	case YSOUTSIDE:
		glColor3d(0.0,0.0,1.0);
		break;
	case YSUNKNOWNSIDE:
		glColor3d(1.0,0.0,0.0);
		break;
	}

	glBegin(GL_LINES);
	glVertex3d(pointOfInterest.x()-0.2,pointOfInterest.y()-0.2,-10.0);
	glVertex3d(pointOfInterest.x()+0.2,pointOfInterest.y()+0.2,-10.0);
	glVertex3d(pointOfInterest.x()+0.2,pointOfInterest.y()-0.2,-10.0);
	glVertex3d(pointOfInterest.x()-0.2,pointOfInterest.y()+0.2,-10.0);
	glEnd();

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
	}
}

void Mouse(int btn,int sta,int x,int y)
{
	button=btn;
	lastX=x;
	lastY=y;
}

void Motion(int x,int y)
{
	if(button==GLUT_DOWN)
	{
		double dx,dy;
		YsVec2 vec;

		dx=double(x-lastX);
		dy=double(lastY-y);

		vec.Set(dx/60.0,dy/60.0);
		pointOfInterest=pointOfInterest+vec;
	}
	lastX=x;
	lastY=y;
}

void Idle(void)
{
	glutPostRedisplay();
}

