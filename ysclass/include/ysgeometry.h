#ifndef YSGEOMETRY_IS_INCLUDED
#define YSGEOMETRY_IS_INCLUDED
/* { */

#include <math.h>

#define YSMATHFAST  // This line disables Memory Leak Tracking

////////////////////////////////////////////////////////////


// Declaration /////////////////////////////////////////////
class YsVec2
{
public:
	inline YsVec2()
	{
	}
	inline YsVec2(const double &x,const double &y)
	{
		value[0]=x;
		value[1]=y;
	}

	// Added 2001/04/30 >>
	YsVec2(const char *x,const char *y);
	// Added 2001/04/30 <<

	// Added 2001/04/26 >>
	inline const double &operator[](int idx) const
	{
		return value[idx];
	}
	inline double &operator[](int idx)
	{
		return value[idx];
	}
	operator const double *() const
	{
		return GetValue();
	}
	const char *Txt(const char *fmt="%10.6lf %10.6lf") const;
	// Added 2001/04/26 <<

	// Added 2001/05/04 >>
	inline void AddX(const double &x)
	{
		value[0]+=x;
	}
	inline void AddY(const double &y)
	{
		value[1]+=y;
	}
	inline void SubX(const double &x)
	{
		value[0]-=x;
	}
	inline void SubY(const double &y)
	{
		value[1]-=y;
	}
	inline void MulX(const double &x)
	{
		value[0]*=x;
	}
	inline void MulY(const double &y)
	{
		value[1]*=y;
	}
	inline void DivX(const double &x)
	{
		value[0]/=x;
	}
	inline void DivY(const double &y)
	{
		value[1]/=y;
	}
	// Added 2001/05/04 <<


	inline double x(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[0];
	}
	inline double y(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[1];
	}
	inline const double *GetValue(void) const
	{
		return value;
	}
	inline void Get(double &x,double &y) const
	{
		x=value[0];
		y=value[1];
	}
	inline double GetSquareLength(void) const
	{
		return value[0]*value[0]+value[1]*value[1];
	}
	inline double GetLength(void) const
	{
		return (double)sqrt(GetSquareLength());
	}

	inline double L0Norm(void) const
	{
		return YsGreater(fabs(value[0]),fabs(value[1]));
	}

	inline int GetLongestAxis(void) const
	{
		if(fabs(value[0])>fabs(value[1]))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	inline double GetLongestDimension(void) const
	{
		if(fabs(value[0])>fabs(value[1]))
		{
			return fabs(value[0]);
		}
		else
		{
			return fabs(value[1]);
		}
	}

	YSBOOL IsNormalized(void) const;
	YSRESULT Normalize(void);
	inline YSBOOL IsZeroVector(void) const
	{
		if(GetSquareLength()<YsToleranceSqr)
		{
			return YSTRUE;
		}
		else
		{
			return YSFALSE;
		}
	}
	inline void Set(const double &x,const double &y)
	{
		value[0]=x;
		value[1]=y;
	}
	inline void SetX(const double &x)
	{
		value[0]=x;
	}
	inline void SetY(const double &y)
	{
		value[1]=y;
	}
	inline void Set(const double v[2])
	{
		value[0]=v[0];
		value[1]=v[1];
	}
	inline void GetXY(const double *vec3)
	{
		value[0]=vec3[0];
		value[1]=vec3[1];
	}
	inline void GetXZ(const double *vec3)
	{
		value[0]=vec3[0];
		value[1]=vec3[2];
	}
	inline void Rotate(const double &ang)
	{
		double c,s,xx,yy;
		c=cos(ang);
		s=sin(ang);
		xx=x()*c-y()*s;
		yy=x()*s+y()*c;
		Set(xx,yy);
	}


	inline const YsVec2 &operator+=(const YsVec2 &a)
	{
		value[0]+=a.value[0];
		value[1]+=a.value[1];
		return *this;
	}
	inline const YsVec2 &operator-=(const YsVec2 &a)
	{
		value[0]-=a.value[0];
		value[1]-=a.value[1];
		return *this;
	}
	inline const YsVec2 &operator/=(double a)
	{
		value[0]/=a;
		value[1]/=a;
		return *this;
	}
	inline const YsVec2 &operator*=(double a)
	{
		value[0]*=a;
		value[1]*=a;
		return *this;
	}

protected:
	double value[2];
};

inline const YsVec2 operator-(const YsVec2 &a)
{
	return YsVec2(-a.x(),-a.y());
}

inline const YsVec2 operator-(const YsVec2 &a,const YsVec2 &b)
{
	return YsVec2(a.x()-b.x(),a.y()-b.y());
}

inline const YsVec2 operator+(const YsVec2 &a,const YsVec2 &b)
{
	return YsVec2(a.x()+b.x(),a.y()+b.y());
}

inline const YsVec2 operator/(const YsVec2 &a,const double &b)
{
	return YsVec2(a.x()/b,a.y()/b);
}

inline const YsVec2 operator*(const YsVec2 &a,const double &b)
{
	return YsVec2(a.x()*b,a.y()*b);
}

inline const YsVec2 operator*(const double &b,const YsVec2 &a)
{
	return YsVec2(a.x()*b,a.y()*b);
}

inline double operator*(const YsVec2 &a,const YsVec2 &b)
{
	return a.x()*b.x()+a.y()*b.y();
}

inline double operator^(const YsVec2 &a,const YsVec2 &b)
{
	// x1 y1
	// x2 y2
	double z;
	z=a.x()*b.y()-a.y()*b.x();
	return z;
}

inline int operator==(const YsVec2 &a,const YsVec2 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()<YsTolerance);

	// Modified again 2000/11/10
	// if(YsEqual(a.x(),b.x())==YSTRUE &&
	//    YsEqual(a.y(),b.y())==YSTRUE)
	// {
	// 	return 1;
	// }
	// else
	// {
	// 	return 0;
	// }

	return ((a-b).GetSquareLength()<YsSqr(YsTolerance));  // Modified 2000/11/10
}

inline int operator!=(const YsVec2 &a,const YsVec2 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()>=YsTolerance);

	// Modified again 2000/11/10
	// if(YsEqual(a.x(),b.x())!=YSTRUE ||
	//    YsEqual(a.y(),b.y())!=YSTRUE)
	// {
	// 	return 1;
	// }
	// else
	// {
	// 	return 0;
	// }

	return ((a-b).GetSquareLength()>=YsSqr(YsTolerance));  // Modified 2000/11/10
}


inline const YsVec2 &YsLonger(const YsVec2 &v1,const YsVec2 &v2)
{
	return (v1.GetSquareLength()>=v2.GetSquareLength() ? v1 : v2);
}


////////////////////////////////////////////////////////////

// Declaration /////////////////////////////////////////////
class YsVec3
{
public:
	inline YsVec3()
	{
	}
	inline YsVec3(const double &x,const double &y,const double &z)
	{
		value[0]=x;
		value[1]=y;
		value[2]=z;
	}
	YsVec3(const char *x,const char *y,const char *z);

	// Added 2001/04/26 >>
	inline const double &operator[](int idx) const
	{
		return value[idx];
	}
	inline double &operator[](int idx)
	{
		return value[idx];
	}
	operator const double *() const
	{
		return GetValue();
	}
	const char *Txt(const char *fmt="%10.6lf %10.6lf %10.6lf") const;
	// Added 2001/04/26 <<

