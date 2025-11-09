#ifndef YSWIZARD_IS_INCLUDED
#define YSWIZARD_IS_INCLUDED
/* { */

// Declaration /////////////////////////////////////////////
class YsWizard
{
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsWizard::ClassName;}
};

// Declaration /////////////////////////////////////////////
class YsCollisionOfPolygon : public YsWizard
{
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsCollisionOfPolygon::ClassName;}

	YsCollisionOfPolygon();

	int GetNumVertexOfPolygon1(void) const;
	const YsVec3 *GetVertexListOfPolygon1(void) const;
	int GetNumVertexOfPolygon2(void) const;
	const YsVec3 *GetVertexListOfPolygon2(void) const;

	void SetPolygon1(int nv1,const YsVec3 v1[]);
	void SetPolygon2(int nv2,const YsVec3 v2[]);
	YSRESULT PrecomputeProjectionOfPolygon1(void);
	YSRESULT PrecomputeProjectionOfPolygon2(void);
	void SetPolygon1(int nv1,const YsVec3 v1[],const YsVec3 &nom1);
	void SetPolygon2(int nv2,const YsVec3 v2[],const YsVec3 &nom2);
	YSBOOL CheckOnTheSamePlane(void) const;
	YSBOOL CheckCollision(void) const;  // Just check true/false
	YSINTERSECTION CheckPenetration(void) const;  // Check aprt/touch/penetrate

	YSBOOL OneEdgeLiesOnTheOtherPlane(void) const;

	YSBOOL CheckIntersectionBetweenPolygon1AndBlock(const YsVec3 &min,const YsVec3 &max) const;
	YSBOOL CheckIntersectionBetweenPolygon1AndLineSegment(const YsVec3 &p1,const YsVec3 &p2) const;
	YSBOOL CheckIntersectionBetweenPolygon1AndLineSegment(YsVec3 &crs,const YsVec3 &p1,const YsVec3 &p2) const;  // 2008/11/06

protected:
	int np1;
	const YsVec3 *p1;
	YsVec3 p1min,p1max;
	YsPlane pln1;

	YsArray <YsVec2,16> p1Prj;
	YsVec2 p1PrjMin,p1PrjMax;
	YsMatrix4x4 p1PrjMat;

	int np2;
	const YsVec3 *p2;
	YsVec3 p2min,p2max;
	YsPlane pln2;

	YsArray <YsVec2,16> p2Prj;
	YsVec2 p2PrjMin,p2PrjMax;
	YsMatrix4x4 p2PrjMat;

	YSSIDE CheckInsideOfPolygon1(const YsVec3 &tst) const;
	YSSIDE CheckInsideOfPolygon2(const YsVec3 &tst) const;
};

// Declaration /////////////////////////////////////////////
class YsSwordPolygonAttr
{
public:
	YsSwordPolygonAttr();
	YSBOOL isOverlappingPolygon;  // For finding overlapping polygon
	int noteOverlapping;          // for polygon ID or any purpose
};

class YsSwordPolygon : public YsBaseClass
{
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsSwordPolygon::ClassName;}

	YsSwordPolygonAttr attrSet;

	YsArray <YsVec3> plg;  // Original Polygon
	YsArray <YsVec2> prj;  // Projected Polygon
	YsArray <int> vtId;    // Corresponding Vertex Id (optional)
	YsArray <YsShellVertexHandle> vtHd;  // Corresponding Vertex Handle (optional)
};

// Declaration /////////////////////////////////////////////
class YsSwordNewVertexLog : public YsBaseClass
{
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsSwordNewVertexLog::ClassName;}

	YsSwordNewVertexLog();

	int freeAttribute;
	int betweenVt1,betweenVt2;
	int idCreated;
	YsVec3 pos;
};

// Declaration /////////////////////////////////////////////

class YsSword : public YsWizard
{
private:
	const YsSword &operator=(const YsSword &from);  // Don't use copy operator
	YsSword(const YsSword &from); // Don't use copy constructor
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsSword::ClassName;}

	YsSword();
	~YsSword();

	void Encache(void) const;

	int GetNumPolygon(void) const;
	int GetNumVertexOfPolygon(int id) const;
	const YsArray <YsVec3> *GetPolygon(int id) const;
	const YsArray <int> *GetVertexIdList(int id) const;

	int GetNumNewVertex(void) const;
	const YsSwordNewVertexLog *GetNewVertexLog(int i) const;

	// For 3D Operation (Slash by plane)
	YSRESULT SetInitialPolygon(int np,const YsVec3 p[],const int id[]=NULL);
	YSRESULT Slash(const YsPlane &pln);
	YSRESULT Slash(int nv,const YsVec3 v[]);
	YSRESULT SlashByOverlappingPolygon3(int nv,const YsVec3 v[],int note=0);
	YSBOOL IsFromOverlappingPolygon3(int i) const;
	YSBOOL IsFromOverlappingPolygon3(int i,int &note) const;
	YSRESULT GetVertexListOfPolygon(YsVec3 vec[],int maxcount,int id) const;
	YSRESULT GetVertexListOfPolygon(const YsVec3 *vec[],int maxcount,int id) const;

	// For 2D Operation (Slash by line)
	YSRESULT SetInitialPolygon(int np,const YsVec2 p[],const int id[]=NULL);
	YSRESULT Slash(const YsVec2 &org,const YsVec2 &vec);
	YSRESULT GetVertexListOfPolygon(YsVec2 vec[],int maxcount,int id) const;

	// For both
	YSRESULT Triangulate(YSCONVEXNIZESTRATEGY strategy=YSCONVEXNIZEDEFAULT);
	YSRESULT Convexnize(YSCONVEXNIZESTRATEGY strategy=YSCONVEXNIZEDEFAULT);

	// Update Ids
	YSRESULT UpdateVertexId(int oldId,int newId);

	YSBOOL debugMode;

