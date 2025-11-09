#ifndef YSADVGEOMETRY_IS_INCLUDED
#define YSADVGEOMETRY_IS_INCLUDED
/* { */

YSBOOL YsCheckConvex2(int np,const YsVec2 p[],YSBOOL strictCheck=YSFALSE);
YSBOOL YsCheckConvex3(int np,const YsVec3 p[],YSBOOL strictCheck=YSFALSE);
YSBOOL YsCheckConvexByAngle2(int np,const YsVec2 *p);
YSBOOL YsCheckConvexByAngle3(int np,const YsVec3 *p);
double YsGetTriangleArea3(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3);
double YsGetTriangleArea2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3);
double YsGetPolygonArea2(int nPlVt,const YsVec2 p[]);
YSRESULT YsGetLargestTriangleFromPolygon3
   (const YsVec3 *v[3],int np,const YsVec3 p[]);
YSRESULT YsGetLargestTriangleFromPolygon3(YsVec3 v[3],int np,const YsVec3 p[]);
YSRESULT YsGetLargestTriangleFromPolygon2
   (const YsVec2 *v[3],int np,const YsVec2 p[]);
YSRESULT YsGetLargestTriangleFromPolygon2(YsVec2 v[3],int np,const YsVec2 p[]);
YSRESULT YsGetAverageNormalVector(YsVec3 &nom,int np,const YsVec3 p[]);
YSBOOL YsCheckInBetween2(const YsVec2 &q,const YsVec2 &p1,const YsVec2 &p2);
YSBOOL YsCheckInBetween3(const YsVec3 &q,const YsVec3 &p1,const YsVec3 &p2);
YSRESULT YsGetLineIntersection2
   (YsVec2 &crs,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &q1,const YsVec2 &q2);
YSBOOL YsCheckLineOverlap2
    (const YsVec2 &o1,const YsVec2 &v1,const YsVec2 &o2,const YsVec2 &v2);
YSBOOL YsCheckLineOverlap3
    (const YsVec3 &o1,const YsVec3 &v1,const YsVec3 &o2,const YsVec3 &v2);
YSSIDE YsCheckInsidePolygon2(const YsVec2 &ref,int np,const YsVec2 p[]);
YSSIDE YsCheckInsidePolygon3(const YsVec3 &ref,int np,const YsVec3 p[]);
YSSIDE YsCheckInsidePolygon3(const YsVec3 &ref,int np,const YsVec3 p[],const YsVec3 &nom);

YSSIDE YsCheckInsideTriangle2(const YsVec2 &ref,const YsVec2 tri[]);
YSSIDE YsCheckInsideTriangle3(const YsVec3 &ref,const YsVec3 tri[]);


YSBOOL YsIsLineSegIntersectionReliable2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &q1,const YsVec2 &q2);

YSBOOL YsPointEdgeDistanceIsWithinRangeOf(const YsVec3 &pnt,int np,const YsVec3 p[],const double &ratio);

YSRESULT YsGetCenterOfPolygon3(YsVec3 &cen,int np,const YsVec3 plg[]);
YSRESULT YsGetCenterOfPolygon2(YsVec2 &cen,int np,const YsVec2 plg[]);
	// Note:These functions will return the center of the
	//      largest triangle given by YsGetLargestTriangleFromPolygon3/2

YSRESULT YsGetPolygonProjectionMatrix(YsMatrix4x4 &mat,int np,const YsVec3 p[]);
	// Returns a matrix that project a Polygon into XY plane.
	// Note:Twist direction will be reversed in Left-Handed coordinate.

YSINTERSECTION YsGetLinePenetration2
    (const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3,const YsVec2 &p4);
YSINTERSECTION YsGetLinePenetration3
    (const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &q1,const YsVec3 &q2);


YSBOOL YsCheckSeparatability2(int np,const YsVec2 p[],int idx1,int idx2);
YSBOOL YsCheckSeparatability3(int np,const YsVec3 p[],int idx1,int idx2);
	// Note:YsCheckSeparatability3 is extremely slow function.
	//      Not recommended to used it in a loop.
	//      It is recommended to make PolygonProjectionMatrix
	//      and project the polygon into XY plane. Then, use it to
	//      Check separatability.