	// Added 2001/05/04 >>
	inline void AddX(const double &x)
	{
		value[0]+=x;
	}
	inline void AddY(const double &y)
	{
		value[1]+=y;
	}
	inline void AddZ(const double &z)
	{
		value[2]+=z;
	}
	inline void SubX(const double &x)
	{
		value[0]-=x;
	}
	inline void SubY(const double &y)
	{
		value[1]-=y;
	}
	inline void SubZ(const double &z)
	{
		value[2]-=z;
	}
	inline void MulX(const double &x)
	{
		value[0]*=x;
	}
	inline void MulY(const double &y)
	{
		value[1]*=y;
	}
	inline void MulZ(const double &z)
	{
		value[2]*=z;
	}
	inline void DivX(const double &x)
	{
		value[0]/=x;
	}
	inline void DivY(const double &y)
	{
		value[1]/=y;
	}
	inline void DivZ(const double &z)
	{
		value[2]/=z;
	}
	// Added 2001/05/04 <<

	inline double x(void) const   // 2001/04/17 : Don't make it const double &
	{
		return value[0];
	}
	inline double y(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[1];
	}
	inline double z(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[2];
	}
	inline const double *GetValue(void) const
	{
		return value;
	}
	inline void Set(const double &x,const double &y,const double &z)
	{
		value[0]=x;
		value[1]=y;
		value[2]=z;
	}
	inline void SetX(const double &x)
	{
		value[0]=x;
	}
	inline void SetY(const double &y)
	{
		value[1]=y;
	}
	inline void SetZ(const double &z)
	{
		value[2]=z;
	}
	inline void SetXY(const YsVec2 &vec2)
	{
		value[0]=vec2[0];
		value[1]=vec2[1];
		value[2]=0.0;
	}
	inline void SetXZ(const YsVec2 &vec2)
	{
		value[0]=vec2[0];
		value[1]=0.0;
		value[2]=vec2[1];
	}
	inline void Set(const double v[3])
	{
		value[0]=v[0];
		value[1]=v[1];
		value[2]=v[2];
	}
	inline void Get(double &x,double &y,double &z) const
	{
		x=value[0];
		y=value[1];
		z=value[2];
	}
	inline double GetSquareLength(void) const
	{
		return value[0]*value[0]+value[1]*value[1]+value[2]*value[2];
	}
	inline double GetLength(void) const
	{
		return (double)sqrt(GetSquareLength());
	}
	inline double L0Norm(void) const
	{
		return YsGreater(YsGreater(fabs(value[0]),fabs(value[1])),fabs(value[2]));
	}
	YSBOOL IsNormalized(void) const;
	YSRESULT Normalize(void);

	inline YSBOOL IsZeroVector(void) const
	{
		if(GetSquareLength()<YsToleranceSqr)
		{
			return YSTRUE;
		}
		else
		{
			return YSFALSE;
		}
	}

	inline int GetLongestAxis(void) const
	{
		if(fabs(value[0])>fabs(value[1]) && fabs(value[0])>fabs(value[2]))
		{
			return 0;
		}
		else if(fabs(value[1])>fabs(value[2]))
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	inline double GetLongestDimension(void) const
	{
		if(fabs(value[0])>fabs(value[1]) && fabs(value[0])>fabs(value[2]))
		{
			return fabs(value[0]);
		}
		else if(fabs(value[1])>fabs(value[2]))
		{
			return fabs(value[1]);
		}
		else
		{
			return fabs(value[2]);
		}
	}

	YsVec3 GetArbitraryPerpendicularVector(void) const;

	inline void RotateXY(const double &ang)
	{
		double c,s,xx,yy;
		c=cos(ang);
		s=sin(ang);
		xx=x()*c-y()*s;
		yy=x()*s+y()*c;
		SetX(xx);
		SetY(yy);
	}
	inline void RotateYX(const double &ang)
	{
		RotateXY(-ang);
	}
	inline void RotateXZ(const double &ang)
	{
		double c,s,xx,zz;
		c=cos(ang);
		s=sin(ang);
		xx=x()*c-z()*s;
		zz=x()*s+z()*c;
		SetX(xx);
		SetZ(zz);
	}
	inline void RotateZX(const double &ang)
	{
		RotateXZ(-ang);
	}
	inline void RotateYZ(const double &ang)
	{
		double c,s,yy,zz;
		c=cos(ang);
		s=sin(ang);
		yy=y()*c-z()*s;
		zz=y()*s+z()*c;
		SetY(yy);
		SetZ(zz);
	}
	inline void RotateZY(const double &ang)
	{
		RotateYZ(-ang);
	}

	inline const YsVec3 &operator+=(const YsVec3 &a)
	{
		value[0]+=a.value[0];
		value[1]+=a.value[1];
		value[2]+=a.value[2];
		return *this;
	}
	inline const YsVec3 &operator-=(const YsVec3 &a)
	{
		value[0]-=a.value[0];
		value[1]-=a.value[1];
		value[2]-=a.value[2];
		return *this;
	}
	inline const YsVec3 &operator/=(double a)
	{
		value[0]/=a;
		value[1]/=a;
		value[2]/=a;
		return *this;
	}
	inline const YsVec3 &operator*=(double a)
	{
		value[0]*=a;
		value[1]*=a;
		value[2]*=a;
		return *this;
	}

protected:
	double value[3];
};

inline const YsVec3 operator-(const YsVec3 &a)
{
	return YsVec3(-a.x(),-a.y(),-a.z());
}

inline const YsVec3 operator-(const YsVec3 &a,const YsVec3 &b)
{
	return YsVec3(a.x()-b.x(),a.y()-b.y(),a.z()-b.z());
}

inline const YsVec3 operator+(const YsVec3 &a,const YsVec3 &b)
{
	return YsVec3(a.x()+b.x(),a.y()+b.y(),a.z()+b.z());
}

inline const YsVec3 operator/(const YsVec3 &a,const double &b)
{
	return YsVec3(a.x()/b,a.y()/b,a.z()/b);
}

inline const YsVec3 operator*(const YsVec3 &a,const double &b)
{
	return YsVec3(a.x()*b,a.y()*b,a.z()*b);
}

inline const YsVec3 operator*(const double &b,const YsVec3 &a)
{
	return YsVec3(a.x()*b,a.y()*b,a.z()*b);
}

inline double operator*(const YsVec3 &a,const YsVec3 &b)
{
	return a.x()*b.x()+a.y()*b.y()+a.z()*b.z();
}

inline const YsVec3 operator^(const YsVec3 &a,const YsVec3 &b)
{
	// y1 z1 x1 y1
	// y2 z2 x2 y2
	double x,y,z;

	x=a.y()*b.z()-a.z()*b.y();
	y=a.z()*b.x()-a.x()*b.z();
	z=a.x()*b.y()-a.y()*b.x();
	return YsVec3(x,y,z);
}

inline int operator==(const YsVec3 &a,const YsVec3 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()<YsTolerance);

	// Modified again 2000/11/10
	//if(YsEqual(a.x(),b.x())==YSTRUE &&
	//   YsEqual(a.y(),b.y())==YSTRUE &&
	//   YsEqual(a.z(),b.z())==YSTRUE)
	//{
	//	return 1;
	//}
	//else
	//{
	//	return 0;
	//}

	return ((a-b).GetSquareLength()<YsSqr(YsTolerance));  // Modified 2000/11/10
}

inline int operator!=(const YsVec3 &a,const YsVec3 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()>=YsTolerance);

	// Modified again 2000/11/10
	//if(YsEqual(a.x(),b.x())!=YSTRUE ||
	//   YsEqual(a.y(),b.y())!=YSTRUE ||
	//   YsEqual(a.z(),b.z())!=YSTRUE)
	//{
	//	return 1;
	//}
	//else
	//{
	//	return 0;
	//}

	return ((a-b).GetSquareLength()>=YsSqr(YsTolerance));  // Modified 2000/11/10
}


