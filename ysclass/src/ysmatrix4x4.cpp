#include "ysclass.h"


// This code is needed to cope with VC++'s stupid bug.
// VC++ cannot compile template parameters correctly when different
// template parameters are mixed in one .CPP file.

YSRESULT YsInvertMatrix4x4(double result[],const double from[])
{
	return YsInvertMatrixNxN <4> (result,from);
}

YSRESULT YsMulInverse4x4(double inv[],const double mat[],const double vec[])
{
	return YsMulInverseNxN <4> (inv,mat,vec);
}


////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////

const YsMatrix4x4 &YsMatrix4x4::operator=(const YsMatrix &mat)
{
	int r,c;

	if((mat.nr()!=3 || mat.nc()!=3) && (mat.nr()!=4 || mat.nc()!=4))
	{
		printf("YsMatrix4x4::operator=(const YsMatrix &mat)\n");
		printf("  Warning:\n");
		printf("  Right hand side of the substitution is not 3x3 nor 4x4 matrix.\n");
	}

	for(r=1; r<=4 && r<=mat.nr(); r++)
	{
		for(c=1; c<=4 && c<=mat.nc(); c++)
		{
			Set(r,c,mat.v(r,c));
		}
	}

	if(mat.nr()==3 && mat.nc()==3)
	{
		Set(1,4,0.0);
		Set(2,4,0.0);
		Set(3,4,0.0);
		Set(4,4,1.0);
		Set(4,1,0.0);
		Set(4,2,0.0);
		Set(4,3,0.0);
	}

	return *this;
}

const YsMatrix4x4 &YsMatrix4x4::operator*=(const class YsAtt3 &att)
{
	Rotate(att);
	return *this;
}

void YsMatrix4x4::Rotate(const YsRotation &rot)
{
	YsVec3 v;
	double a;
	rot.Get(v,a);
	Rotate(v.x(),v.y(),v.z(),a);
}

void YsMatrix4x4::Rotate(const YsAtt3 &att)
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


YSRESULT YsMatrix4x4::MakeToXY(const YsVec3 &cen,const YsVec3 &zDir)
{
	YsAtt3 att;
	att.SetForwardVector(zDir);
	att.SetB(0.0);

	LoadIdentity();
	Rotate(att);
	Transpose();

	Translate(-cen);

	return YSOK;
}
