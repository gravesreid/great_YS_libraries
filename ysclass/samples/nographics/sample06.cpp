#include <stdio.h>
#include <ysclass.h>

// This program demonstrates 2D polygon slashing by YsSword class.
// YsSword class has more features like:
//   Triangulate a 2D or 3D polygon
//   Separate a 2D or 3D polygon into piecewise convex polygons
//   Slash a 2D polygon by 2D lines.
//   Slash a 3D polygon by another 3D polygon or by a plane.
//   etc....
// Please refer to online manual for more information.

char vScreen[24][75];

void ClearVirtualScreen(void);
void DrawPolygon(int np,int p[],char dot);
void DrawLine(int x1,int y1,int x2,int y2,char dot);
void ExposeVirtualScreen(void);

YsScreenPolygon <16,24> plgDriver;
YsDrawLineByDDA ddaDriver;

int main(void)
{
	int i,j,n;
	const int np=6;
	int p[256]=
	{
		 5,10,
		30, 4,
		60,12,
		50,20,
		45,20,
		15,17
	};
	const int nsl=2;
	int sl[nsl*4]=
	{
		 1,15,70,10,
		30, 3,40,22
	};

	YsVec2 a[256],s1,s2;
	YsSword swd;


	ClearVirtualScreen();
	DrawPolygon(6,p,'+');
	ExposeVirtualScreen();
	printf("This is an original polygon.\n");

	ClearVirtualScreen();
	DrawPolygon(np,p,'+');
	for(i=0; i<nsl; i++)
	{
		DrawLine(sl[i*4],sl[i*4+1],sl[i*4+2],sl[i*4+3],'C');
	}
	ExposeVirtualScreen();
	printf("These are slashing lines.\n");


	for(i=0; i<np; i++)
	{
		a[i].Set(double(p[i*2]),double(p[i*2+1]));
	}
	swd.SetInitialPolygon(np,a);

	for(i=0; i<nsl; i++)
	{
		s1.Set(double(sl[i*4  ]),double(sl[i*4+1]));
		s2.Set(double(sl[i*4+2]),double(sl[i*4+3]));
		swd.Slash(s1,s2-s1);  // 2nd parameter must be a vector
	}


	ClearVirtualScreen();
	for(i=0; i<swd.GetNumPolygon(); i++)
	{
		n=swd.GetNumVertexOfPolygon(i);
		swd.GetVertexListOfPolygon(a,256,i);
		for(j=0; j<n; j++)
		{
			p[j*2  ]=int(a[j].x());
			p[j*2+1]=int(a[j].y());
		}
		DrawPolygon(n,p,'0'+i);
	}
	ExposeVirtualScreen();
	printf("Slashed Into these polygons.\n");

	return 0;
}

void ClearVirtualScreen(void)
{
	int i,j;
	for(i=0; i<24; i++)
	{
		for(j=0; j<75; j++)
		{
			vScreen[i][j]='.';
		}
		vScreen[i][74]=0;
	}
}

void DrawPolygon(int np,int p[],char dot)
{
	int i,n,*buf,x,y;
	int miny,maxy;

	plgDriver.MakeHorizontalIntersectionBuffer(np,p);
	plgDriver.GetRangeOfY(miny,maxy);
	for(y=miny; y<=maxy; y++)
	{
		n=plgDriver.GetNumIntersectionAtY(y);
		buf=plgDriver.GetIntersectionBufferAtY(y);
		for(i=0; i<n; i+=2)
		{
			for(x=buf[i]; x<=buf[i+1]; x++)
			{
				vScreen[y][x]=dot;
			}
		}
	}
}

void DrawLine(int x1,int y1,int x2,int y2,char dot)
{
	ddaDriver.Set(x1,y1,x2,y2);
	vScreen[y1][x1]=dot;
	while(ddaDriver.ReachedToTheEnd()!=YSTRUE)
	{
		int x,y;
		ddaDriver.MoveOneStep();
		ddaDriver.GetPosition(x,y);
		vScreen[y][x]=dot;
	}
}

void ExposeVirtualScreen(void)
{
	int i;
	for(i=0; i<24; i++)
	{
		printf("%s\n",vScreen[i]);
	}
}