inline const YsVec3 &YsLonger(const YsVec3 &v1,const YsVec3 &v2)
{
	return (v1.GetSquareLength()>=v2.GetSquareLength() ? v1 : v2);
}


////////////////////////////////////////////////////////////
// Declaration /////////////////////////////////////////////
class YsRotation
{
public:
	inline YsRotation(){};
	YsRotation(double x,double y,double z,double a);
	YsRotation(const YsVec3 &ax,double a);

	inline void Set(double x,double y,double z,double a)
	{
		axis.Set(x,y,z);
		axis.Normalize();
		angle=a;
	}
	inline void Get(double &x,double &y,double &z,double &a) const
	{
		x=axis.x();
		y=axis.y();
		z=axis.z();
		a=angle;
	}
	inline void Set(const YsVec3 &vec,double a)
	{
		axis=vec;
		axis.Normalize();
		angle=a;
	}
	inline void SetUnit(const YsVec3 &vec,double a)
	{
		axis=vec;
		angle=a;
	}
	inline void Get(YsVec3 &vec,double &a) const
	{
		vec=axis;
		a=angle;
	}
	inline const YsVec3 &GetAxis(void) const
	{
		return axis;
	}
	inline const double &GetAngle(void) const
	{
		return angle;
	}

	inline void SetAxis(const YsVec3 &axs)
	{
		axis=axs;
		axis.Normalize();
	}
	inline void SetUnitAxis(const YsVec3 &axs)
	{
		axis=axs;
	}

	inline void SetAngle(double a)
	{
		angle=a;
	}

	YSRESULT RotatePositive(YsVec3 &result,const YsVec3 &start) const;
	YSRESULT RotateNegative(YsVec3 &result,const YsVec3 &start) const;
protected:
	double angle;
	YsVec3 axis;
};

inline YsVec3 operator*(const YsRotation &rot,const YsVec3 &vec)
{
	YsVec3 r;
	rot.RotatePositive(r,vec);
	return r;
}

////////////////////////////////////////////////////////////

class YsMatrix3x3 : public YsMatrixTemplate <3,3>  // Body is written in ysmatrix3x3.cpp
{
public:
	inline const double &v11(void) const;
	inline const double &v12(void) const;
	inline const double &v13(void) const;
	inline const double &v21(void) const;
	inline const double &v22(void) const;
	inline const double &v23(void) const;
	inline const double &v31(void) const;
	inline const double &v32(void) const;
	inline const double &v33(void) const;

	inline void Set11(const double &v);
	inline void Set12(const double &v);
	inline void Set13(const double &v);
	inline void Set21(const double &v);
	inline void Set22(const double &v);
	inline void Set23(const double &v);
	inline void Set31(const double &v);
	inline void Set32(const double &v);
	inline void Set33(const double &v);

	inline YsMatrix3x3(YSBOOL autoInit=YSTRUE)
	{
		if(autoInit==YSTRUE)
		{
			Initialize();
		}
	}
	inline YsMatrix3x3(const YsMatrixTemplate <3,3> &from)
	{
		Create(from.GetArray());
	}

	inline const YsMatrix3x3 &operator*=(const YsMatrixTemplate <3,3> &mul)
	{
		YsMatrixTemplate <3,3> org;
		org.Create(dat);
		dat[0]=org.dat[0]*mul.dat[0]+org.dat[1]*mul.dat[3]+org.dat[2]*mul.dat[6];
		dat[1]=org.dat[0]*mul.dat[1]+org.dat[1]*mul.dat[4]+org.dat[2]*mul.dat[7];
		dat[2]=org.dat[0]*mul.dat[2]+org.dat[1]*mul.dat[5]+org.dat[2]*mul.dat[8];

		dat[3]=org.dat[3]*mul.dat[0]+org.dat[4]*mul.dat[3]+org.dat[5]*mul.dat[6];
		dat[4]=org.dat[3]*mul.dat[1]+org.dat[4]*mul.dat[4]+org.dat[5]*mul.dat[7];
		dat[5]=org.dat[3]*mul.dat[2]+org.dat[4]*mul.dat[5]+org.dat[5]*mul.dat[8];

		dat[6]=org.dat[6]*mul.dat[0]+org.dat[7]*mul.dat[3]+org.dat[8]*mul.dat[6];
		dat[7]=org.dat[6]*mul.dat[1]+org.dat[7]*mul.dat[4]+org.dat[8]*mul.dat[7];
		dat[8]=org.dat[6]*mul.dat[2]+org.dat[7]*mul.dat[5]+org.dat[8]*mul.dat[8];

		return *this;
	}

	inline void CreateScaling(const double &sx,const double &sy,const double &sz)
	{
		Set11(sx);
		Set12(0.0);
		Set13(0.0);

		Set21(0.0);
		Set22(sy);
		Set23(0.0);

		Set31(0.0);
		Set32(0.0);
		Set33(sz);
	}

	const YsMatrix3x3 &operator*=(const class YsAtt3 &att);

	const YsMatrix3x3 &operator*=(const YsRotation &rot)
	{
		Rotate(rot);
		return *this;
	}

	inline const YsMatrix3x3 &operator=(const YsMatrixTemplate <3,3> &from)
	{
		Create(from.GetArray());
		return *this;
	}

	inline void Initialize(void)
	{
		LoadIdentity();
	}

	inline void GetOpenGlCompatibleMatrix(double mat[16]) const
	{
		mat[ 0]=dat[0];
		mat[ 1]=dat[3];
		mat[ 2]=dat[6];
		mat[ 3]=0.0;

		mat[ 4]=dat[1];
		mat[ 5]=dat[4];
		mat[ 6]=dat[7];
		mat[ 7]=0.0;

		mat[ 8]=dat[2];
		mat[ 9]=dat[5];
		mat[10]=dat[8];
		mat[11]=0.0;

		mat[12]=0.0;
		mat[13]=0.0;
		mat[14]=0.0;
		mat[15]=1.0;
	}

	// See document/mat2rot.jpg
	inline YSRESULT GetRotation(double &x,double &y,double &z,double &rad) const
	{
		double a,b,c;
		double d,e,f;
		double h,i,j;

		a=dat[0];
		b=dat[1];
		c=dat[2];
		d=dat[3];
		e=dat[4];
		f=dat[5];
		h=dat[6];
		i=dat[7];
		j=dat[8];

		YsVec3 v1,v2,v3,v;
		double l1,l2,l3,l;
		v1.Set(a-1,b,c);
		v2.Set(d,e-1,f);
		v3.Set(h,i,j-1);

		l1=v1.GetSquareLength();
		l2=v2.GetSquareLength();
		l3=v3.GetSquareLength();

		if(l1>l2 && l1>l3)
		{
			v=v1;
			l=l1;
		}
		else if(l2>l3)
		{
			v=v2;
			l=l2;
		}
		else
		{
			v=v3;
			l=l3;
		}

		if(l<=YsTolerance*YsTolerance)
		{
			x=0.0;
			y=1.0;
			z=0.0;
			rad=0.0;
			return YSOK;
		}

		v.Normalize();

		YsVec3 u;
		u=v;
		Mul(u,v);
		u.Normalize();

		double inner;
		YsVec3 outer;
		inner=v*u;
		outer=v^u;
		outer.Normalize();

		rad=acos(inner);
		outer.Get(x,y,z);
		return YSOK;
	}

	inline YSRESULT GetRotation(YsRotation &rot) const
	{
		double x,y,z,rad;
		if(GetRotation(x,y,z,rad)==YSOK)
		{
			rot.Set(x,y,z,rad);
		}
		return YSERR;
	}

