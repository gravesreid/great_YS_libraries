#include <stdio.h>
#include <string.h>

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


int Menu(void)
{
	int r,key;
	r=0;
	while(r==0)
	{
		FsPollDevice();
		key=FsInkey();
		switch(key)
		{
		case FSKEY_S:
			r=1;
			break;
		case FSKEY_ESC:
			r=2;
			break;
		}

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-0.5,(GLdouble)wid-0.5,(GLdouble)hei-0.5,-0.5,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		const char *msg1="S.....Start Game";
		const char *msg2="ESC...Exit";
		glColor3ub(255,255,255);
		glRasterPos2i(32,32);
		glCallLists(strlen(msg1),GL_UNSIGNED_BYTE,msg1);

		glRasterPos2i(32,48);
		glCallLists(strlen(msg2),GL_UNSIGNED_BYTE,msg2);

		FsSwapBuffers();
		FsSleep(10);
	}
	return r;
}

int Game(void)
{
	FsPassedTime();

	int rackSize=50;
	int rackX,rackY,ballX,ballY,ballVx,ballVy,ballSpd;
	int wid,hei;
	int score;

	FsGetWindowSize(wid,hei);
	ballX=wid/2;
	ballY=hei/2;
	ballVx=-1;
	ballVy=-1;
	ballSpd=3;

	score=0;
	while(1)
	{
		int lb,mb,rb,mx,my;
		int passedTime;

		FsPollDevice();
		FsGetMouseState(lb,mb,rb,mx,my);
		FsGetWindowSize(wid,hei);

		rackX=mx;
		rackY=hei-30;


		ballX+=ballVx*ballSpd;
		ballY+=ballVy*ballSpd;
		if(ballX<0 && ballVx<0)
		{
			ballX=-ballX;
			ballVx=-ballVx;
		}
		if(ballY<0 && ballVy<0)
		{
			ballY=-ballY;
			ballVy=-ballVy;
		}
		if(ballX>wid && ballVx>0)
		{
			ballX=wid-(ballX-wid);
			ballVx=-ballVx;
		}
		if(rackY<=ballY && ballY<rackY+10 &&
		   rackX-rackSize<=ballX && ballX<=rackX+rackSize &&
		   ballVy>0)
		{
			score++;
			ballVy=-ballVy;
			if(ballSpd<10)
			{
				ballSpd++;
			}
			if(rackSize>30)
			{
				rackSize--;
			}
		}
		if(ballY>hei)
		{
			break;
		}

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-0.5,(GLdouble)wid-0.5,(GLdouble)hei-0.5,-0.5,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		glBegin(GL_QUADS);

		glVertex2i(rackX-rackSize,rackY);
		glVertex2i(rackX+rackSize,rackY);
		glVertex2i(rackX+rackSize,rackY+10);
		glVertex2i(rackX-rackSize,rackY+10);

		glVertex2i(ballX-3,ballY-3);
		glVertex2i(ballX+3,ballY-3);
		glVertex2i(ballX+3,ballY+3);
		glVertex2i(ballX-3,ballY+3);

		glEnd();

		char str[256];
		sprintf(str,"SCORE:%d",score);
		glRasterPos2i(32,32);
		glCallLists(strlen(str),GL_UNSIGNED_BYTE,str);

		FsSwapBuffers();

		passedTime=FsPassedTime(); // Making it up to 50fps
		FsSleep(20-passedTime);
	}
	return score;
}

void GameOver(int score)
{
	int r;
	r=0;

	FsPollDevice();
	while(FsInkey()!=0)
	{
		FsPollDevice();
	}

	while(FsInkey()==0)
	{
		FsPollDevice();

		int wid,hei;
		FsGetWindowSize(wid,hei);

		glViewport(0,0,wid,hei);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,(float)wid-1,(float)hei-1,0,-1,1);

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		const char *msg1="Game Over";
		char msg2[256];
		glColor3ub(255,255,255);
		glRasterPos2i(32,32);
		glCallLists(strlen(msg1),GL_UNSIGNED_BYTE,msg1);

		sprintf(msg2,"Your score is %d",score);

		glRasterPos2i(32,48);
		glCallLists(strlen(msg2),GL_UNSIGNED_BYTE,msg2);

		FsSwapBuffers();
		FsSleep(10);
	}
}

int main(void)
{
	int menu;
	FsOpenWindow(32,32,800,600,1); // 800x600 pixels, useDoubleBuffer=1

	int listBase;
	listBase=glGenLists(256);
	YsGlUseFontBitmap8x12(listBase);
	glListBase(listBase);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDepthFunc(GL_ALWAYS);

	while(1)
	{
		menu=Menu();
		if(menu==1)
		{
			int score;
			score=Game();
			GameOver(score);
		}
		else if(menu==2)
		{
			break;
		}
	}

	return 0;
}


