// This program is separated from ysadvgeometry.cpp due to
// VC++5.0's bug.

// Not to add functions that are not affected by VC++5.0's bug   > me.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "ysclass.h"



////////////////////////////////////////////////////////////

YSBOOL YsCheckInBetween2(const YsVec2 &q,const YsVec2 &p1,const YsVec2 &p2)
{
	if(p1==q || p2==q)
	{
		return YSTRUE;
	}
	else if((p2-p1)*(q-p1)>=0.0 && (p1-p2)*(q-p2)>=0.0)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

////////////////////////////////////////////////////////////


YSBOOL YsCheckInBetween3(const YsVec3 &q,const YsVec3 &p1,const YsVec3 &p2)
{
// VC++5.0 Optimizer Bug here.
// Following Code doesn't work unless I turn off the optimization.
	if(p1==q || p2==q)
	{
		return YSTRUE;
	}
	else if((p2-p1)*(q-p1)>=0.0 && (p1-p2)*(q-p2)>=0.0)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

////////////////////////////////////////////////////////////



YSSIDE YsCheckInsidePolygon3(const YsVec3 &pnt,int np,const YsVec3 plg[])
{
	int i;

	// Bounding Box Check // added on 01/25/2000
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	bbx.Begin(plg[0]);
	for(i=0; i<np; i++)
	{
		bbx.Add(plg[i]);
		if(pnt==plg[i])
		{
			return YSBOUNDARY;
		}
	}
	bbx.Get(bbxMin,bbxMax);
	if(YsCheckInsideBoundingBox3(pnt,bbxMin,bbxMax)!=YSTRUE)
	{
		return YSOUTSIDE;
	}



	YsVec3 nom;

	if(np==3)   // Added 2003/05/02
	{
		nom=(plg[1]-plg[0])^(plg[2]-plg[0]);
		if(nom.Normalize()!=YSOK)
		{
			YsErrOut("YsCheckInsidePolygon3()\n");
			YsErrOut("  Cannot compute a normal of a triangle.\n");
			for(i=0; i<np; i++)
			{
				YsErrOut("    %s\n",plg[i].Txt());
			}
			return YSUNKNOWNSIDE;
		}
	}
	else
	{
		if(YsFindLeastSquareFittingPlaneNormal(nom,np,plg)!=YSOK)
		{
			if(YsGetAverageNormalVector(nom,np,plg)!=YSOK)   // Added 2003/05/02
			{
				YsErrOut("YsCheckInsidePolygon3()\n");
				YsErrOut("  Cannot compute a normal of a polygon.\n");
				for(i=0; i<np; i++)
				{
					YsErrOut("    %s\n",plg[i].Txt());
				}
				return YSUNKNOWNSIDE;
			}
		}
	}


	YsVec3 prj;
	YsArray <YsVec2,16> p2;
	YsVec2 tst;
	YsMatrix3x3 tfm;
	double h,p;
	p=asin(nom.y());
	h=atan2(-nom.x(),nom.z());
	tfm.RotateZY(-p);
	tfm.RotateXZ(-h);

	p2.Set(np,NULL);
	for(i=0; i<np; i++)
	{
		prj=tfm*plg[i];
		p2[i].Set(prj.x(),prj.y());
	}
	prj=tfm*pnt;
	tst.Set(prj.x(),prj.y());

	return YsCheckInsidePolygon2(tst,np,p2);
}

YSSIDE YsCheckInsidePolygon3(const YsVec3 &pnt,int np,const YsVec3 plg[],const YsVec3 &nom)
{
	int i;

	if(nom==YsOrigin())
	{
		return YsCheckInsidePolygon3(pnt,np,plg);
	}



	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	bbx.Begin(plg[0]);
	for(i=0; i<np; i++)
	{
		bbx.Add(plg[i]);
		if(pnt==plg[i])
		{
			return YSBOUNDARY;
		}
	}
	bbx.Get(bbxMin,bbxMax);
	if(YsCheckInsideBoundingBox3(pnt,bbxMin,bbxMax)!=YSTRUE)
	{
		return YSOUTSIDE;
	}



	YsVec3 prj;
	YsArray <YsVec2,16> p2;
	YsVec2 tst;
	YsMatrix3x3 tfm;
	double h,p;
	p=asin(nom.y());
	h=atan2(-nom.x(),nom.z());
	tfm.RotateZY(-p);
	tfm.RotateXZ(-h);

	p2.Set(np,NULL);
	for(i=0; i<np; i++)
	{
		prj=tfm*plg[i];
		p2[i].Set(prj.x(),prj.y());
	}
	prj=tfm*pnt;
	tst.Set(prj.x(),prj.y());

	return YsCheckInsidePolygon2(tst,np,p2);
}


