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
void PrepareShell(YsShell &shl);
void DrawShell(YsShell &shell,YsColor &col,YSBOOL inPolygon);
void RotateShell(YsShell &sh,double pitch,double yaw);
void MoveShell(YsShell &sh,YsVec3 &mov);
void DrawAxis(double length);
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


YSBOOL drawInPolygon=YSFALSE;
YsShell sh1,sh2;


int main(int argc, char** argv)
{
	InitYsClass();
	PrepareShell(sh1);
	PrepareShell(sh2);

	printf("Keys\n");
	printf("A....Viewing Rotation Mode\n");
	printf("B....Viewing Translation (Scroll) Mode\n");
	printf("C....Rotate Shell 1\n");
	printf("D....Move Shell 1\n");
	printf("E....Rotate Shell 2\n");
	printf("F....Move Shell 2\n");
	printf("G....Polygon/Wireframe\n");
	printf("O....Or(UNION)\n");
	printf("I....And(INTERSECTION)\n");
	printf("S....Minus(DIFFERENCE)\n");
	printf("Z....Zoom\n");
	printf("M....Mooz\n");
	printf("----------\n");

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

void DrawShell(YsShell &shell,YsColor &col,YSBOOL inPolygon)
{
	int i,j,k;
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

			if(inPolygon==YSFALSE || YsCheckConvex3(nVtx,vtx)==YSTRUE)
			{
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
			else
			{
				YsSword swd;
				swd.SetInitialPolygon(nVtx,vtx);
				swd.Convexnize();
				for(j=0; j<swd.GetNumPolygon(); j++)
				{
					nVtx=swd.GetNumVertexOfPolygon(j);
					swd.GetVertexListOfPolygon(vtx,256,j);
					glBegin(GL_POLYGON);
					for(k=0; k<nVtx; k++)
					{
						glVertex3dv(vtx[k].GetValue());
					}
					glEnd();
				}
			}
		}
	}
}

void RotateShell(YsShell &sh,double pitch,double yaw)
{
	YsMatrix4x4 mat,rot,viw,iViw;

	sh.GetMatrix(mat);

	viw=eyeAtt.GetMatrix();
	iViw=viw;
	iViw.Invert();

	rot.Initialize();
	rot.RotateZY(pitch);
	rot.RotateXZ(yaw);

	mat=viw*rot*iViw*mat;

	sh.SetMatrix(mat);
}

void MoveShell(YsShell &sh,YsVec3 &mov)
{
	YsMatrix4x4 mat,trn,viw,iViw;

	sh.GetMatrix(mat);

	viw=eyeAtt.GetMatrix();
	iViw=viw;
	iViw.Invert();

	trn.Initialize();
	trn.Translate(mov);

	mat=viw*trn*iViw*mat;

	sh.SetMatrix(mat);
}

void PrepareShell(YsShell &shl)
{
	int idx[8];
	idx[0]=shl.AddVertex(YsVec3(-2.5, 1.0, 1.0));
	idx[1]=shl.AddVertex(YsVec3( 2.5, 1.0, 1.0));
	idx[2]=shl.AddVertex(YsVec3(-2.5, 1.0,-1.0));
	idx[3]=shl.AddVertex(YsVec3( 2.5, 1.0,-1.0));
	idx[4]=shl.AddVertex(YsVec3(-2.5,-1.0,-1.0));
	idx[5]=shl.AddVertex(YsVec3( 2.5,-1.0,-1.0));
	idx[6]=shl.AddVertex(YsVec3(-2.5,-1.0, 1.0));
	idx[7]=shl.AddVertex(YsVec3( 2.5,-1.0, 1.0));

	int plg[4];
	plg[0]=idx[0];
	plg[1]=idx[1];
	plg[2]=idx[3];
	plg[3]=idx[2];
	shl.AddPolygon(4,plg);

	plg[0]=idx[2];
	plg[1]=idx[3];
	plg[2]=idx[5];
	plg[3]=idx[4];
	shl.AddPolygon(4,plg);

	plg[0]=idx[4];
	plg[1]=idx[5];
	plg[2]=idx[7];
	plg[3]=idx[6];
	shl.AddPolygon(4,plg);

	plg[0]=idx[6];
	plg[1]=idx[7];
	plg[2]=idx[1];
	plg[3]=idx[0];
	shl.AddPolygon(4,plg);

	plg[0]=idx[0];
	plg[1]=idx[2];
	plg[2]=idx[4];
	plg[3]=idx[6];
	shl.AddPolygon(4,plg);

	plg[0]=idx[7];
	plg[1]=idx[5];
	plg[2]=idx[3];
	plg[3]=idx[1];
	shl.AddPolygon(4,plg);
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
	DrawShell(sh1,blu,drawInPolygon);
	DrawShell(sh2,grn,drawInPolygon);
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
	YsShell tmp;
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
		printf("C....Rotate shell 1 mode\n");
		mouseMode=2;
		break;
	case 'D':
	case 'd':
		printf("D....Move shell 1 mode\n");
		mouseMode=3;
		break;
	case 'E':
	case 'e':
		printf("E....Rotate shell 2 mode\n");
		mouseMode=4;
		break;
	case 'F':
	case 'f':
		printf("F....Move shell 2 mode\n");
		mouseMode=5;
		break;
	case 'G':
	case 'g':
		printf("G....Toggle wire/polygon\n");
		drawInPolygon=(drawInPolygon==YSTRUE ? YSFALSE : YSTRUE);
		break;
	case 'O':
	case 'o':
		YsBlendShell(tmp,sh1,sh2,YSBOOLOR);
		sh1=tmp;
		break;
	case 'I':
	case 'i':
		YsBlendShell(tmp,sh1,sh2,YSBOOLAND);
		sh1=tmp;
		break;
	case 'S':
	case 's':
		YsBlendShell(tmp,sh1,sh2,YSBOOLMINUS);
		sh1=tmp;
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
			RotateShell(sh1,dy/300.0,-dx/300.0);
			break;
		case 3:
			vec.Set(dx/30.0,dy/30.0,0.0);
			//vec=eyeAtt.GetMatrix()*vec;
			MoveShell(sh1,vec);
			break;
		case 4:
			RotateShell(sh2,dy/300.0,-dx/300.0);
			break;
		case 5:
			vec.Set(dx/30.0,dy/30.0,0.0);
			//vec=eyeAtt.GetMatrix()*vec;
			MoveShell(sh2,vec);
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