protected:
	YsList <YsSwordPolygon> *SeparateByTwo(YsList <YsSwordPolygon> *org,int id1,int id2);

	YsList <YsSwordPolygon> *SeparateByMulti(YsList <YsSwordPolygon> *org,YsArray <int> &cutPoint);


	YSRESULT SlashOnePolygon(YsList <YsSwordPolygon> *target,const YsPlane &pln,int nv=0,const YsVec3 v[]=NULL);
	YSRESULT FindAddSlashPoint
	    (YsArray <int> &idx,YsList <YsSwordPolygon> *taget,const YsPlane &pln,
	     int nCuttingPolygonVtx=0,const YsVec3 *cuttingPolygon=NULL);
	YSRESULT FindAddSlashPointToNeighborPolygons
		(const YsVec3 &v1,const YsVec3 &v2,
		 const YsVec3 &newVtx3,const YsVec2 &newVtx2,int newCreatedId,YsList <YsSwordPolygon> *exclude);

	YSRESULT ConvexnizeGo(YSBOOL tri,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy);
	YsList <YsSwordPolygon> *ConvexnizeOnePolygon
	    (YSBOOL tri,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy);

	YSRESULT FindConvexnizeVertexPair(int &id1,int &id2,YsList <YsSwordPolygon> *target,YSCONVEXNIZESTRATEGY strategy);

	int newCreateId;
	YsMatrix4x4 plgPrjMatrix;
	YsList <YsSwordPolygon> *plg;
	YsList <YsSwordNewVertexLog> *cre;
};

// Declaration /////////////////////////////////////////////
class YsBoundingBoxMaker2 : public YsWizard
{
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsBoundingBoxMaker2::ClassName;}

	YsBoundingBoxMaker2();

	void Begin(void);
	void Begin(const YsVec2 &vec);
	void Add(const YsVec2 &vec);
	void Make(int n,const YsVec2 v[]);
	void Make(YsList <YsVec2> *lst);
	void Make(const YsArray <YsVec2> &ary);
	void Get(YsVec2 &min,YsVec2 &max) const;
	const YsVec2 &GetCenter(YsVec2 &cen) const;

	YSBOOL IsInside(const YsVec2 &pos) const;  // 2008/09/16
	YSBOOL IsInside(const YsVec3 &pos) const;  // 2008/09/16
	YsVec2 Center(void) const;                 // 2008/09/16

protected:
	YSBOOL first;
	YsVec2 min,max;
};

// Declaration /////////////////////////////////////////////
class YsBoundingBoxMaker3 : public YsWizard
{
public:
	static char *ClassName;
	virtual char *WhatItIs(void){return YsBoundingBoxMaker3::ClassName;}

	YsBoundingBoxMaker3();

	void Begin(void);
	void Begin(const YsVec3 &vec);
	void Add(const YsVec3 &vec);
	void Make(int n,const YsVec3 v[]);
	void Make(const YsList <YsVec3> *lst);
	void Make(const YsArray <YsVec3> &ary);
	void Make(const YsShell &shl,int nVt,const YsShellVertexHandle vtHd[]);
	void Get(YsVec3 &min,YsVec3 &max) const;
	const YsVec3 &GetCenter(YsVec3 &cen) const;

	YSBOOL IsInside(const YsVec3 &pos) const;    // 2008/09/16
	YSBOOL IsInside(const YsVec2 &pos) const;    // 2008/09/16
	YSBOOL IsInsideXY(const YsVec3 &pos) const;  // 2008/09/16
	YSBOOL IsInsideXY(const YsVec2 &pos) const;  // 2008/09/16
	YsVec3 Center(void) const;                   // 2008/09/16

protected:
	YSBOOL first;
	YsVec3 min,max;
};

////////////////////////////////////////////////////////////

class YsLineSegmentTracer
{
public:
	YsVec3 pos;
	int seg;
	double segParam;
};

class YsTraceLineSegment
{
protected:
	YSBOOL isLoop;
	YsArray <YsVec3> lSeg;

	YsLineSegmentTracer curPos;

	double totalLength;
	YsArray <double> segLength;
	YsArray <double> segLengthAddUp;

	YSRESULT ResetLineSegment(void);

public:
	YSRESULT SetLineSegment(int np,const YsVec3 p[],YSBOOL isLoop);
	YSRESULT SetLineSegment(YsArray <YsVec3> &p,YSBOOL isLoop);

	int GetCurrentSegment(void) const;
	const YsVec3 &GetCurrentPosition(void) const;
	const YsVec3 GetCurrentTangent(void) const;
	const double &GetTotalLength(void) const;

	YSRESULT MoveByDistance(const double &dist);
	YSRESULT SetPositionByParameter(const double &t);

	YSRESULT MoveByDistance(YsLineSegmentTracer &tracer,const double &dist);
	YSRESULT SetPositionByParameter(YsLineSegmentTracer &tracer,const double &t);
	YSRESULT SetPositionByVertex(YsLineSegmentTracer &tracer,int idx);
	double GetParameter(const YsLineSegmentTracer &tracer) const;
};

/* } */
#endif