	inline void RotateZY(const double &a)
	{
		double s,c;
		s=sin(a);
		c=cos(a);

		register double m1,m2;
		m1=dat[1];
		m2=dat[2];
		Set12( c*m1-s*m2);
		Set13( s*m1+c*m2);
		m1=dat[4];
		m2=dat[5];
		Set22( c*m1-s*m2);
		Set23( s*m1+c*m2);
		m1=dat[7];
		m2=dat[8];
		Set32( c*m1-s*m2);
		Set33( s*m1+c*m2);
	}
	inline void RotateYZ(const double &a)
	{
		RotateZY(-a);
	}

	void RotateXZ(const double &a)
	{
		// XZ Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		register double m1,m2;
		m1=dat[0];
		m2=dat[2];
		Set11( c*m1+s*m2);
		Set13(-s*m1+c*m2);
		m1=dat[3];
		m2=dat[5];
		Set21( c*m1+s*m2);
		Set23(-s*m1+c*m2);
		m1=dat[6];
		m2=dat[8];
		Set31( c*m1+s*m2);
		Set33(-s*m1+c*m2);
	}
	void RotateZX(const double &a)
	{
		RotateXZ(-a);
	}

	void RotateXY(const double &a)
	{
		// XY Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		register double m1,m2;
		m1=dat[0];
		m2=dat[1];
		Set11( c*m1+s*m2);
		Set12(-s*m1+c*m2);
		m1=dat[3];
		m2=dat[4];
		Set21( c*m1+s*m2);
		Set22(-s*m1+c*m2);
		m1=dat[6];
		m2=dat[7];
		Set31( c*m1+s*m2);
		Set32(-s*m1+c*m2);
	}
	void RotateYX(const double a)
	{
		RotateXY(-a);
	}

	inline void Rotate(const double &x,const double &y,const double &z,const double &a)
	{
		double l;
		l=x*x+y*y+z*z;
		if(l==0.0)
		{
			YsErrOut("YsMatrix3x3::Rotate\n  Zero Vector is specified for Rotation Axis\n");
			return;
		}

		double hdg,pch;

		if(l<=0.99F || 1.01F<=l)
		{
			l=sqrt(l);
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x/l,z/l) : 0.0);
			pch=asin(y/l);
		}
		else
		{
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x,z) : 0.0);
			pch=asin(y);
		}

		RotateXZ(hdg);
		RotateZY(pch);
		RotateXY(a);
		RotateZY(-pch);
		RotateXZ(-hdg);   //
	}

	void Rotate(const class YsRotation &rot);
	void Rotate(const class YsAtt3 &att);

	inline void Scale(const double &x,const double &y,const double &z)
	{
		dat[0]*=x;
		dat[3]*=x;
		dat[6]*=x;

		dat[1]*=y;
		dat[4]*=y;
		dat[7]*=y;

		dat[2]*=z;
		dat[5]*=z;
		dat[8]*=z;
	}

	inline void Scale(const YsVec3 &sca)
	{
		dat[0]*=sca.x();
		dat[3]*=sca.x();
		dat[6]*=sca.x();

		dat[1]*=sca.y();
		dat[4]*=sca.y();
		dat[7]*=sca.y();

		dat[2]*=sca.z();
		dat[5]*=sca.z();
		dat[8]*=sca.z();
	}

	inline YSRESULT Mul(YsVec3 &dst,const YsVec3 &src) const
	{
		double x,y,z;
		x=dat[0]*src.x()+dat[1]*src.y()+dat[2]*src.z();
		y=dat[3]*src.x()+dat[4]*src.y()+dat[5]*src.z();
		z=dat[6]*src.x()+dat[7]*src.y()+dat[8]*src.z();
		dst.Set(x,y,z);
		return YSOK;
	}
	inline YSRESULT MulInverse(YsVec3 &dst,const YsVec3 &src) const
	{
		return YsMulInverse3x3(&dst[0],GetArray(),src);
	}
	YSRESULT MulTranspose(YsVec3 &dst,const YsVec3 &src) const;  // <- Written in ysmatrix3x3.cpp
	YSRESULT MakeToXY(const YsVec3 &zDir);
};


inline const double &YsMatrix3x3::v11(void) const
{
	return dat[0];
}

inline const double &YsMatrix3x3::v12(void) const
{
	return dat[1];
}

inline const double &YsMatrix3x3::v13(void) const
{
	return dat[2];
}

inline const double &YsMatrix3x3::v21(void) const
{
	return dat[3];
}

inline const double &YsMatrix3x3::v22(void) const
{
	return dat[4];
}

inline const double &YsMatrix3x3::v23(void) const
{
	return dat[5];
}

inline const double &YsMatrix3x3::v31(void) const
{
	return dat[6];
}

inline const double &YsMatrix3x3::v32(void) const
{
	return dat[7];
}

inline const double &YsMatrix3x3::v33(void) const
{
	return dat[8];
}

inline void YsMatrix3x3::Set11(const double &v)
{
	dat[0]=v;
}

inline void YsMatrix3x3::Set12(const double &v)
{
	dat[1]=v;
}

inline void YsMatrix3x3::Set13(const double &v)
{
	dat[2]=v;
}

inline void YsMatrix3x3::Set21(const double &v)
{
	dat[3]=v;
}

inline void YsMatrix3x3::Set22(const double &v)
{
	dat[4]=v;
}

inline void YsMatrix3x3::Set23(const double &v)
{
	dat[5]=v;
}

inline void YsMatrix3x3::Set31(const double &v)
{
	dat[6]=v;
}

inline void YsMatrix3x3::Set32(const double &v)
{
	dat[7]=v;
}

inline void YsMatrix3x3::Set33(const double &v)
{
	dat[8]=v;
}


inline const YsVec3 operator*(const YsMatrix3x3 &a,const YsVec3 &b)
{
	YsVec3 c;
	a.Mul(c,b);
	return c;
}

inline const YsMatrix3x3 operator*(const YsMatrix3x3 &mat,const YsRotation &rot)
{
	YsMatrix3x3 r(mat);
	r.Rotate(rot);
	return r;
}

inline const YsMatrix3x3 operator*(const YsRotation &rot,const YsMatrix3x3 &mat)
{
	YsMatrix3x3 r;
	r.Rotate(rot);
	return r*mat;
}

////////////////////////////////////////////////////////////

class YsMatrix4x4 : public YsMatrixTemplate <4,4> // body is written in ysmatrix4x4.cpp
{
public:
	using YsMatrixTemplate <4,4>::Set;

	inline YsMatrix4x4(YSBOOL autoInit=YSTRUE)
	{
		if(autoInit==YSTRUE)
		{
			Initialize();
		}
	}

	inline YsMatrix4x4(const YsMatrixTemplate <4,4> &from)
	{
		Create(from.GetArray());
	}


	inline const double &v11(void) const;
	inline const double &v12(void) const;
	inline const double &v13(void) const;
	inline const double &v14(void) const;
	inline const double &v21(void) const;
	inline const double &v22(void) const;
	inline const double &v23(void) const;
	inline const double &v24(void) const;
	inline const double &v31(void) const;
	inline const double &v32(void) const;
	inline const double &v33(void) const;
	inline const double &v34(void) const;
	inline const double &v41(void) const;
	inline const double &v42(void) const;
	inline const double &v43(void) const;
	inline const double &v44(void) const;

	inline void Set11(const double &v);
	inline void Set12(const double &v);
	inline void Set13(const double &v);
	inline void Set14(const double &v);
	inline void Set21(const double &v);
	inline void Set22(const double &v);
	inline void Set23(const double &v);
	inline void Set24(const double &v);
	inline void Set31(const double &v);
	inline void Set32(const double &v);
	inline void Set33(const double &v);
	inline void Set34(const double &v);
	inline void Set41(const double &v);
	inline void Set42(const double &v);
	inline void Set43(const double &v);
	inline void Set44(const double &v);


