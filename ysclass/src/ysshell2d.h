
//Next thing to do
//
//(X) Add YsArray <YsFixedLengthArray <YsShell2dVertexHandle,2> > strongCandidate,weakCandidage
//(X) StartAdvancingFront   Add all edges to strong candidate. Clear weak candidate
//( ) Write
//(X)     FindEdgeVertexPair    Based on FindStrongEdgeVertexPair
//(X)     FindRemoveEdgeVertexPair
//( )     FindRemoveStrongEdgeVertexPair
//( )     FindRemoveWeakEdgeVertexPair



#ifndef YSSHELL_D_IS_INCLUDED
#define YSSHELL_D_IS_INCLUDED
/* { */

// Attributes are mutable
class YsShell2dSpecialAttribute
{
public:
};

class YsShell2dVertex
{
public:
	YsShell2dVertex();
	void Initialize(void);

	YsVec3 p;
	mutable YsVec2 projected;
	int refCount;
	mutable int att1,att2,att3,att4;
	mutable const YsListItem <YsShellVertex> *vtHdMap;  // 2005/01/21

	unsigned searchKey;
	YsListItem <YsShell2dVertex> *thisInTheList;

	YsShell2dSpecialAttribute *special;
};


enum
{
	YSS2DEDGE_VIRTUAL=1,
	YSS2DEDGE_INTERNALCONSTEDGE=2
};

typedef const YsListItem <YsShell2dVertex> *YsShell2dVertexHandle;

class YsShell2dEdge
{
public:
	YsShell2dEdge();
	void Initialize(void);

	YsShell2dVertexHandle vtId1,vtId2;
	mutable int att1,att2,att3,att4;
	mutable void *attp;
	unsigned int flags;

	unsigned searchKey;
	YsListItem <YsShell2dEdge> *thisInTheList;

	YsShell2dSpecialAttribute *special;
};

typedef const YsListItem <YsShell2dEdge> *YsShell2dEdgeHandle;

// Note:
// Should be speeded up by supporting vtxListStore and edgListStore in the future


////////////////////////////////////////////////////////////

class YsShell2dLatticeElem
{
public:
	YSSIDE side;
	YsArray <YsShell2dVertexHandle> vtxList;
	YsArray <YsShell2dEdgeHandle> edgList;
};

class YsShell2dLattice : public YsLattice2d <YsShell2dLatticeElem>
{
public:
	YsShell2dLattice();

	YSRESULT SetDomain(const class YsShell2d &shl,int nCell);
	YSRESULT SetDomain(const class YsShell2d &shl,int nCell,const YsVec2 &bbxMin,const YsVec2 &bbxMax);
	YSRESULT SetDomain(const class YsShell2d &shl,int nx,int ny);
	YSRESULT SetDomain(const class YsShell2d &shl,int nx,int ny,const YsVec2 &bbxMin,const YsVec2 &bbxMax);
	YSRESULT AddVertex(YsShell2dVertexHandle v2Hd);
	YSRESULT DeleteVertex(YsShell2dVertexHandle v2Hd);
	YSRESULT AddEdge(YsShell2dEdgeHandle edHd);
	YSRESULT DeleteEdge(YsShell2dEdgeHandle edHd,YSSIDE cellMustBecome);
	YSRESULT FillUndeterminedBlock(void);

	YSSIDE CheckInsidePolygon(const YsVec2 &pnt,YSBOOL showWhichFunctionDetermined=YSFALSE) const;
	YSSIDE CheckInsidePolygon(const YsVec3 &pnt,YSBOOL showWhichFunctionDetermined=YSFALSE) const;  // Check after applying shl->matrix


	YSRESULT FindVertexListByPoint(YsArray <YsShell2dVertexHandle> &vtHdList,const YsVec2 &pnt) const;
	YSRESULT FindVertexListByRegion(YsArray <YsShell2dVertexHandle> &vtHdList,const YsVec2 &p0,const YsVec2 &p1) const;
	YSRESULT FindEdgeListByPoint(YsArray <YsShell2dEdgeHandle> &edHdList,const YsVec2 &pnt) const;
	YSRESULT FindEdgeListByRegion(YsArray <YsShell2dEdgeHandle> &edHdList,const YsVec2 &p0,const YsVec2 &p1) const;