YSBOOL YsCheckBoundingBoxCollision3
 (const YsVec3 &min1,const YsVec3 &max1,const YsVec3 &min2,const YsVec3 &max2);
YSBOOL YsCheckBoundingBoxCollision2
 (const YsVec2 &min1,const YsVec2 &max1,const YsVec2 &min2,const YsVec2 &max2);

YSBOOL YsCheckInsideBoundingBox3
    (const YsVec3 &ref,const YsVec3 &min,const YsVec3 &max);
YSBOOL YsCheckInsideBoundingBox2
    (const YsVec2 &ref,const YsVec2 &min,const YsVec2 &max);

YSFLIPDIRECTION YsCheckFlipDirection2(int np,const YsVec2 p[]);
YSFLIPDIRECTION YsCheckFlipDirection3(int np,const YsVec3 p[],const YsVec3 &n);

YSRESULT YsGetTwoPlaneCrossLine
    (YsVec3 &lnOrg,YsVec3 &lnVec,const YsPlane &pl1,const YsPlane &pl2);

YSRESULT YsGetNearestPointOfTwoLine
    (YsVec3 &np,YsVec3 &nq,
     const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &q1,const YsVec3 &q2);
YSRESULT YsGetNearestPointOnLine3
    (YsVec3 &np,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref);
YSRESULT YsGetNearestPointOnLine2
    (YsVec2 &np,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &ref);
const double YsGetPointLineDistance2(const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &ref);
const double YsGetPointLineDistance3(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref);
double YsGetDistanceBetweenTwoLineSegment(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &q1,const YsVec3 &q2);
const double YsGetPointLineSegDistance2(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref); 
const double YsGetPointLineSegDistance3(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &ref); 

YSBOOL YsCheckPointIsOnLineSegment3(const YsVec3 &p,const YsVec3 &p1,const YsVec3 &p2);


YSINTERSECTION YsGetPolygonPenetration(int np,const YsVec3 p[],int nq,const YsVec3 q[]);

YSRESULT YsGetArbitraryInsidePointOfPolygon2
  (YsVec2 &aip,int np,const YsVec2 p[],YSBOOL strictCheckOfConvexity=YSFALSE);
YSRESULT YsGetArbitraryInsidePointOfPolygon3
  (YsVec3 &aip,int np,const YsVec3 p[],YSBOOL strictCheckOfConvexity=YSFALSE);


YSRESULT YsSortPointSet3(int np,YsVec3 p[],int idx[]);
YSRESULT YsSortPointSet3(int np,YsVec3 p[],int idx[],const YsVec3 &refp);
YSRESULT YsSortPointSet3(YsVec3 p[],int idx[],int np,const YsVec3 refp,double sqKnownMaxDistance);  // Only when sqDist is known


YSRESULT YsGetCircumCircle(YsVec2 &cen,double &r,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3);
YSRESULT YsGetInscribedCircle(YsVec2 &cen,double &rad,const YsVec2 &p1,const YsVec2 &p2,const YsVec2 &p3);

YSRESULT YsGetCircumSphereRadius(double &rad,YsVec3 const *p[4]);
YSRESULT YsGetCircumSphere(YsVec3 &cen,double &rad,YsVec3 const *p[4]);
YSRESULT YsGetCircumSphereRadius(double &rad,const YsVec3 p[4]);
YSRESULT YsGetCircumSphere(YsVec3 &cen,double &rad,const YsVec3 p[4]);

YSRESULT YsComputeNormalOfTriangle(YsVec3 &nom,const YsVec3 &t1,const YsVec3 &t2,const YsVec3 &t3);
YSRESULT YsGetNormalOfTet(YsVec3 nom[4],YsVec3 const *p[4]);
YSRESULT YsGetInscribedSphereRadius(double &rad,YsVec3 const *p[4]);
YSRESULT YsGetInscribedSphere(YsVec3 &cen,double &rad,YsVec3 const *p[4]);
YSRESULT YsGetInscribedSphereRadius(double &rad,const YsVec3 p[4]);
YSRESULT YsGetInscribedSphere(YsVec3 &cen,double &rad,const YsVec3 p[4]);

