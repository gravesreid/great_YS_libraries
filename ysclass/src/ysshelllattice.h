#ifndef YSSHELLLATTICE_IS_INCLUDED
#define YSSHELLLATTICE_IS_INCLUDED
/* { */

// Body of YsShellLattice is written in ysshelllattice.cpp

class YsShellLatticeEdgePiece
{
public:
	YsShellVertexHandle edVtHd[2];
	unsigned int ceKey;
};

class YsShellLatticeElem
{
public:
	YSSIDE side;
	YsArray <YsShellVertexHandle> vtxList;
	YsArray <YsShellPolygonHandle> plgList;
	YsArray <YsShellLatticeEdgePiece> edgList;
};

class YsShellLattice : public YsLattice3d <YsShellLatticeElem>
{
public:
	YsShellLattice(int hashSize=256);
	~YsShellLattice();

	void EnableAutoResizingHashTable(int minSize,int maxSize);
	void EnableAutoResizingHashTable(void);
	void DisablePolygonToCellHashTable(void);
	void DisableInsideOutsideInfo(void);

	YSRESULT SetDomain(const class YsShell &shl,int nCell);
	YSRESULT SetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,int nCell);
	YSRESULT SetDomain(const class YsShell &shl,int nx,int ny,int nz);
	YSRESULT SetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,int nx,int ny,int nz);
	YSRESULT SetDomainXYMode(const class YsShell &shl,int nCell);

	YSRESULT SetDomain(const class YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[],int nCell);
	YSRESULT SetDomain(const class YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbxMin,const YsVec3 &bbxMax,int nCell);
	YSRESULT SetDomain(const class YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[],int nx,int ny,int nz);
	YSRESULT SetDomain(const class YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbxMin,const YsVec3 &bbxMax,int nx,int ny,int nz);

	YSRESULT FillUndeterminedBlockRange(const class YsShell &shl,int bx0,int by0,int bz0,int bx1,int by1,int bz1);
	YSRESULT RepairLatticeAfterModification(YsShell &shl,int np,const YsVec3 p[]);
	YSRESULT AddVertex(YsShellVertexHandle vtHd);
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);
	YSRESULT AddPolygon(YsShellPolygonHandle plHd);
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);

	YSRESULT MakeSortedCellPolygonBalance(YsArray <int> &balance) const; // x y z N, x y z N, ....
	YSBOOL CheckSubLatticeNecessity
	    (int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsVec3 bbx[2],int &percentPlg,int &percentVol);
	YSBOOL MakeSubLattice(int nCell);
	const YsShellLattice *GetSubLattice(void) const;

	YSRESULT AddEdgePiece(const YsShellVertexHandle edVtHd[2],unsigned int ceKey);

	YSRESULT FixIndeterminantCell(void);

	YSSIDE CheckInsideSolid(const YsVec3 &pnt,YSBOOL showWhichFunctionDetermined=YSFALSE) const;
	YSRESULT FindFirstIntersection
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,YsShellPolygonHandle plHdExclude=NULL) const;
	YSRESULT FindFirstIntersection
	    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
	     const YsVec3 &org,const YsVec3 &dir,int nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const;

	YSRESULT GetCellListFromPolygon
	    (YsArray <YsFixedLengthArray <int,4>,4> &cell,YsShellPolygonHandle plHd) const;


	YSRESULT SelfDiagnostic(YsArray <YsVec3> &disagree);

	YSRESULT MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,int np,const YsVec3 p[]) const;
	YSRESULT MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,const YsVec3 &vt1,const YsVec3 &vt2) const;
	YSRESULT MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,YsShellPolygonHandle plHd) const;
	YSRESULT MakePossibllyIncludedVertexList
	    (YsArray <YsShellVertexHandle,16> &vtxList,int np,const YsVec3 p[]) const;
	YSRESULT MakeEdgePieceListByBoundingBox(
	     YsArray <YsShellVertexHandle,16> &edVtHd,YsArray <unsigned int,16> &ceKey,
	     const YsVec3 &vt1,const YsVec3 &vt2) const;

	YSBOOL CheckPolygonIntersection(YsShellPolygonHandle plHd) const;

	YSRESULT GetIntersectingPolygon(YsArray <YsShellPolygonHandle> &itscPlHd,const YsVec3 &vtPos1,const YsVec3 &vtPos2) const;
	YSRESULT GetIntersectingPolygon(YsArray <YsShellPolygonHandle> &itscPlHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;


	YSSIDE ShootRay
	(YsArray <YsVec3,16> &itsc,YsArray <YsShellPolygonHandle,16> &itscPlHd,
	 const YsVec3 &org,const YsVec3 &vec,YSBOOL watch=YSFALSE) const;
	YSRESULT CalculatePolygonRayIntersection  // Returns YSERR when the ray hits an edge or bad polygon.
	(YSBOOL &hit,YsVec3 &itsc,int np,const YsVec3 p[],
	 const YsVec3 &ray1,const YsVec3 &ray2,YSBOOL watch=YSFALSE) const;


	YsShellPolygonHandle FindNearestPolygon
	   (double &dist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL att4Test=YSFALSE,int att4Value=0) const;
	YSRESULT FindNearestEdgePiece
	   (YsShellVertexHandle edVtHd[2],unsigned int &ceKey,double &dist,YsVec3 &nearPos,const YsVec3 &from,
	    const YsVec3 &dissimilarConst=YsOrigin()) const;


	YSBOOL isSubLattice;  // If true, CheckInsideSolid will not call shl->CheckInsideSolid and return YSUNKNOWNSIDE when undetermined.
	mutable YSBOOL checkInsideSolidLog;
	mutable YsArray <YsVec3> foundIntersection;
	mutable YsArray <YsShellPolygonHandle,16> foundPolygon;
	mutable YsVec3 ray1,ray2;

protected:
	const YsShell *shl;
	YSBOOL enablePlgToCellTable;
	YSBOOL enableInsideOutsideInfo;
	int plgToCellHashSize;

	YSBOOL xyPlane,xzPlane,yzPlane;

	YsShellLattice *subLattice;

	//  plgToCellTable is used in AddPolygon, DeletePolygon, and MakePossibllyCollidingPolygonList.
	//  Disabling plgToCellTable will make DeletePolygon and MakePossibllyCollidingPolygonList unavailable.

	YsArray <YsFixedLengthArray <int,3>,16> indeterminantCellList;
	// YsMultiKeyHash <YsFixedLengthArray <int,4>,1,16> plgToCellTable;
	YsFixedLengthToMultiHashTable <YsFixedLengthArray <int,4>,1,4> plgToCellTable;

	YSRESULT FillUndeterminedBlock(int bx,int by,int bz,YSSIDE side);

	YSRESULT RecursivelyFillUndeterminedBlock(int bx,int by,int bz,YSSIDE side,int limitter=64);

	YSSIDE SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary(const YsVec3 &pnt,YSBOOL showDetail) const;
	YSSIDE ShootFiniteRay(const YsVec3 &pnt,int bx,int by,int bz,int cx,int cy,int cz,YSSIDE sideAtC) const;

	YSRESULT MakeListOfPolygonsThatGoThroughTheBlockRegion
	    (YsArray <YsShellPolygonHandle,16> &plgList,int x1,int y1,int z1,int x2,int y2,int z2) const;

	YSRESULT PreparePolygon
	    (YsArray <YsVec3> &plg,int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsShellPolygonHandle plHd);
};

/* } */
#endif