	int numberOfUndeterminedCellCreatedByDeleteEdge;

protected:
	const YsShell2d *shl;
	YSRESULT FillUndeterminedBlock(int bx,int by,YSSIDE side,int limitter=32);

	YSSIDE SophisticatedCheckInsidePolygonWhenTheCellIsMarkedAsBoundary(const YsVec2 &pnt,YSBOOL showDetail) const;
	YSRESULT MakeListOfEdgesThatGoThroughTheBlockRegion(YsArray <YsShell2dEdgeHandle> &edgList,int x1,int y1,int x2,int y2) const;
	YSRESULT MakeListOfVertexInBlockRegion(YsArray <YsShell2dVertexHandle> &vtxList,int x1,int y1,int x2,int y2) const;
};


// Declaration /////////////////////////////////////////////
class YsShell2d
{
private:
	YsShell2d(const YsShell2d &from);
	const YsShell2d &operator=(const YsShell2d &from);

public:
	YsShell2d();
	~YsShell2d();


	void Encache(void) const;

// Modifier
	YSRESULT AttachSearchTable(class YsShell2dSearchTable *search);
	YSRESULT DetachSearchTable(void);
	const class YsShell2dSearchTable *GetSearchTable(void) const;

	void CleanUp(void);
	static void CleanUpVertexEdgeStore(void);
	void SetBasePlane(const YsPlane &pln);
	void SetNoTransformation(void);  // For purely 2d operation.
	YSRESULT AutoComputeBasePlane(void);
	int AddVertex(const YsVec3 &pos,int attrib=0);
	int AddEdge(int vtId1,int vtId2);
	YSRESULT DeleteEdge(int edId);
	YSRESULT DeleteEdge(YsShell2dEdgeHandle edId);
	YsShell2dVertexHandle AddVertexH(const YsVec3 &pos,int attrib=0);
	YsShell2dVertexHandle AddVertexH(const YsVec2 &pos,int attrib=0);
	YsShell2dEdgeHandle AddEdgeH(YsShell2dVertexHandle vtId1,YsShell2dVertexHandle vtId2);
	YSRESULT SetVertexPosition(YsShell2dVertexHandle vtHd,const YsVec3 &pos);
	YSRESULT SetVertexPosition(YsShell2dVertexHandle vtHd,const YsVec2 &pos);
	//  YSRESULT DeleteVertex(int vtId); boundingBoxIsValid will become YSFALSE;
	YSRESULT DeleteVertex(YsShell2dVertexHandle vtHd);  // boundingBoxIsValid will become YSFALSE;
	YSRESULT RemoveEdgeVertexTriangle
	    (YsShell2dEdgeHandle edId,YsShell2dVertexHandle vtId,YSBOOL iKnowItIsValid=YSFALSE,YsShell2dLattice *ltc=NULL);
	YSRESULT RemoveEdgeEdgeTriangle
	    (YsShell2dEdgeHandle edId1,YsShell2dEdgeHandle edId2,YSBOOL iKnowItIsValid=YSFALSE,YsShell2dLattice *ltc=NULL);
	YSRESULT RemoveThreeEdgeTriangle
	    (YsShell2dEdgeHandle edId1,YsShell2dEdgeHandle edId2,YsShell2dEdgeHandle edId3,YSBOOL iKnowItIsValid=YSFALSE,YsShell2dLattice *ltc=NULL);
	YSRESULT RemoveEdgeNewVertexTriangle
	    (YsShell2dEdgeHandle edId,const YsVec3 &newVtx,YSBOOL iKnowItIsValid=YSFALSE,YsShell2dLattice *ltc=NULL);