YSRESULT YsGetCircumSphereOfTriangle(YsVec3 &cen,double &rad,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3);
YSRESULT YsGetInscribedSphereOfTriangle(YsVec3 &cen,double &rad,const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3);


YSRESULT YsInflateBoundingBox
    (YsVec3 &newBbxMin,YsVec3 &newBbxMax,const YsVec3 &bbxMin,const YsVec3 &bbxMax,const double ratio);


YSRESULT YsComputeTwoSphereIntersection
(YsVec3 &o,YsVec3 &n,double &r,const YsVec3 &c1,const double &r1,const YsVec3 &c2,const double &r2);


YSRESULT YsComputeThreeSphereIntersection
(YsVec3 &i1,YsVec3 &i2,
 const YsVec3 &c1,const double &r1,const YsVec3 &c2,const double &r2,const YsVec3 &c3,const double &r3);



YSRESULT YsGetNearestPointOnLineSegments3
   (YsVec3 &neaPos,int &idx,YSBOOL &nearNode,int nP,const YsVec3 p[],const YsVec3 &refp);

YSRESULT YsGetNearestPointOnLineSegments2
   (YsVec2 &nearPos,int &nearIdx,YSBOOL &nearNode,int np,const YsVec2 p[],const YsVec2 &refp);

YSRESULT YsGetTangentialVectorAtNodeOfLineSegments3
   (YsVec3 &unitTan,int ndId,int np,const YsVec3 p[]);
YSRESULT YsGetTangentialVectorOnEdgeOfLineSegments3
   (YsVec3 &unitTan,int edId,int np,const YsVec3 p[]);



template <class T>
int YsFindNearestPoint(const T &refp,int np,const T p[])
{
	if(np>0)
	{
		int i,id;
		double d,dMin;
		id=0;
		dMin=(p[0]-refp).GetSquareLength();
		for(i=1; i<np; i++)
		{
			d=(p[i]-refp).GetSquareLength();
			if(d<dMin)
			{
				id=i;
				dMin=d;
			}
		}
		return id;
	}
	return -1;
}

inline int YsFindNearestPoint3(const YsVec3 &refp,int np,const YsVec3 p[])
{
	return YsFindNearestPoint <YsVec3> (refp,np,p);
}

inline int YsFindNearestPoint2(const YsVec2 &refp,int np,const YsVec2 p[])
{
	return YsFindNearestPoint <YsVec2> (refp,np,p);
}


template <class T>
int YsFindFarthestPoint(const T &refp,int np,const T p[])
{
	if(np>0)
	{
		int i,id;
		double d,dMax;
		id=0;
		dMax=(p[0]-refp).GetSquareLength();
		for(i=1; i<np; i++)
		{
			d=(p[i]-refp).GetSquareLength();
			if(d>dMax)
			{
				id=i;
				dMax=d;
			}
		}
		return id;
	}
	return -1;
}

inline int YsFindFarthestPoint3(const YsVec3 &refp,int np,const YsVec3 p[])
{
	return YsFindFarthestPoint <YsVec3> (refp,np,p);
}

inline int YsFindFarthestPoint2(const YsVec2 &refp,int np,const YsVec2 p[])
{
	return YsFindFarthestPoint <YsVec2> (refp,np,p);
}



////////////////////////////////////////////////////////////

