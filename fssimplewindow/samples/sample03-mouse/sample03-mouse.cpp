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
	int cx,cy;

	FsOpenWindow(32,32,800,600,1); // 800x600 pixels, useDoubleBuffer=1

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);

	cx=400;
	cy=300;

	while(FsInkey()==0)
	{
		int mx,my,lb,mb,rb;

		FsPollDevice();
		FsGetMouseState(lb,mb,rb,mx,my);

		if(cx<mx-5)
		{
			cx+=5;
		}
		if(cx>mx+5)
		{
			cx-=5;
		}
		if(cy<my-5)
		{
			cy+=5;
		}
		if(cy>my+5)
		{
			cy-=5;
		}

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

		glColor3ub(255,255,255);
		glBegin(GL_QUADS);
		glVertex2i(cx   ,cy+25);
		glVertex2i(cx+25,cy   );
		glVertex2i(cx   ,cy-25);
		glVertex2i(cx-25,cy   );
		glEnd();

		FsSwapBuffers();

		FsSleep(10);
	}

	return 0;
}