	inline const YsMatrix4x4 &operator*=(const YsMatrixTemplate <4,4> &mul)
	{
		YsMatrixTemplate <4,4> org;
		org.Create(dat);
		dat[ 0]=org.dat[ 0]*mul.dat[ 0]+org.dat[ 1]*mul.dat[ 4]+org.dat[ 2]*mul.dat[ 8]+org.dat[ 3]*mul.dat[12];
		dat[ 1]=org.dat[ 0]*mul.dat[ 1]+org.dat[ 1]*mul.dat[ 5]+org.dat[ 2]*mul.dat[ 9]+org.dat[ 3]*mul.dat[13];
		dat[ 2]=org.dat[ 0]*mul.dat[ 2]+org.dat[ 1]*mul.dat[ 6]+org.dat[ 2]*mul.dat[10]+org.dat[ 3]*mul.dat[14];
		dat[ 3]=org.dat[ 0]*mul.dat[ 3]+org.dat[ 1]*mul.dat[ 7]+org.dat[ 2]*mul.dat[11]+org.dat[ 3]*mul.dat[15];

		dat[ 4]=org.dat[ 4]*mul.dat[ 0]+org.dat[ 5]*mul.dat[ 4]+org.dat[ 6]*mul.dat[ 8]+org.dat[ 7]*mul.dat[12];
		dat[ 5]=org.dat[ 4]*mul.dat[ 1]+org.dat[ 5]*mul.dat[ 5]+org.dat[ 6]*mul.dat[ 9]+org.dat[ 7]*mul.dat[13];
		dat[ 6]=org.dat[ 4]*mul.dat[ 2]+org.dat[ 5]*mul.dat[ 6]+org.dat[ 6]*mul.dat[10]+org.dat[ 7]*mul.dat[14];
		dat[ 7]=org.dat[ 4]*mul.dat[ 3]+org.dat[ 5]*mul.dat[ 7]+org.dat[ 6]*mul.dat[11]+org.dat[ 7]*mul.dat[15];

		dat[ 8]=org.dat[ 8]*mul.dat[ 0]+org.dat[ 9]*mul.dat[ 4]+org.dat[10]*mul.dat[ 8]+org.dat[11]*mul.dat[12];
		dat[ 9]=org.dat[ 8]*mul.dat[ 1]+org.dat[ 9]*mul.dat[ 5]+org.dat[10]*mul.dat[ 9]+org.dat[11]*mul.dat[13];
		dat[10]=org.dat[ 8]*mul.dat[ 2]+org.dat[ 9]*mul.dat[ 6]+org.dat[10]*mul.dat[10]+org.dat[11]*mul.dat[14];
		dat[11]=org.dat[ 8]*mul.dat[ 3]+org.dat[ 9]*mul.dat[ 7]+org.dat[10]*mul.dat[11]+org.dat[11]*mul.dat[15];

		dat[12]=org.dat[12]*mul.dat[ 0]+org.dat[13]*mul.dat[ 4]+org.dat[14]*mul.dat[ 8]+org.dat[15]*mul.dat[12];
		dat[13]=org.dat[12]*mul.dat[ 1]+org.dat[13]*mul.dat[ 5]+org.dat[14]*mul.dat[ 9]+org.dat[15]*mul.dat[13];
		dat[14]=org.dat[12]*mul.dat[ 2]+org.dat[13]*mul.dat[ 6]+org.dat[14]*mul.dat[10]+org.dat[15]*mul.dat[14];
		dat[15]=org.dat[12]*mul.dat[ 3]+org.dat[13]*mul.dat[ 7]+org.dat[14]*mul.dat[11]+org.dat[15]*mul.dat[15];

		return *this;
	}

	inline void CreateScaling(const double &sx,const double &sy,const double &sz)
	{
		Set11(sx);
		Set12(0.0);
		Set13(0.0);
		Set14(0.0);

		Set21(0.0);
		Set22(sy);
		Set23(0.0);
		Set24(0.0);

		Set31(0.0);
		Set32(0.0);
		Set33(sz);
		Set34(0.0);

		Set41(0.0);
		Set42(0.0);
		Set43(0.0);
		Set44(1.0);
	}

	inline void CreateTranslation(const double &dx,const double &dy,const double &dz)
	{
		Set11(1.0);
		Set12(0.0);
		Set13(0.0);
		Set14(0.0);

		Set21(0.0);
		Set22(1.0);
		Set23(0.0);
		Set24(0.0);

		Set31(0.0);
		Set32(0.0);
		Set33(1.0);
		Set34(0.0);

		Set41(dx);
		Set42(dy);
		Set43(dz);
		Set44(1.0);
	}

	inline void CreateTranslation(const YsVec3 &d)
	{
		CreateTranslation(d.x(),d.y(),d.z());
	}

	inline const YsMatrix4x4 &operator=(const YsMatrixTemplate <4,4> &from)
	{
		Create(from.GetArray());
		return *this;
	}

	inline const YsMatrix4x4 &operator=(const YsMatrixTemplate <3,3> &from)
	{
		Set11(from.dat[0]);
		Set12(from.dat[1]);
		Set13(from.dat[2]);

		Set21(from.dat[3]);
		Set22(from.dat[4]);
		Set23(from.dat[5]);

		Set31(from.dat[6]);
		Set32(from.dat[7]);
		Set33(from.dat[8]);

		Set14(0.0);
		Set24(0.0);
		Set34(0.0);

		Set41(0.0);
		Set42(0.0);
		Set43(0.0);

		Set44(1.0);

		return *this;
	}

	const YsMatrix4x4 &operator=(const YsMatrix &mat);
	const YsMatrix4x4 &operator*=(const class YsAtt3 &att);

	const YsMatrix4x4 &operator*=(const YsRotation &rot)
	{
		Rotate(rot);
		return *this;
	}

	inline void Initialize(void)
	{
		LoadIdentity();
	}

	inline void RotateZY(const double &a)
	{
		// ZY Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		//  m11 m12 m13 m14     1     0     0    0
		//  m21 m22 m23 m24  x  0     c     s    0
		//  m31 m32 m33 m34     0    -s     c    0
		//  m41 m42 m43 m44     0     0     0    1
		// Only 2nd and 3rd column changes
		register double m1,m2;
		m1=dat[ 1];
		m2=dat[ 2];
		Set12( c*m1-s*m2);
		Set13( s*m1+c*m2);
		m1=dat[ 5];
		m2=dat[ 6];
		Set22( c*m1-s*m2);
		Set23( s*m1+c*m2);
		m1=dat[ 9];
		m2=dat[10];
		Set32( c*m1-s*m2);
		Set33( s*m1+c*m2);
		m1=dat[13];
		m2=dat[14];
		Set42( c*m1-s*m2);
		Set43( s*m1+c*m2);
	}
	inline void RotateYZ(const double &a)
	{
		RotateZY(-a);
	}

	inline void RotateXZ(const double &a)
	{
		// XZ Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		//  m11 m12 m13 m14     c     0    -s    0
		//  m21 m22 m23 m24  x  0     1     0    0
		//  m31 m32 m33 m34     s     0     c    0
		//  m41 m42 m43 m44     0     0     0    1
		// Only 1st and 3rd column changes
		register double m1,m2;
		m1=dat[ 0];
		m2=dat[ 2];
		Set11( c*m1+s*m2);
		Set13(-s*m1+c*m2);
		m1=dat[ 4];
		m2=dat[ 6];
		Set21( c*m1+s*m2);
		Set23(-s*m1+c*m2);
		m1=dat[ 8];
		m2=dat[10];
		Set31( c*m1+s*m2);
		Set33(-s*m1+c*m2);
		m1=dat[12];
		m2=dat[14];
		Set41( c*m1+s*m2);
		Set43(-s*m1+c*m2);
	}
	inline void RotateZX(const double &a)
	{
		RotateXZ(-a);
	}