	YsShell2dSpecialAttribute *GetSpecialAttributeVertex(YsShell2dVertexHandle vtHd);
	const YsShell2dSpecialAttribute *GetSpecialAttributeVertex(YsShell2dVertexHandle vtHd) const;
	YsShell2dSpecialAttribute *GetSpecialAttributeEdge(YsShell2dEdgeHandle vtHd);
	const YsShell2dSpecialAttribute *GetSpecialAttributeEdge(YsShell2dEdgeHandle vtHd) const;

	YSRESULT SaveSrf(const char fn[]);
	YSRESULT SaveS2d(const char fn[]);
	YSRESULT LoadS2d(const char fn[]);

protected:
	virtual YSRESULT AttachSpecialAttributeToNewVertex(YsShell2dVertex &vtx,YsShell2dVertexHandle vtHd);
	virtual YSRESULT AttachSpecialAttributeToNewEdge(YsShell2dEdge &plg,YsShell2dEdgeHandle edHd);
	virtual YSRESULT DetachSpecialAttributeBeforeDeletingVertex(YsShell2dVertex &vtx);
	virtual YSRESULT DetachSpecialAttributeBeforeDeletingEdge(YsShell2dEdge &plg);


// Non modifier
public:
	const YsPlane &GetBasePlane(void) const;

	const YsMatrix4x4 &GetProjectionMatrix(void) const;

	YSRESULT GetBoundingBox(YsVec2 &mn,YsVec2 &mx) const;

	int GetNumVertex(void) const;
	const YsVec3 &GetVertexPosition(int vtId) const;
	int GetVertexReferenceCount(int vtId) const;
	int GetVertexAttrib1(int vtId) const;
	int GetVertexAttrib2(int vtId) const;
	int GetVertexAttrib3(int vtId) const;
	int GetVertexAttrib4(int vtId) const;
	YSRESULT SetVertexAttrib1(int vtId,int att) const;
	YSRESULT SetVertexAttrib2(int vtId,int att) const;
	YSRESULT SetVertexAttrib3(int vtId,int att) const;
	YSRESULT SetVertexAttrib4(int vtId,int att) const;

	int GetNumEdge(void) const;
	YSRESULT GetEdgeVertexId(int &edVt1,int &edVt2,int edId) const;
	int GetEdgeAttrib1(int edId) const;
	int GetEdgeAttrib2(int edId) const;
	int GetEdgeAttrib3(int edId) const;
	int GetEdgeAttrib4(int edId) const;
	YSRESULT SetEdgeAttrib1(int edId,int att) const;
	YSRESULT SetEdgeAttrib2(int edId,int att) const;
	YSRESULT SetEdgeAttrib3(int edId,int att) const;
	YSRESULT SetEdgeAttrib4(int edId,int att) const;

	YSSIDE CheckInsidePolygon(const YsVec2 &p,YSBOOL showDetail=YSFALSE) const;
	YSSIDE CheckInsidePolygon(const YsVec3 &p) const;

