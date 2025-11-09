#include <stdio.h>
#include <string.h>
#include <ctype.h>

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
#include <ysglfontdata.h>


int main(void)
{
	FsOpenWindow(32,32,800,600,1); // 800x600 pixels, useDoubleBuffer=1

	int listBase;
	listBase=glGenLists(256);
	YsGlUseFontBitmap8x12(listBase);
	glListBase(listBase);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);

	int nChr;
	char str[256];

	nChr=0;
	str[0]=0;

	FsPassedTime();
	while(1)
	{
		int passedTime;
		int keyCode,charCode;

		FsPollDevice();
		keyCode=FsInkey();
		charCode=FsInkeyChar();

		if(isprint(charCode) && nChr<255)
		{
			str[nChr]=charCode;
			nChr++;
			str[nChr]=0;
		}
		if(keyCode==FSKEY_ESC)
		{
			break;
		}
		else if(keyCode==FSKEY_BS && nChr>0)
		{
			nChr--;
			str[nChr]=0;
		}

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		const char *msg="Type Keys:";

		glColor3ub(255,255,255);

		glRasterPos2i(32,32);
		glCallLists(strlen(msg),GL_UNSIGNED_BYTE,msg);

		glRasterPos2i(32,48);
		glCallLists(nChr,GL_UNSIGNED_BYTE,str);

		FsSwapBuffers();

		passedTime=FsPassedTime();
		FsSleep(20-passedTime);
	}

	return 0;
}


