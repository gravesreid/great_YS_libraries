#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef MACOSX
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <fssimplewindow.h>

int main(void)
{
	double cubeMatrix[16]=
	{
		1.0,0.0,0.0,0.0,
		0.0,1.0,0.0,0.0,
		0.0,0.0,1.0,0.0,
		0.0,0.0,0.0,1.0
	};

	FsOpenWindow(32,32,800,600,1); // 800x600 pixels, useDoubleBuffer=1

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_LESS);

	FsPassedTime();  // Reset the timer

	while(FsInkey()==0)
	{
		int mx,my,lb,mb,rb,passed;
		double spinX,spinY;

		passed=FsPassedTime();

		FsPollDevice();
		FsGetMouseState(lb,mb,rb,mx,my);

		int wid,hei,cx,cy;
		FsGetWindowSize(wid,hei);
		cx=wid/2;
		cy=hei/2;

		spinX=(double)((mx-cx)/10)*(double)passed/1000.0;  // 1 pixel = degrees/sec
		spinY=(double)((my-cy)/10)*(double)passed/1000.0;  // 1 pixel = degrees/sec

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0,(double)wid/(double)hei,1.0,20.0);
		glTranslated(0.0,0.0,-10.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotated(spinX,0.0,1.0,0.0);
		glRotated(spinY,1.0,0.0,0.0);
		glMultMatrixd(cubeMatrix);
		glGetDoublev(GL_MODELVIEW_MATRIX,cubeMatrix);

		glBegin(GL_QUADS);

		glColor3ub(0,0,255);
		glVertex3d(-2.5,-2.5,-2.5);
		glVertex3d(-2.5, 2.5,-2.5);
		glVertex3d( 2.5, 2.5,-2.5);
		glVertex3d( 2.5,-2.5,-2.5);

		glColor3ub(255,0,0);
		glVertex3d(-2.5,-2.5, 2.5);
		glVertex3d(-2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5, 2.5);
		glVertex3d( 2.5,-2.5, 2.5);

		glColor3ub(255,0,255);
		glVertex3d(-2.5,-2.5,-2.5);
		glVertex3d(-2.5,-2.5, 2.5);
		glVertex3d(-2.5, 2.5, 2.5);
		glVertex3d(-2.5, 2.5,-2.5);

		glColor3ub(0,255,0);
		glVertex3d( 2.5,-2.5,-2.5);
		glVertex3d( 2.5,-2.5, 2.5);
		glVertex3d( 2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5,-2.5);

		glColor3ub(0,255,255);
		glVertex3d(-2.5,-2.5,-2.5);
		glVertex3d(-2.5,-2.5, 2.5);
		glVertex3d( 2.5,-2.5, 2.5);
		glVertex3d( 2.5,-2.5,-2.5);

		glColor3ub(255,255,0);
		glVertex3d(-2.5, 2.5,-2.5);
		glVertex3d(-2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5, 2.5);
		glVertex3d( 2.5, 2.5,-2.5);

		glEnd();

		FsSwapBuffers();

		FsSleep(20-passed);
	}

	return 0;
}