	YSRESULT ShootFiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
	     const YsVec3 &p1,const YsVec3 &p2,const YsShell2dLattice *ltc=NULL) const;
	YSRESULT ShootFiniteRay
	    (YsArray <YsVec2> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
	     const YsVec2 &p1,const YsVec2 &p2,const YsShell2dLattice *ltc=NULL,
	     YsShell2dVertexHandle excludeVtx1=NULL,YsShell2dVertexHandle excludeVtx2=NULL) const;

	YSRESULT ShootFiniteRay
	    (YsArray <YsVec3> &is,
	     const YsVec3 &p1,const YsVec3 &p2,const YsShell2dLattice *ltc=NULL) const;
	YSRESULT ShootFiniteRay
	    (YsArray <YsVec2> &is,
	     const YsVec2 &p1,const YsVec2 &p2,const YsShell2dLattice *ltc=NULL,
	     YsShell2dVertexHandle excludeVtx1=NULL,YsShell2dVertexHandle excludeVtx2=NULL) const;

	YSRESULT ShootInfiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
	     const YsVec3 &org,const YsVec3 &vec,const YsShell2dLattice *ltc=NULL) const;
	YSRESULT ShootInfiniteRay
	    (YsArray <YsVec2> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
	     const YsVec2 &org,const YsVec2 &vec,const YsShell2dLattice *ltc=NULL,
	     YsShell2dVertexHandle excludeVtx1=NULL,YsShell2dVertexHandle excludeVtx2=NULL) const;


	YSBOOL CheckNeighborEdge(int edVtId[3],int edId1,int edId2) const;
	YSBOOL CheckNeighborEdge(YsShell2dVertexHandle edVtId[3],YsShell2dEdgeHandle edId1,YsShell2dEdgeHandle edId2) const;

	double GetCosineCornerAngle(YsShell2dVertexHandle v2Hd) const;  
	    // Make sure reference count is two before using.
	    // Need search table.
	double GetEdgeSquareLength(YsShell2dEdgeHandle edHd) const;
	double GetEdgeLength(YsShell2dEdgeHandle edHd) const;


	YSBOOL CheckEdgeVertexTriangleIsRemovable
	    (YsShell2dEdgeHandle edId,YsShell2dVertexHandle vtId,const YsShell2dLattice *ltc=NULL) const;
	            // 1 edge + 1 boundary vertex
	YSBOOL CheckEdgeEdgeTriangleIsRemovable
	    (YsShell2dEdgeHandle edId1,YsShell2dEdgeHandle edId2,const YsShell2dLattice *ltc=NULL) const;
	            // 2 conseqtive edges
	YSBOOL CheckThreeEdgeTriangleIsRemovable
	    (YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2,YsShell2dEdgeHandle edHd3,const YsShell2dLattice *ltc=NULL) const;  // 2005/01/25
	YSBOOL CheckEdgeNewVertexTriangleIsRemovable
	    (YsShell2dEdgeHandle edId,const YsVec3 &newVtx,const YsShell2dLattice *ltc=NULL) const;
	            // 1 edge + 1 new vertex

	YSBOOL CheckSelfIntersection(const YsShell2dLattice *ltc=NULL) const;
	YSBOOL CheckEdgeIntersection(YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2) const;


	int GetVtIdFromHandle(YsShell2dVertexHandle vtx) const;
	YsShell2dVertexHandle GetVertexHandleFromId(int vtId) const;

	void RewindVtxPtr(void) const;
	YsShell2dVertexHandle StepVtxPtr(void) const;
	YsShell2dVertexHandle FindNextVertex(YsShell2dVertexHandle currentVtx) const;
	const YsVec3 &GetVertexPosition(YsShell2dVertexHandle vtx) const;
	int GetVertexReferenceCount(YsShell2dVertexHandle vtx) const;
	int GetVertexAttrib1(YsShell2dVertexHandle vtHd) const;
	int GetVertexAttrib2(YsShell2dVertexHandle vtHd) const;
	int GetVertexAttrib3(YsShell2dVertexHandle vtHd) const;
	int GetVertexAttrib4(YsShell2dVertexHandle vtHd) const;
	unsigned GetSearchKey(YsShell2dVertexHandle vtHd) const;
	YsShellVertexHandle GetVtHdMap(YsShell2dVertexHandle vtHd) const;
	YSRESULT SetVertexAttrib1(YsShell2dVertexHandle vtHd,int att) const;
	YSRESULT SetVertexAttrib2(YsShell2dVertexHandle vtHd,int att) const;
	YSRESULT SetVertexAttrib3(YsShell2dVertexHandle vtHd,int att) const;
	YSRESULT SetVertexAttrib4(YsShell2dVertexHandle vtHd,int att) const;
	YSRESULT SetVtHdMap(YsShell2dVertexHandle v2Hd,YsShellVertexHandle vtHd) const;


	int GetEdIdFromHandle(YsShell2dEdgeHandle edHd) const;
	int GetEdgeAttrib1(YsShell2dEdgeHandle edHd) const;
	int GetEdgeAttrib2(YsShell2dEdgeHandle edHd) const;
	int GetEdgeAttrib3(YsShell2dEdgeHandle edHd) const;
	int GetEdgeAttrib4(YsShell2dEdgeHandle edHd) const;
	unsigned GetSearchKey(YsShell2dEdgeHandle edHd) const;
	void *GetEdgeAttribPointer(YsShell2dEdgeHandle edHd) const;
	YSRESULT SetEdgeAttrib1(YsShell2dEdgeHandle edHd,int att) const;
	YSRESULT SetEdgeAttrib2(YsShell2dEdgeHandle edHd,int att) const;
	YSRESULT SetEdgeAttrib3(YsShell2dEdgeHandle edHd,int att) const;
	YSRESULT SetEdgeAttrib4(YsShell2dEdgeHandle edHd,int att) const;
	YSRESULT SetEdgeAttribPointer(YsShell2dEdgeHandle edHd,void *ptr) const;
	YsShell2dEdgeHandle GetEdgeHandleFromId(int edId) const;

	YSRESULT SetEdgeFlag(YsShell2dEdgeHandle edHd,unsigned flag);
	unsigned int GetEdgeFlag(YsShell2dEdgeHandle edHd) const;
	YSRESULT SetEdgeVirtualFlag(YsShell2dEdgeHandle edHd,YSBOOL flag);
	YSBOOL GetEdgeVirtualFlag(YsShell2dEdgeHandle edHd) const;

	void RewindEdgePtr(void) const;
	YsShell2dEdgeHandle StepEdgePtr(void) const;
	YsShell2dEdgeHandle FindNextEdge(YsShell2dEdgeHandle currentEdg) const;
	YsShell2dEdgeHandle FindPrevEdge(YsShell2dEdgeHandle currentEdg) const;
	YSRESULT GetEdgeVertexHandle(YsShell2dVertexHandle &vtx1,YsShell2dVertexHandle &vtx2,YsShell2dEdgeHandle edHd) const;

	YSRESULT GetProjectedVertexPosition(YsVec2 &pnt,int vtId) const;
	YSRESULT GetProjectedVertexPosition(YsVec2 &pnt,YsShell2dVertexHandle vtx) const;

	YSRESULT BuildSearchTable(class YsShell2dSearchTable &search) const;

	const YsVec2 &GetProjectedVertexPosition(int vtId) const;
	const YsVec2 &GetProjectedVertexPosition(YsShell2dVertexHandle vtx) const;

	YSRESULT MakeEdgeChain
	   (YsArray <YsShell2dEdgeHandle,16> &edHdList,YsArray <YsShell2dVertexHandle,16> &vtHdList,
	    YsShell2dEdgeHandle seedEdHd,const double &angleThr) const;
	YSRESULT FindConnectedEdge
	   (YsArray <YsShell2dEdgeHandle,16> &edHdList,YsShell2dEdgeHandle seedEdHd) const;


