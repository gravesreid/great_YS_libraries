#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ysdef.h"
#include "ysbase.h"
#include "ysclassmgr.h"
#include "ysprintf.h"
#include "ysmath.h"
#include "ysmatrix.h"
#include "ysmatrixtemplate.h"
#include "ysgeometry.h"
#include "yslist.h"
#include "ysarray.h"
#include "ysadvgeometry.h"


////////////////////////////////////////////////////////////

static const int YsPrintBufSize=4096;
static int YsPrintBufUsed=0;
static char YsPrintBuf[YsPrintBufSize];

// Implementation //////////////////////////////////////////

YsVec2::YsVec2(const char *x,const char *y)
{
	value[0]=atof(x);
	value[1]=atof(y);
}

YSBOOL YsVec2::IsNormalized(void) const
{
	double verify;
	verify=GetSquareLength();
	if(1.0-YsTolerance<=verify && verify<=1.0+YsTolerance)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSRESULT YsVec2::Normalize(void)
{
	double length;
	length=GetSquareLength();
	if(length>YsTolerance*YsTolerance)  // Modified 2000/11/10
	{
		length=sqrt(length);
		value[0]/=length;
		value[1]/=length;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

const char *YsVec2::Txt(const char *fmt) const
{
	int n;
	char buf[1024];
	n=sprintf(buf,fmt,x(),y());

	if(4095<=n)
	{
		return "Cannot print YsVec2.  Buffer Overflow.\n";
	}

	if(YsPrintBufSize-YsPrintBufUsed<n+1)
	{
		YsPrintBufUsed=0;
	}

	char *txtPos;
	txtPos=YsPrintBuf+YsPrintBufUsed;
	strcpy(txtPos,buf);
	YsPrintBufUsed+=n+1;
	return txtPos;
}


////////////////////////////////////////////////////////////

// Implementation //////////////////////////////////////////

YsVec3::YsVec3(const char *x,const char *y,const char *z)
{
	value[0]=atof(x);
	value[1]=atof(y);
	value[2]=atof(z);
}

YSBOOL YsVec3::IsNormalized(void) const
{
	double verify;
	verify=GetSquareLength();
	if(YsSqr(1.0-YsTolerance)<=verify && verify<=YsSqr(1.0+YsTolerance))  // Modified 2000/11/10
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSRESULT YsVec3::Normalize(void)
{
	double length;
	length=GetSquareLength();
	if(length>YsSqr(YsTolerance))
	{
		length=sqrt(length);
		value[0]/=length;
		value[1]/=length;
		value[2]/=length;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YsVec3 YsVec3::GetArbitraryPerpendicularVector(void) const
{
	YsVec3 vec;

	if(fabs(z())<fabs(y()) && fabs(z())<fabs(z()))
	{
		vec.Set(-y(),x(),0.0);
	}
	else if(fabs(y())<fabs(x()))
	{
		vec.Set(-z(),0.0,x());
	}
	else
	{
		vec.Set(0.0,-z(),y());
	}


//	double x,y,z;
//	x=value[0];
//	y=value[1];
//	z=value[2];
//	vec.Set(y-z+y*z,z-x-2*x*z,x-y+x*y);
//	vec.Normalize();

//	if(YsEqual(x(),0.0)==YSTRUE)
//	{
//		vec.Set(1.0,0.0,0.0);
//	}
//	else if(YsEqual(y(),0.0)==YSTRUE)
//	{
//		vec.Set(0.0,1.0,0.0);
//	}
//	else if(YsEqual(z(),0.0)==YSTRUE)
//	{
//		vec.Set(0.0,0.0,1.0);
//	}
//	else
//	{
//		double a,b,c;
//		/* (a,b,c)(d,e,f) -> ad+be+cf=0                */
//		/* P= ad ,Q=  be,R=  cf -> P+Q+R=0             */
//		/* P=2abc,Q=-abc,R=-abc -> P+Q+R=0             */
//	    /*   -> d=2bc,e=-ac,f=-ab                      */
//		a=x();
//		b=y();
//		c=z();
//		vec.Set(2.0*b*c,-a*c,-a*b);
//		vec.Normalize();
//	}
	return vec;
}

const char *YsVec3::Txt(const char *fmt) const
{
	int n;
	char buf[1024];
	n=sprintf(buf,fmt,x(),y(),z());

	if(4095<=n)
	{
		return "Cannot print YsVec3.  Buffer Overflow.\n";
	}

	if(YsPrintBufSize-YsPrintBufUsed<n+1)
	{
		YsPrintBufUsed=0;
	}

	char *txtPos;
	txtPos=YsPrintBuf+YsPrintBufUsed;
	strcpy(txtPos,buf);
	YsPrintBufUsed+=n+1;
	return txtPos;
}

////////////////////////////////////////////////////////////

// Implementation //////////////////////////////////////////
YsRotation::YsRotation(double x,double y,double z,double a)
{
	axis.Set(x,y,z);
	axis.Normalize();
	angle=a;
}

YsRotation::YsRotation(const YsVec3 &ax,double a)
{
	axis.Set(ax.x(),ax.y(),ax.z());
	axis.Normalize();
	angle=a;
}

YSRESULT YsRotation::RotatePositive(YsVec3 &result,const YsVec3 &start) const
{
	// YsMatrix4x4 mat;
	// mat.Rotate(*this);
	// mat.Mul(result,start,0.0);

	YsVec3 v0,v0p,b,aav;
	const YsVec3 &v=start;
	const YsVec3 &a=axis;

	// see memo/ysrotation.doc
	aav=a*(a*v);
	v0=v-aav;
	b=a^v0;
	v0p=v0*cos(angle)+b*sin(angle);
	result=v0p+aav;

	return YSOK;
}

YSRESULT YsRotation::RotateNegative(YsVec3 &result,const YsVec3 &start) const
{
	// YsMatrix4x4 mat;
	// mat.Rotate(*this);
	// mat.MulInverse(result,start,0.0);

	YsVec3 v0,v0p,b,aav;
	const YsVec3 &v=start;
	const YsVec3 &a=axis;

	aav=a*(a*v);
	v0=v-aav;
	b=v0^a;
	v0p=v0*cos(angle)+b*sin(angle);
	result=v0p+aav;

	return YSOK;
}

////////////////////////////////////////////////////////////
// Implementation //////////////////////////////////////////
YsAtt3::YsAtt3(const double &h,const double &p,const double &b)
{
	Set(h,p,b);
}

void YsAtt3::Set(const double &h,const double &p,const double &b)
{
	hdg=h;
	pch=p;
	bnk=b;
}

void YsAtt3::SetH(const double &h)
{
	hdg=h;
}

void YsAtt3::SetP(const double &p)
{
	pch=p;
}

void YsAtt3::SetB(const double &b)
{
	bnk=b;
}

YsVec3 YsAtt3::GetForwardVector(void) const
{
	YsVec3 vec;
	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		vec.Set(0.0,0.0,1.0);
		break;
	case YSOPENGL:
		vec.Set(0.0,0.0,-1.0);
		break;
	}
	Mul(vec,vec);
	return vec;
}

YsVec3 YsAtt3::GetUpVector(void) const
{
	YsVec3 vec;
	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		vec.Set(0.0,1.0,0.0);
		break;
	case YSOPENGL:
		vec.Set(0.0,1.0,0.0);
		break;
	}
	Mul(vec,vec);
	return vec;
}

YSRESULT YsAtt3::SetTwoVector(const YsVec3 &fwd,const YsVec3 &up)
{
	SetForwardVector(fwd);
	SetUpVector(up);
	return YSOK;
}

YSRESULT YsAtt3::SetForwardVector(YsVec3 ev)
{
	if(ev.IsNormalized()!=YSTRUE)
	{
		ev.Normalize();
	}

	if(ev.y()>1.0)   // 2002/02/05
	{
		ev.SetY(1.0);
	}
	else if(ev.y()<-1.0)
	{
		ev.SetY(-1.0);
	}


	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		hdg=atan2(-ev.x(),ev.z());
		pch=asin(ev.y());
		break;
	case YSOPENGL:
		hdg=atan2(-ev.x(),-ev.z());
		pch=asin(ev.y());
		break;
	}
	bnk=0.0;

	return YSOK;
}

YSRESULT YsAtt3::SetUpVector(YsVec3 uv)
{
	if(uv.IsNormalized()!=YSTRUE)
	{
		uv.Normalize();
	}

	SetB(0.0);

	MulInverse(uv,uv);

	double dBnk=0.0;
	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		dBnk=atan2(-uv.x(),uv.y());
		break;
	case YSOPENGL:
		dBnk=atan2(-uv.x(),uv.y());
		break;
	}
	SetB(dBnk);

	return YSOK;
}

void YsAtt3::NoseUp(const double &d)
{
	YsVec3 fwd,up;
	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		fwd.Set(0.0,sin(d), cos(d));
		up.Set (0.0,cos(d),-sin(d));
		break;
	case YSOPENGL:
		fwd.Set(0.0,sin(d),-cos(d));
		up.Set (0.0,cos(d), sin(d));
		break;
	}
	Mul(fwd,fwd);
	Mul(up,up);
	SetTwoVector(fwd,up);
}

void YsAtt3::YawLeft(const double &d)
{
	YsVec3 fwd,up;
	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		fwd.Set(-sin(d),0.0, cos(d));
		up.Set (0.0,1.0,0.0);
		break;
	case YSOPENGL:
		fwd.Set(-sin(d),0.0,-cos(d));
		up.Set (0.0,1.0,0.0);
		break;
	}
	Mul(fwd,fwd);
	Mul(up,up);
	SetTwoVector(fwd,up);
}

