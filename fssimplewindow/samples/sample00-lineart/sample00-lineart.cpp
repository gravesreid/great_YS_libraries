#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
	FsOpenWindow(32,32,800,600,0); // 800x600 pixels, useDoubleBuffer=0

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);

	glClear(GL_COLOR_BUFFER_BIT);

	srand(time(NULL));

	while(FsInkey()==0)
	{
		FsPollDevice();

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

		int x1,y1,x2,y2;
		int r,g,b;

		x1=rand()%wid;
		y1=rand()%hei;
		x2=rand()%wid;
		y2=rand()%hei;
		r=rand()%256;
		g=rand()%256;
		b=rand()%256;

		glColor3ub(r,g,b);
		glBegin(GL_LINES);
		glVertex2i(x1,y1);
		glVertex2i(x2,y2);
		glEnd();
		glFlush();

		FsSleep(10);
	}

	return 0;
}