template <class T>
void YsQuickSortPointSet3Template(YsVec3 tab[],T idx[],int nTab,const YsVec3 &refp,double mind,double maxd)
{
	if(nTab>1)
	{
		double threshold;

		threshold=(mind+maxd)/2.0;

		// smaller comes first
		YSBOOL allTheSame;
		double d0;
		int i,nSmall;

		nSmall=0;
		allTheSame=YSTRUE;
		d0=(tab[0]-refp).GetSquareLength();

		for(i=0; i<nTab; i++) // Actually can begin with 1
		{
			double var;
			var=(tab[i]-refp).GetSquareLength();
			if(allTheSame==YSTRUE && YsAbs(sqrt(var)-sqrt(d0))>YsTolerance)  // Modified 2000/11/10
			{
				allTheSame=YSFALSE;
			}
			if(var<=threshold)
			{
				YsVec3 a;
				a=tab[i];
				tab[i]=tab[nSmall];
				tab[nSmall]=a;

				if(idx!=NULL)
				{
					T x;
					x=idx[i];
					idx[i]=idx[nSmall];
					idx[nSmall]=x;
				}

				nSmall++;
			}
		}

		if(allTheSame!=YSTRUE)
		{
			if(idx!=NULL)
			{
				YsQuickSortPointSet3Template <T> (tab       ,idx       ,nSmall       ,refp,mind,threshold);
				YsQuickSortPointSet3Template <T> (tab+nSmall,idx+nSmall,(nTab-nSmall),refp,threshold,maxd);
			}
			else
			{
				YsQuickSortPointSet3Template <T> (tab       ,NULL,nSmall       ,refp,mind,threshold);
				YsQuickSortPointSet3Template <T> (tab+nSmall,NULL,(nTab-nSmall),refp,threshold,maxd);
			}
		}
	}
}

template <class T>
YSRESULT YsSortPointSet3Template(YsVec3 p[],T idx[],int np,const YsVec3 refp,double sqKnownMaxDistance)
{                                                               //  ^^^^ refp must not be a pointer/reference
	YsQuickSortPointSet3Template(p,idx,np,refp,0.0,sqKnownMaxDistance);
	return YSOK;
}

template <class T>
YSRESULT YsSortPointSet3Template(int np,YsVec3 p[],T idx[])
{
	int i,j;
	double sqDist;
	int vt1,vt2;
	vt1=0;
	vt2=0;
	sqDist=0.0;
	for(i=0; i<np; i++)
	{
		for(j=i+1; j<np; j++)
		{
			if((p[i]-p[j]).GetSquareLength()>sqDist)
			{
				vt1=i;
				vt2=j;
				sqDist=(p[i]-p[j]).GetSquareLength();
			}
		}
	}

	return YsSortPointSet3(p,idx,np,p[vt1],sqDist);

}

template <class T>
YSRESULT YsSortPointSet3Template(int np,YsVec3 p[],T idx[],const YsVec3 &refp)
{
	int i;
	double sqDist;
	sqDist=0.0;
	for(i=0; i<np; i++)
	{
		if((refp-p[i]).GetSquareLength()>sqDist)
		{
			sqDist=(refp-p[i]).GetSquareLength();
		}
	}
	return YsSortPointSet3Template(p,idx,np,refp,sqDist);
}

////////////////////////////////////////////////////////////

YSRESULT YsFindLeastSquareFittingPlane
    (double &aa,double &bb,double &cc,double &dd,int nVtx,const YsVec3 vtx[]);
YSRESULT YsFindLeastSquareFittingPlane
  (double &a,double &b,double &c,double &d,const YsArray <YsVec3> &vtxList);
YSRESULT YsFindLeastSquareFittingPlaneNormal(YsVec3 &nom,int np,const YsVec3 p[]);
YSRESULT YsFindLeastSquareFittingPlaneNormal(YsVec3 &nom,const YsArray <YsVec3> &vtxList);
YSRESULT YsFindLeastSquareFittingPlane(YsVec3 &org,YsVec3 &nom,int np,const YsVec3 p[]);
YSRESULT YsFindLeastSquareFittingPlane(YsVec3 &org,YsVec3 &nom,const YsArray <YsVec3> &vtxList);

YSRESULT YsFindLeastSquarePointFromLine(YsVec3 &pos,const int nLine,const YsVec3 o[],const YsVec3 _v[]);


double YsCalculateTotalLength2(int np,const YsVec2 p[],YSBOOL isLoop);
double YsCalculateTotalLength3(int np,const YsVec3 p[],YSBOOL isLoop);

////////////////////////////////////////////////////////////


/* } */
#endif