	inline void RotateXY(const double &a)
	{
		// XY Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		//  m11 m12 m13 m14     c    -s     0    0
		//  m21 m22 m23 m24  x  s     c     0    0
		//  m31 m32 m33 m34     0     0     1    0
		//  m41 m42 m43 m44     0     0     0    1
		// Only 1st and 2nd column changes
		register double m1,m2;
		m1=dat[ 0];
		m2=dat[ 1];
		Set11( c*m1+s*m2);
		Set12(-s*m1+c*m2);
		m1=dat[ 4];
		m2=dat[ 5];
		Set21( c*m1+s*m2);
		Set22(-s*m1+c*m2);
		m1=dat[ 8];
		m2=dat[ 9];
		Set31( c*m1+s*m2);
		Set32(-s*m1+c*m2);
		m1=dat[12];
		m2=dat[13];
		Set41( c*m1+s*m2);
		Set42(-s*m1+c*m2);
	}
	inline void RotateYX(const double &a)
	{
		RotateXY(-a);
	}

	inline void Rotate(const double &x,const double &y,const double &z,const double &a)
	{
		double l;
		l=x*x+y*y+z*z;
		if(l==0.0)
		{
			YsErrOut("YsMatrix4x4::Rotate\n  Zero Vector is specified for Rotation Axis\n");
			return;
		}

		double hdg,pch;

		if(l<=0.99F || 1.01F<=l)
		{
			l=sqrt(l);
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x/l,z/l) : 0.0);
			pch=asin(y/l);
		}
		else
		{
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x,z) : 0.0);
			pch=asin(y);
		}

		RotateXZ(hdg);
		RotateZY(pch);
		RotateXY(a);
		RotateZY(-pch);
		RotateXZ(-hdg);   //
	}

	void Rotate(const class YsRotation &rot);
	void Rotate(const class YsAtt3 &att);

	inline void Translate(const double &x,const double &y,const double &z)
	{
		//  m11 m12 m13 m14     1     0     0    x
		//  m21 m22 m23 m24  x  0     1     0    y
		//  m31 m32 m33 m34     0     0     1    z
		//  m41 m42 m43 m44     0     0     0    1
		Set14(dat[ 0]*x+dat[ 1]*y+dat[ 2]*z+dat[ 3]);
		Set24(dat[ 4]*x+dat[ 5]*y+dat[ 6]*z+dat[ 7]);
		Set34(dat[ 8]*x+dat[ 9]*y+dat[10]*z+dat[11]);
		Set44(dat[12]*x+dat[13]*y+dat[14]*z+dat[15]);
	}

	inline void Translate(const YsVec3 &vec)
	{
		Translate(vec.x(),vec.y(),vec.z());
	}

	inline void Scale(const double &x,const double &y,const double &z)
	{
		dat[ 0]*=x;
		dat[ 4]*=x;
		dat[ 8]*=x;
		dat[12]*=x;
		dat[ 1]*=y;
		dat[ 5]*=y;
		dat[ 9]*=y;
		dat[13]*=y;
		dat[ 2]*=z;
		dat[ 6]*=z;
		dat[10]*=z;
		dat[14]*=z;
	}

	inline void Scale(const YsVec3 &sca)
	{
		Scale(sca.x(),sca.y(),sca.z());
	}

	inline void Mul(double result[4],const double vec[4]) const
	{
		double x,y,z,w;
		x=dat[ 0]*vec[0]+dat[ 1]*vec[1]+dat[ 2]*vec[2]+dat[ 3]*vec[3];
		y=dat[ 4]*vec[0]+dat[ 5]*vec[1]+dat[ 6]*vec[2]+dat[ 7]*vec[3];
		z=dat[ 8]*vec[0]+dat[ 9]*vec[1]+dat[10]*vec[2]+dat[11]*vec[3];
		w=dat[12]*vec[0]+dat[13]*vec[1]+dat[14]*vec[2]+dat[15]*vec[3];
		result[0]=x;
		result[1]=y;
		result[2]=z;
		result[3]=w;
	}

	inline void Mul(YsVec3 &result,const YsVec3 &org,const double &translate) const
	{
		double x,y,z;
		if(YsAbs(translate)<YsTolerance)
		{
			x=dat[ 0]*org.x()+dat[ 1]*org.y()+dat[ 2]*org.z();
			y=dat[ 4]*org.x()+dat[ 5]*org.y()+dat[ 6]*org.z();
			z=dat[ 8]*org.x()+dat[ 9]*org.y()+dat[10]*org.z();
		}
		else
		{
			x=dat[ 0]*org.x()+dat[ 1]*org.y()+dat[ 2]*org.z()+dat[ 3]*translate;
			y=dat[ 4]*org.x()+dat[ 5]*org.y()+dat[ 6]*org.z()+dat[ 7]*translate;
			z=dat[ 8]*org.x()+dat[ 9]*org.y()+dat[10]*org.z()+dat[11]*translate;
		}
		result.Set(x,y,z);
	}

	inline void MulInverse(double result[4],const double vec[4]) const
	{
		YsMulInverse4x4(result,dat,vec);
	}

	inline void MulInverse(YsVec3 &result,const YsVec3 &org,const double &translate) const
	{
		if(YsAbs(translate)<YsTolerance)
		{
			YsMatrix3x3 mat3(YSFALSE);
			mat3.Set11(dat[ 0]);
			mat3.Set12(dat[ 1]);
			mat3.Set13(dat[ 2]);
			mat3.Set21(dat[ 4]);
			mat3.Set22(dat[ 5]);
			mat3.Set23(dat[ 6]);
			mat3.Set31(dat[ 8]);
			mat3.Set32(dat[ 9]);
			mat3.Set33(dat[10]);
			mat3.MulInverse(result,org);
		}
		else
		{
			double inv[4],src[4];
			src[0]=org.x();
			src[1]=org.y();
			src[2]=org.z();
			src[3]=translate;
			YsMulInverse4x4(inv,dat,src);
			result.Set(inv[0],inv[1],inv[2]);
		}
	}

	inline void GetArray(double mat[16]) const
	{
		int i;
		for(i=0; i<16; i++)
		{
			mat[i]=dat[i];
		}
	}

	inline YSRESULT GetInverseArray(double mat[16]) const
	{
		return YsInvertMatrix4x4(mat,dat);
	}

	inline const double *GetArray(void) const
	{
		return dat;
	}

	inline void GetOpenGlCompatibleMatrix(double mat[16]) const
	{
		mat[ 0]=dat[ 0];
		mat[ 1]=dat[ 4];
		mat[ 2]=dat[ 8];
		mat[ 3]=dat[12];

		mat[ 4]=dat[ 1];
		mat[ 5]=dat[ 5];
		mat[ 6]=dat[ 9];
		mat[ 7]=dat[13];

		mat[ 8]=dat[ 2];
		mat[ 9]=dat[ 6];
		mat[10]=dat[10];
		mat[11]=dat[14];

		mat[12]=dat[ 3];
		mat[13]=dat[ 7];
		mat[14]=dat[11];
		mat[15]=dat[15];
	}

	// See document/mat2rot.jpg
	inline YSRESULT GetRotation(double &x,double &y,double &z,double &rad) const
	{
		double a,b,c;
		double d,e,f;
		double h,i,j;

		a=dat[ 0];
		b=dat[ 1];
		c=dat[ 2];
		d=dat[ 4];
		e=dat[ 5];
		f=dat[ 6];
		h=dat[ 8];
		i=dat[ 9];
		j=dat[10];

		YsVec3 v1,v2,v3,v;
		double l1,l2,l3,l;
		v1.Set(a-1,b,c);
		v2.Set(d,e-1,f);
		v3.Set(h,i,j-1);

		l1=v1.GetSquareLength();
		l2=v2.GetSquareLength();
		l3=v3.GetSquareLength();

		if(l1>l2 && l1>l3)
		{
			v=v1;
			l=l1;
		}
		else if(l2>l3)
		{
			v=v2;
			l=l2;
		}
		else
		{
			v=v3;
			l=l3;
		}

		if(l<=YsTolerance*YsTolerance)
		{
			x=0.0;
			y=1.0;
			z=0.0;
			rad=0.0;
			return YSOK;
		}

		v.Normalize();

		YsVec3 u;
		u=v;
		Mul(u,v,0.0);
		u.Normalize();

		double inner;
		YsVec3 outer;
		inner=v*u;
		outer=v^u;
		outer.Normalize();

		rad=acos(inner);
		outer.Get(x,y,z);
		return YSOK;
	}

	inline YSRESULT GetRotation(YsRotation &rot) const
	{
		double x,y,z,rad;
		if(GetRotation(x,y,z,rad)==YSOK)
		{
			rot.Set(x,y,z,rad);
		}
		return YSERR;
	}


	inline YSRESULT GetTranslation(double &x,double &y,double &z) const
	{
		YsVec3 tmp;
		GetTranslation(tmp);
		x=tmp.x();
		y=tmp.y();
		z=tmp.z();
		return YSOK;
	}

	inline YSRESULT GetTranslation(YsVec3 &trs) const
	{
		YsVec3 tmp(0.0,0.0,0.0);
		Mul(tmp,tmp,1.0);
		trs=tmp;
		return YSOK;
	}

	YSRESULT MakeToXY(const YsVec3 &cen,const YsVec3 &zDir);

	void Set(const double from[16])
	{
		dat[ 0]=from[ 0];
		dat[ 1]=from[ 1];
		dat[ 2]=from[ 2];
		dat[ 3]=from[ 3];
		dat[ 4]=from[ 4];
		dat[ 5]=from[ 5];
		dat[ 6]=from[ 6];
		dat[ 7]=from[ 7];
		dat[ 8]=from[ 8];
		dat[ 9]=from[ 9];
		dat[10]=from[10];
		dat[11]=from[11];
		dat[12]=from[12];
		dat[13]=from[13];
		dat[14]=from[14];
		dat[15]=from[15];
	}
};