protected:

	YsListItem <YsShell2dVertex> *CreateVertex(void);
	YsListItem <YsShell2dEdge> *CreateEdge(void);
	void DestroyVertex(YsListItem <YsShell2dVertex> * vtx);
	void DestroyEdge(YsListItem <YsShell2dEdge> * edg);


	YSRESULT ComputeProjection(void) const;
	YSRESULT ComputeBoundingBox(void) const;

	YSRESULT FixLatticeAfterRemovingTriangle
	    (YsShell2dLattice &ltc,YsShell2dVertexHandle v1,YsShell2dVertexHandle v2,YsShell2dVertexHandle v3);
	YSRESULT FixLatticeAfterUndoingTriangle
	    (YsShell2dLattice &ltc,YsShell2dVertexHandle v1,YsShell2dVertexHandle v2,YsShell2dVertexHandle v3);

	YsListItem <YsShell2dVertex> *GetVertexPointerFromHandle(YsShell2dVertexHandle vtHd) const;
	YsListItem <YsShell2dEdge> *GetEdgePointerFromHandle(YsShell2dEdgeHandle vtHd) const;

	mutable YSBOOL projectionIsCached;  // Set FALSE when edited.   Set TRUE when projection is re-computed
	                            // Only ComputeProjection() can set it TRUE

	static YsListAllocator <YsShell2dVertex> vtxAllocator;
	static YsListAllocator <YsShell2dEdge> edgAllocator;
	YsListContainer <YsShell2dVertex> vtxList;
	YsListContainer <YsShell2dEdge> edgList;
	YsPlane basePlane;
	YsMatrix4x4 proj;        // 2D->3D Projection

	unsigned GetUniqueSearchKey(void);
	static unsigned uniqueSearchKeyGenerator;

	mutable YSBOOL boundingBoxIsValid;  // Set TRUE in the ComputeBoundingBox function
	                                    // Becomes FALSE when a vertex is added or deleted
	mutable YsVec2 bbxMin2,bbxMax2;
	mutable YsVec3 bbxMin3,bbxMax3;

	YsShell2dSearchTable *searchTable;
};