// void YsAtt3::RefreshMatrix(void)
// {
// 	mat.Initialize();
// 	switch(YsCoordSysModel)
// 	{
// 	case YSBLUEIMPULSE:
// 		mat.RotateXZ(hdg);
// 		mat.RotateZY(pch);
// 		mat.RotateXY(bnk);
// 		break;
// 	case YSOPENGL:
// 		mat.RotateXZ(-hdg);
// 		mat.RotateZY(-pch);
// 		mat.RotateXY(bnk);
// 		break;
// 	}
// }

//const YsMatrix4x4 &YsAtt3::GetMatrix(void) const
// {
// 	return mat;
// }

////////////////////////////////////////////////////////////
// Implementation //////////////////////////////////////////
YsPlane::YsPlane(const YsVec3 &o,const YsVec3 &n)
{
	Set(o,n);
}

YSBOOL YsPlane::CheckOnPlane(const YsVec3 &pos) const
{
	YsVec3 prj;
	GetNearestPoint(prj,pos);
	if(prj==pos)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}

//	YsVec3 vec;
//	vec=pos-org;
//	vec.Normalize();
//	return YsZero(vec*nom);
}

YSRESULT YsPlane::GetIntersection
    (YsVec3 &pos,const YsVec3 &lnOrg,const YsVec3 &lnVec) const
{
	double fa,fb;

	fa=-nom.x()*(lnOrg.x()-org.x())
	   -nom.y()*(lnOrg.y()-org.y())
	   -nom.z()*(lnOrg.z()-org.z());
	fb= nom.x()* lnVec.x()
	   +nom.y()* lnVec.y()
	   +nom.z()* lnVec.z();

	if(YsAbs(fb)<YsTolerance)
	{
		return YSERR;
	}

	double x,y,z;
	x=lnOrg.x()+lnVec.x()*fa/fb;
	y=lnOrg.y()+lnVec.y()*fa/fb;
	z=lnOrg.z()+lnVec.z()*fa/fb;
	pos.Set(x,y,z);

	return YSOK;
}