inline const double &YsMatrix4x4::v11(void) const
{
	return dat[0];
}

inline const double &YsMatrix4x4::v12(void) const
{
	return dat[1];
}

inline const double &YsMatrix4x4::v13(void) const
{
	return dat[2];
}

inline const double &YsMatrix4x4::v14(void) const
{
	return dat[3];
}

inline const double &YsMatrix4x4::v21(void) const
{
	return dat[4];
}

inline const double &YsMatrix4x4::v22(void) const
{
	return dat[5];
}

inline const double &YsMatrix4x4::v23(void) const
{
	return dat[6];
}

inline const double &YsMatrix4x4::v24(void) const
{
	return dat[7];
}

inline const double &YsMatrix4x4::v31(void) const
{
	return dat[8];
}

inline const double &YsMatrix4x4::v32(void) const
{
	return dat[9];
}

inline const double &YsMatrix4x4::v33(void) const
{
	return dat[10];
}

inline const double &YsMatrix4x4::v34(void) const
{
	return dat[11];
}

inline const double &YsMatrix4x4::v41(void) const
{
	return dat[12];
}

inline const double &YsMatrix4x4::v42(void) const
{
	return dat[13];
}

inline const double &YsMatrix4x4::v43(void) const
{
	return dat[14];
}

inline const double &YsMatrix4x4::v44(void) const
{
	return dat[15];
}

inline void YsMatrix4x4::Set11(const double &v)
{
	dat[0]=v;
}

inline void YsMatrix4x4::Set12(const double &v)
{
	dat[1]=v;
}

inline void YsMatrix4x4::Set13(const double &v)
{
	dat[2]=v;
}

inline void YsMatrix4x4::Set14(const double &v)
{
	dat[3]=v;
}

inline void YsMatrix4x4::Set21(const double &v)
{
	dat[4]=v;
}

inline void YsMatrix4x4::Set22(const double &v)
{
	dat[5]=v;
}

inline void YsMatrix4x4::Set23(const double &v)
{
	dat[6]=v;
}

inline void YsMatrix4x4::Set24(const double &v)
{
	dat[7]=v;
}

inline void YsMatrix4x4::Set31(const double &v)
{
	dat[8]=v;
}

inline void YsMatrix4x4::Set32(const double &v)
{
	dat[9]=v;
}

inline void YsMatrix4x4::Set33(const double &v)
{
	dat[10]=v;
}

inline void YsMatrix4x4::Set34(const double &v)
{
	dat[11]=v;
}

inline void YsMatrix4x4::Set41(const double &v)
{
	dat[12]=v;
}

inline void YsMatrix4x4::Set42(const double &v)
{
	dat[13]=v;
}

inline void YsMatrix4x4::Set43(const double &v)
{
	dat[14]=v;
}

inline void YsMatrix4x4::Set44(const double &v)
{
	dat[15]=v;
}



inline const YsVec3 operator*(const YsMatrix4x4 &a,const YsVec3 &b)
{
	YsVec3 r;
	a.Mul(r,b,1.0);
	return r;
}

inline const YsMatrix4x4 operator*(const YsMatrix4x4 &a,const YsMatrix3x3 &b)
{
	YsMatrix4x4 mat;
	mat.Set11(a.v11()*b.v11()+a.v12()*b.v21()+a.v13()*b.v31());
	mat.Set12(a.v11()*b.v12()+a.v12()*b.v22()+a.v13()*b.v32());
	mat.Set13(a.v11()*b.v13()+a.v12()*b.v23()+a.v13()*b.v33());

	mat.Set21(a.v21()*b.v11()+a.v22()*b.v21()+a.v23()*b.v31());
	mat.Set22(a.v21()*b.v12()+a.v22()*b.v22()+a.v23()*b.v32());
	mat.Set23(a.v21()*b.v13()+a.v22()*b.v23()+a.v23()*b.v33());

	mat.Set31(a.v31()*b.v11()+a.v32()*b.v21()+a.v33()*b.v31());
	mat.Set32(a.v31()*b.v12()+a.v32()*b.v22()+a.v33()*b.v32());
	mat.Set33(a.v31()*b.v13()+a.v32()*b.v23()+a.v33()*b.v33());

	mat.Set41(a.v41());
	mat.Set42(a.v42());
	mat.Set43(a.v43());

	mat.Set14(a.v14());
	mat.Set24(a.v24());
	mat.Set34(a.v34());

	mat.Set44(a.v44());

	return mat;
}