////////////////////////////////////////////////////////////

// Idea :
//   Each vertex/edge has unique serial number (kind of ID)
//   Hash vertex/edge by serial number.

class YsShell2dVtxToEdgTable
{
public:
	enum
	{
		HashSize=128
	};

	YsShell2dVtxToEdgTable();
	~YsShell2dVtxToEdgTable();

	YSRESULT Prepare(void);
	YSRESULT AddEdge(const YsShell2d &shl,YsShell2dEdgeHandle edg);
	YSRESULT DeleteEdge(const YsShell2d &shl,YsShell2dEdgeHandle edg);
	const YsArray <YsShell2dEdgeHandle> *GetVtxToEdgTable(YsShell2dVertexHandle vtx) const;

protected:
	YsFixedLengthToMultiHashTable <YsShell2dEdgeHandle,1,1> hashTable;
};

////////////////////////////////////////////////////////////

class YsShell2dEdgeFinder
{
public:
	enum
	{
		HashSize=128
	};

	YsShell2dEdgeFinder();
	~YsShell2dEdgeFinder();

	YSRESULT Prepare(void);
	YSRESULT AddEdge(const YsShell2d &shl,YsShell2dEdgeHandle edg);
	YSRESULT DeleteEdge(const YsShell2d &shl,YsShell2dEdgeHandle edg);
	const YsArray <YsShell2dEdgeHandle> *FindEdge(YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2) const;

protected:
	YsFixedLengthToMultiHashTable <YsShell2dEdgeHandle,2,1> hashTable;
};

////////////////////////////////////////////////////////////

class YsShell2dSearchTable
{
public:
	YSRESULT Prepare(void);
	YSRESULT AddEdge(const YsShell2d &shl,YsShell2dEdgeHandle edg);
	YSRESULT DeleteEdge(const YsShell2d &shl,YsShell2dEdgeHandle edg);
	YSRESULT AddVertex(const YsShell2d &shl,YsShell2dVertexHandle vtx);
	YSRESULT DeleteVertex(const YsShell2d &shl,YsShell2dVertexHandle vtx);

	const YsArray <YsShell2dEdgeHandle> *GetVtxToEdgTable(YsShell2dVertexHandle vtx) const;
	const YsArray <YsShell2dEdgeHandle> *FindEdge(YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2) const;

	YsShell2dEdgeHandle FindEdge(unsigned int key) const;

protected:
	YsShell2dVtxToEdgTable vtxToEdg;
	YsShell2dEdgeFinder edgFind;
	YsHashTable <YsShell2dEdgeHandle> edgeKeyToEdHd;
};


////////////////////////////////////////////////////////////

class YsShell2dTessTriangle
{
public:
	YsShell2dVertexHandle trVtHd[3];

	void Initialize(void);
};

class YsShell2dTessellator
{
public:
	YsListAllocator <YsShell2dTessTriangle> triAlloc;
	YsListContainer <YsShell2dTessTriangle> triList;

	YsShell2d shl2d;
	YsShell2dSearchTable *search2d;
	YsShell2dLattice ltc2d;


	YsShell2dTessellator();
	~YsShell2dTessellator();

	YSRESULT SetDomain(int nVt,const YsVec3 vtPos[]);
	YSRESULT SetDomain(int nVt,const YsVec2 vtPos[]);

	YSRESULT RemoveEdge(YsShell2dEdgeHandle edHd,int searchRange);
};

/* } */
#endif