YSRESULT YsPlane::GetPenetration
    (YsVec3 &crs,const YsVec3 &p1,const YsVec3 &p2) const
{
	if(((p1-org)*nom)*((p2-org)*nom)<YsTolerance &&
	   GetIntersection(crs,p1,p2-p1)==YSOK &&
	   (p1-crs)*(p2-crs)<0.0)
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsPlane::MakeBestFitPlane(int np,const YsVec3 p[])
{
	if(np>=10)
	{
		if(YsFindLeastSquareFittingPlaneNormal(nom,np,p)==YSOK)
		{
			org=p[0];
			return YSOK;
		}
		YsErrOut("YsPlane::MakeBestFitPlane\nDetected Bad Polygon (least square)\n");
	}
	else if(np>=3 && YsGetAverageNormalVector(nom,np,p)==YSOK)
	{
		if(nom.GetSquareLength()>=YsSqr(YsTolerance))
		{
			org=p[0];
			return YSOK;
		}
		YsErrOut("YsPlane::MakeBestFitPlane\nDetected Bad Polygon (largest tri)\n");
	}
	return YSERR;
}

YSRESULT YsPlane::MakePlaneFromTriangle(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3)
{
	int i;
	YsVec3 v1,v2,n;

	SetOrigin(p1);

	for(i=0; i<3; i++)
	{
		switch(i)
		{
		case 0:
			v1=p2-p1;
			v2=p3-p1;
			break;
		case 1:
			v1=p1-p2;
			v2=p3-p2;
			break;
		case 2:
			v1=p1-p3;
			v2=p2-p3;
			break;
		}
		if(v1.Normalize()==YSOK && v2.Normalize()==YSOK)
		{
			n=v1^v2;
			if(n.Normalize()==YSOK)
			{
				SetNormal(n);
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsPlane::GetNearestPoint(YsVec3 &np,const YsVec3 &from) const
{
	if(nom.IsNormalized()==YSTRUE)
	{
		np=from-nom*(nom*(from-org));
		return YSOK;
	}
	else
	{
		YsErrOut("YsPlane::GetNearestPoint()\n  Normal is not set (or zero vector is given)!\n");
		return YSERR;
	}
}

int YsPlane::GetSideOfPlane(const YsVec3 &ref) const
{
	double ip;
	YsVec3 vec;
	vec=ref-org;
	vec.Normalize();

	ip=vec*GetNormal();

	if(YsAbs(ip)<YsTolerance)
	{
		return 0;
	}
	else
	{
		return (ip>0 ? 1 : -1);
	}
}

double YsPlane::GetDistance(const YsVec3 &from) const
{
	return fabs((from-GetOrigin())*GetNormal());
}

////////////////////////////////////////////////////////////

inline void YsSwapVec2(YsVec2 &a,YsVec2 &b)
{
	YsSwapSomething <YsVec2> (a,b);
}

inline void YsSwapVec3(YsVec3 &a,YsVec3 &b)
{
	YsSwapSomething <YsVec3> (a,b);
}
