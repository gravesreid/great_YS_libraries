#ifndef YSSHELLUTIL_IS_INCLUDED
#define YSSHELLUTIL_IS_INCLUDED
/* { */

YSRESULT YsShellMakeQuadFromEdge
    (YsShellVertexHandle quad[4],YsShellPolygonHandle plHd[2],
     const YsShell &shl,const YsShellVertexHandle vtHd1,const YsShellVertexHandle vtHd2);

YSRESULT YsShellMakeConnectedPolygonList
    (YsArray <YsShellPolygonHandle> &plHdList,
     const YsShell &shl,YsShellPolygonHandle initPlHd,YSBOOL takeNonManifold,int hashSize=65533);


YSRESULT YsSlidePointOnShell
    (YsVec3 &newPos,YsVec3 &lastDir,YsShellPolygonHandle &newPlHd,
     YSBOOL &reachedDeadEnd,
     YsArray <YsVec3,16> *pathPnt,YsArray <YsShellPolygonHandle,16> *pathPlHd,
     const YsShell &shl,const YsVec3 &oldPos,const YsVec3 &displacement,YsShellPolygonHandle oldPlHd);

class YsShellVectorFunction
{
public:
	virtual YSRESULT GetVectorOnPolygon(YsVec3 &dir,const YsShell &shl,const YsShellPolygonHandle plHd,const YsVec3 &pos) const;
	virtual YSRESULT GetVectorOnVertex(YsVec3 &dir,const YsShell &shl,const YsShellVertexHandle vtHd) const;
};

class YsShellCrawlStopperFunction
{
public:
	virtual YSBOOL IsStopperVertex(const YsShell &shl,YsShellVertexHandle vtHd) const;
	virtual YSBOOL IsStopperEdge(const YsShell &shl,const YsShellVertexHandle edVtHd[2]) const;
	virtual YSBOOL IsStopperPolygon(const YsShell &shl,const YsShellPolygonHandle plHd) const;
};

YSBOOL YsVectorPointingInside(int np,const YsVec3 plg[],const YsVec3 &nom,int fromIdx,const YsVec3 &dir);

class YsShellCrawler   // Generalized sliding point
{
public:
	YsShellPolygonHandle currentPlHd;
	YsVec3 currentPos,currentDir;
	const YsShellVectorFunction *crawlDirFunc;  // 2006/08/02
	const YsShellCrawlStopperFunction *crawlStopFunc; // 2007/02/07

	YsVec3 goal;  // Used only when crawlingMode==1
	YsPlane constPlane;  // Used only when crawlingMode==2

	int currentState;  // 0:Inside the polygon    1:On an edge of the polygon    2:On a vertex of the polygon
	                   // Number shouldn't be changed.  Already some programs are using currentState value.
	int crawlingMode;  // 0:Follow the curvature (default)  1:Head to the goal  2:Crawler on a plane

	YsShellVertexHandle currentEdVtHd[2];

	YSBOOL reachedDeadEnd;
	YSBOOL firstStep;

	YsArray <YsVec3,32> pathPnt;
	YsArray <YsShellPolygonHandle,32> pathPlHd;
	YsArray <YsVec2,32> prjPlg;
	YsVec2 prjPos,prjDir;


	YsShellCrawler();
	void Initialize(void);
	YSRESULT Start(const YsShell &shl,const YsVec3 &startPos,const YsVec3 &startDir,YsShellPolygonHandle startPlHd);
	YSRESULT Crawl(const YsShell &shl,const double &dist,YSBOOL watch=YSFALSE);  // dist==0.0 -> Move to the next polygon.
	double GetTotalLength(void) const;

protected:
	void CrawlCalculateProjectedPolygonAndDirection(
	    const YsShell &shl,YsVec2 &dir,YsArray <YsVec2,32> &plg,int nPlVt,const YsShellVertexHandle plVtHd[],const YsVec3 &nom);
	YSRESULT MoveAcrossEdge(const YsShell &shl,const YsShellSearchTable *search,const YsVec3 &nom,const YsShellVertexHandle nextEdVtHd[2],const double &nextEdParam);
	YSRESULT MoveAcrossVertex(const YsShell &shl,const YsShellSearchTable *search,const YsVec3 &nom,const YsShellVertexHandle nextEdVtHd);
};

YSRESULT YsGetLoopAroundShellPolygon
   (YsArray <YsShellVertexHandle> &loop,const YsShell &shl,YsShellPolygonHandle plHd);

double YsGetCosDihedralAngleAcrossShellEdge
   (const YsShell &shl,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);

YSRESULT YsShellFindPath
	    (YsArray <YsShellVertexHandle,16> &pathVtHd,
	     const YsShell &shl,YsShellVertexHandle from,YsShellVertexHandle to);

YSRESULT YsShellFindProximityPolygonListByVertex
   (YsArray <YsShellPolygonHandle,16> &plHdList,
    const YsShell &shl,YsShellVertexHandle from,const double &dist);
YSRESULT YsShellFindProximityPolygonListByEdge
   (YsArray <YsShellPolygonHandle,16> &plHdList,
    const YsShell &shl,YsShellVertexHandle fromEdVtHd[2],const double &dist);

YSRESULT YsShellMakePolygonStrip(
    YSBOOL &isLoop,YsArray <YsShellPolygonHandle> &plgStrip,
    const YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[]);

YSRESULT YsMakePolygonSubGroup(
    YsArray <YsShellPolygonHandle,64> &plHdList,
    const YsShell &shl,YsShellPolygonHandle seedPlHd,const YsKeyStore &includedPlKey);



template <class T>
YSRESULT YsGetNthEdge(T edVt[2],int nPlVt,const T vt[],int n)
{
	if(0<=n && n<nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[n+1];
		return YSOK;
	}
	else if(n==nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[0];
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsGetNthEdgeCyclic(T edVt[2],int nPlVt,const T vt[],int n)
{
	while(n<0)
	{
		n+=nPlVt;
	}
	while(n>=nPlVt)
	{
		n-=nPlVt;
	}

	if(0<=n && n<nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[n+1];
		return YSOK;
	}
	else if(n==nPlVt-1)
	{
		edVt[0]=vt[n];
		edVt[1]=vt[0];
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellGetNthEdge(YsShellVertexHandle edVtHd[2],int nPlVt,const YsShellVertexHandle plVtHd[],int n);
YSRESULT YsShellGetNthEdgeCyclic(YsShellVertexHandle edVtHd[2],int nPlVt,const YsShellVertexHandle plVtHd[],int n);


YSRESULT YsMergeShellPolygon(YsShell &shl,int nOrgPlg,const YsShellPolygonHandle orgPlHd[]);
YSRESULT YsGlueShellPolygon
    (int &nvNew,YsShellVertexHandle vNew[],
     int nv1,const YsShellVertexHandle v1[],int nv2,const YsShellVertexHandle v2[],
     YsShellVertexHandle sharedEdVtHd1,YsShellVertexHandle sharedEdVtHd2);


YSRESULT YsGetCrawlToNearestPointOnShell(YsVec3 &nearPos,YsShellElemType &nearElem,const YsShell &shl,const YsShellElemType &from,const YsVec3 &to);



/* } */
#endif
