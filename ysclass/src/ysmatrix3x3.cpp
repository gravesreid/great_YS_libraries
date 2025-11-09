#include "ysclass.h"

// This code is needed to cope with VC++'s stupid bug.
// VC++ cannot compile template parameters correctly when different
// template parameters are mixed in one .CPP file.

YSRESULT YsInvertMatrix3x3(double result[],const double from[])
{
	return YsInvertMatrixNxN <3> (result,from);
	// I want to call this template function directly, but VC++'s stupidity prevent me from doing so.
}

YSRESULT YsMulInverse3x3(double inv[],const double mat[],const double vec[])
{
	return YsMulInverseNxN <3> (inv,mat,vec);
	// I want to call this template function directly, but VC++'s stupidity prevent me from doing so.
}



////////////////////////////////////////////////////////////

const YsMatrix3x3 &YsMatrix3x3::operator*=(const class YsAtt3 &att)
{
	Rotate(att);
	return *this;
}

void YsMatrix3x3::Rotate(const YsRotation &rot)
{
	YsVec3 v;
	double a;
	rot.Get(v,a);
	Rotate(v.x(),v.y(),v.z(),a);
}

void YsMatrix3x3::Rotate(const class YsAtt3 &att)
{
	switch(YsCoordSysModel)
	{
	case YSBLUEIMPULSE:
		RotateXZ(att.h());
		RotateZY(att.p());
		RotateXY(att.b());
		break;
	case YSOPENGL:
		RotateXZ(-att.h());
		RotateZY(-att.p());
		RotateXY(att.b());
		break;
	}
}

YSRESULT YsMatrix3x3::MulTranspose(YsVec3 &dst,const YsVec3 &src) const
{
	YsVec3 *o,tmp;

	if(&dst==&src)
	{
		o=&tmp;
	}
	else
	{
		o=&dst;
	}

	o->SetX(v11()*src[0]+v21()*src[1]+v31()*src[2]);
	o->SetY(v12()*src[0]+v22()*src[1]+v32()*src[2]);
	o->SetZ(v13()*src[0]+v23()*src[1]+v33()*src[2]);

	if(o==&tmp)
	{
		dst=tmp;
	}

	return YSOK;
}

YSRESULT YsMatrix3x3::MakeToXY(const YsVec3 &zDir)
{
	YsAtt3 att;
	att.SetForwardVector(zDir);
	att.SetB(0.0);

	LoadIdentity();
	(*this)*=att;

	Transpose();

	return YSOK;
}