inline const YsMatrix4x4 operator*(const YsMatrix3x3 &a,const YsMatrix4x4 &b)
{
	YsMatrix4x4 mat;
	mat.Set11(a.v11()*b.v11()+a.v12()*b.v21()+a.v13()*b.v31());
	mat.Set12(a.v11()*b.v12()+a.v12()*b.v22()+a.v13()*b.v32());
	mat.Set13(a.v11()*b.v13()+a.v12()*b.v23()+a.v13()*b.v33());

	mat.Set21(a.v21()*b.v11()+a.v22()*b.v21()+a.v23()*b.v31());
	mat.Set22(a.v21()*b.v12()+a.v22()*b.v22()+a.v23()*b.v32());
	mat.Set23(a.v21()*b.v13()+a.v22()*b.v23()+a.v23()*b.v33());

	mat.Set31(a.v31()*b.v11()+a.v32()*b.v21()+a.v33()*b.v31());
	mat.Set32(a.v31()*b.v12()+a.v32()*b.v22()+a.v33()*b.v32());
	mat.Set33(a.v31()*b.v13()+a.v32()*b.v23()+a.v33()*b.v33());

	mat.Set41(b.v41());
	mat.Set42(b.v42());
	mat.Set43(b.v43());

	mat.Set14(b.v14());
	mat.Set24(b.v24());
	mat.Set34(b.v34());

	mat.Set44(b.v44());

	return mat;
}

inline const YsMatrix4x4 operator*(const YsMatrix4x4 &mat,const YsRotation &rot)
{
	YsMatrix4x4 r(mat);
	r.Rotate(rot);
	return r;
}

inline const YsMatrix4x4 operator*(const YsRotation &rot,const YsMatrix4x4 &mat)
{
	YsMatrix4x4 r;
	r.Rotate(rot);
	return r*mat;
}

////////////////////////////////////////////////////////////

class YsAtt3
{
public:
	inline YsAtt3(){}
	YsAtt3(const double &h,const double &p,const double &b);

	inline double h(void) const
	{
		return hdg;
	}
	inline double p(void) const
	{
		return pch;
	}
	inline double b(void) const
	{
		return bnk;
	}
	void Set(const double &h,const double &p,const double &b);
	void SetH(const double &h);
	void SetP(const double &p);
	void SetB(const double &b);
	YsVec3 GetForwardVector(void) const;
	YsVec3 GetUpVector(void) const;
	YSRESULT SetTwoVector(const YsVec3 &fwd,const YsVec3 &up);
	YSRESULT SetForwardVector(YsVec3 vec);
	YSRESULT SetUpVector(YsVec3 vec);

	inline void Mul(YsVec3 &to,const YsVec3 &from) const
	{
		to=from;
		switch(YsCoordSysModel)
		{
		case YSBLUEIMPULSE:
			to.RotateXY(bnk);
			to.RotateYZ(-pch);  // RotateZY(pch);
			to.RotateXZ(hdg);
			break;
		case YSOPENGL:
			to.RotateXY(bnk);
			to.RotateYZ(pch);  // RotateZY(-pch);
			to.RotateXZ(-hdg);
			break;
		}
	}

	inline void MulInverse(YsVec3 &to,const YsVec3 &from) const
	{
		to=from;
		switch(YsCoordSysModel)
		{
		case YSBLUEIMPULSE:
			to.RotateXZ(-hdg);
			to.RotateYZ(pch);  // RotateZY(-pch);
			to.RotateXY(-bnk);
			break;
		case YSOPENGL:
			to.RotateXZ(hdg);
			to.RotateYZ(-pch);  // RotateZY(pch);
			to.RotateXY(-bnk);
			break;
		}
	}

	void NoseUp(const double &d);
	void YawLeft(const double &d);

	inline const YsMatrix4x4 &GetMatrix4x4(YsMatrix4x4 &mat) const
	{
		mat.Initialize();
		switch(YsCoordSysModel)
		{
		case YSBLUEIMPULSE:
			mat.RotateXZ(hdg);
			mat.RotateZY(pch);
			mat.RotateXY(bnk);
			break;
		case YSOPENGL:
			mat.RotateXZ(-hdg);
			mat.RotateZY(-pch);
			mat.RotateXY(bnk);
			break;
		}
		return mat;
	}

	inline const YsMatrix3x3 &GetMatrix3x3(YsMatrix3x3 &mat) const
	{
		mat.Initialize();
		switch(YsCoordSysModel)
		{
		case YSBLUEIMPULSE:
			mat.RotateXZ(hdg);
			mat.RotateZY(pch);
			mat.RotateXY(bnk);
			break;
		case YSOPENGL:
			mat.RotateXZ(-hdg);
			mat.RotateZY(-pch);
			mat.RotateXY(bnk);
			break;
		}
		return mat;
	}

protected:
	double hdg,pch,bnk;
};

inline YsVec3 operator*(const YsAtt3 &att,const YsVec3 &vec)
{
	YsVec3 r;
	att.Mul(r,vec);
	return r;
}

inline YsMatrix3x3 operator*(const YsMatrix3x3 &mat,const YsAtt3 &att)
{
	YsMatrix3x3 r(YSFALSE);
	r=mat;
	r.Rotate(att);
	return r;
}

inline YsMatrix3x3 operator*(const YsAtt3 &att,const YsMatrix3x3 &mat)
{
	YsMatrix3x3 r(YSFALSE);
	att.GetMatrix3x3(r);
	r=r*mat;
	return r;
}

inline YsMatrix4x4 operator*(const YsMatrix4x4 &mat,const YsAtt3 &att)
{
	YsMatrix4x4 r(YSFALSE);
	r=mat;
	r.Rotate(att);
	return r;
}

inline YsMatrix4x4 operator*(const YsAtt3 &att,const YsMatrix4x4 &mat)
{
	YsMatrix4x4 r(YSFALSE);
	att.GetMatrix4x4(r);
	r=r*mat;
	return r;
}

inline int operator==(const YsAtt3 &a,const YsAtt3 &b)
{
	return (YsEqual(a.h(),b.h())==YSTRUE && YsEqual(a.p(),b.p())==YSTRUE && YsEqual(a.b(),b.b())==YSTRUE);
}

inline int operator!=(const YsAtt3 &a,const YsAtt3 &b)
{
	return (YsEqual(a.h(),b.h())!=YSTRUE || YsEqual(a.p(),b.p())!=YSTRUE || YsEqual(a.b(),b.b())!=YSTRUE);
}

////////////////////////////////////////////////////////////
// Declaration /////////////////////////////////////////////
class YsPlane
{
public:
	inline YsPlane(){}
	YsPlane(const YsVec3 &o,const YsVec3 &n);

	inline void Set(const YsVec3 &o,const YsVec3 &n)
	{
		org=o;
		nom=n;
		if(nom.IsNormalized()!=YSTRUE)
		{
			nom.Normalize();
		}
	}
	inline void SetOrigin(const YsVec3 &o)
	{
		org=o;
	}
	inline void SetNormal(const YsVec3 &n)
	{
		nom=n;
		if(nom.IsNormalized()!=YSTRUE)
		{
			nom.Normalize();
		}
	}

	inline void GetOrigin(YsVec3 &o) const
	{
		o=org;
	}
	inline void GetNormal(YsVec3 &n) const
	{
		n=nom;
	}

	inline const YsVec3 &GetOrigin(void) const
	{
		return org;
	}
	inline const YsVec3 &GetNormal(void) const
	{
		return nom;
	}

	YSBOOL CheckOnPlane(const YsVec3 &pos) const;
	YSRESULT GetIntersection
	    (YsVec3 &crs,const YsVec3 &o,const YsVec3 &v) const;
	YSRESULT GetPenetration
	    (YsVec3 &crs,const YsVec3 &p1,const YsVec3 &p2) const;
	YSRESULT GetNearestPoint
	    (YsVec3 &np,const YsVec3 &refp) const;
	YSRESULT MakeBestFitPlane(int np,const YsVec3 p[]);
	YSRESULT MakePlaneFromTriangle(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3);
	int GetSideOfPlane(const YsVec3 &ref) const;
	double GetDistance(const YsVec3 &from) const;
protected:
	YsVec3 org,nom;
};

////////////////////////////////////////////////////////////

/* } */
#endif
