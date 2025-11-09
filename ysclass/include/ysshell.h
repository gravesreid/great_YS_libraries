#ifndef YSSHELL_IS_INCLUDED
#define YSSHELL_IS_INCLUDED
/* { */

#ifdef YS_COMPILING_YSSHELL_CPP
class YsShell;
static YSRESULT YsBlendShell1to2
    (YsShell &newShell,
     const YsShell &sh1,const YsShell &sh2,int f,YSBOOL elimOverlap);
#endif


// YsBaseClass
//   YsShell

// Declaration /////////////////////////////////////////////

class YsShellSpecialAttribute
	// If an application wants to have special attributes :
	//     Inherit YsShell class and override functions
	//             YsShell::AttachSpecialAttributeToNewVertex(),
	//             YsShell::AttachSpecialAttributeToNewPolygon(),
	//             YsShell::DetachSpecialAttributeBeforeDeletingVertex() and
	//             YsShell::AttachSpecialAttributeBeforeDeletingPolygon().
	//      In this case, special attribute must be created
	//      and attached inside AttachSpecialAttributeToNew?????()
	//      and must be detach & deleted inside DetachSpecialAttributeBeforeDeleting?????().
	//      To detach, program must call function YsShellItem::SpecialAttributeDetached().
	//      This will guarantee that all vertices and polygons have
	//      necessary attributes.
{
};

class YsShellItem
{
public:
	YsShellItem();
	~YsShellItem();
	void Initialize(void);
	unsigned GetSearchKey(void) const;

	void CopyAttribute(const YsShellItem &from);

	int freeAttribute0; // Use free for any purpose
	int freeAttribute1; // Use free for any purpose
	int freeAttribute2; // Use free for any purpose
	mutable int freeAttribute3; // Use free for any purpose
	mutable int freeAttribute4;

	unsigned searchKey; // Key for hash search

	// const class YsShell *belongTo;  // belongTo is set only in CreateVertex
	// 2006/02/07 Use vtHd/plHd->GetContainer() instead of belongTo

	YSRESULT SetSpecialAttribute(YsShellSpecialAttribute *attrib);
	YsShellSpecialAttribute *GetSpecialAttribute(void);
	const YsShellSpecialAttribute *GetSpecialAttribute(void) const;
	void SpecialAttributeDetached(void);

protected:
	// YsShellItem will initialize 'special' to NULL.
	// But never delete it.
	YsShellSpecialAttribute *special;
};

// Declaration /////////////////////////////////////////////
class YsShellVertex : public YsShellItem
{
public:
	YsShellVertex();
	void Initialize(void);

	YSRESULT IncrementReferenceCount(void);
	YSRESULT DecrementReferenceCount(void);
	YSRESULT SetPosition(const YsVec3 &pos);
	YSRESULT SetNormal(const YsVec3 &nom);
	const YsVec3 &GetPosition(void) const;
	const YsVec3 &GetNormal(void) const;
	int GetReferenceCount(void) const;

	YSRESULT SetReferenceCount(int n);  // This function must be eliminated soon

	const YsListItem <YsShellVertex> *mapper;     // Mapping purpose. Volatile
	union
	{
		const YsListItem <class YsShell2dVertex> *v2HdMap;  // 2005/01/21 For co-operation with YsShell2d
		const YsListItem <class YsMeshNode> *ndHd;          // 2006/09/12 For co-operation with YsHexDominantMesh
	};

protected:
	YsVec3 pos;
	int refCount;
#ifndef YSLOWMEM
	YsVec3 nom;
#endif
};

// Declaration /////////////////////////////////////////////
class YsShellPolygon : public YsShellItem
{
public:
	YsShellPolygon();
	void Initialize(void);

	YSRESULT SetNormal(const YsVec3 &n);
	const YsVec3 &GetNormal(void) const;
	YSRESULT SetColor(const YsColor &c);
	const YsColor &GetColor(void) const;
	YSRESULT SetVertexList(const class YsShell &shl,int nVtId,const int vtId[]);
	YSRESULT SetVertexList(int nVtx,const YsShellVertexHandle vtx[]);
	int GetNumVertex(void) const;
	const YsShellVertexHandle *GetVertexArray(void) const;
	int GetVertexId(int i)const;
	YsShellVertexHandle GetVertexHandle(int i) const;
	void Invert(void);

	const YsListItem <class YsMeshElem2d> *el2dHd;  // 2006/09/12 For co-operation with YsHexDominantMesh

protected:
	YsVec3 nom;
	YsArray <YsShellVertexHandle,4> idx;
#ifndef YSLOWMEM
	YsColor col;
#endif
};

class YsShellReadSrfVariable
{
public:
	YsShellReadSrfVariable();

	YsArray <YsString,16> args;
	int mode;
	YsShellPolygonHandle plHd;
	YSBOOL noShadingPolygon;
	YsArray <YsShellVertexHandle,16> plVtHdList;
	YsArray <YsShellVertexHandle,16> allVtHdList;
	YsColor color;
	YsVec3 nom;
};


////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

class YsShellRepairFlipDirectionInfo
{
public:
	YsShellPolygonHandle plHd;
	YsShellVertexHandle vtHd1,vtHd2;
};

class YsShellVertexHandleKeyPair
{
public:
	YsShellVertexHandle vtHd;
	unsigned searchKey;
};

class YsShellPolygonHandlehKeyPair
{
public:
	YsShellVertexHandle plHd;
	unsigned searchKey;
};

typedef enum 
{
	YSSHELL_NOELEM,
	YSSHELL_VERTEX,
	YSSHELL_EDGE,
	YSSHELL_POLYGON
} YSSHELLELEMTYPE;

class YsShellElemType
{
public:
	YSSHELLELEMTYPE elemType;
	union
	{
		YsShellVertexHandle vtHd;
		YsShellVertexHandle edVtHd[2];
		YsShellPolygonHandle plHd;
	};
};

// Declaration /////////////////////////////////////////////
class YsShellSearchTable;


class YsShell
{
// CAUTION!
// Whenever add an item, do not forget to modify operator=

public:
	YsShell();
	YsShell(const YsShell &from);
	~YsShell();

	static YsListAllocator <YsShellVertex> vertexStore;
	static YsListAllocator <YsShellPolygon> polygonStore;


	YSRESULT AttachSearchTable(YsShellSearchTable *search) const;
	  // NOTE:YsShell will not automatically release memory of search table
	  //      YsShell user is responsible for releasing memory block assigned to the search table
	YSRESULT DetachSearchTable(void) const;
	const YsShellSearchTable *GetSearchTable(void) const;


// Modifiers
public:
	const YsShell &operator=(const YsShell &from);
	void CleanUp(void);
	int AddVertex(const YsVec3 &vec);
	// int AddPolygon(int nv,const int v[]);   2005/01/19 Deleted
	int AddPolygonVertex(int nv,const YsVec3 v[]);
	YsShellVertexHandle AddVertexH(const YsVec3 &vec);
	YsShellPolygonHandle AddPolygonH(int nv,const int v[]);
	YsShellPolygonHandle AddPolygonH(int nv,const YsShellVertexHandle v[]);
	YsShellPolygonHandle AddPolygonVertexH(int nv,const YsVec3 v[]);
	YSRESULT ModifyVertexPosition(int vtId,const YsVec3 &neoPos);
	YSRESULT ModifyVertexPosition(YsShellVertexHandle vtHd,const YsVec3 &neoPos);
	YSRESULT ModifyPolygon(int plId,int nv,const int v[]);
	YSRESULT ModifyPolygon(YsShellPolygonHandle plHd,int nv,const YsShellVertexHandle v[]);
	YSRESULT DeletePolygon(int plId);
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);
	YSRESULT DeleteVertex(int vtId);
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);
	YSRESULT DeleteVertexAtTheSamePosition(int nLatX=32,int nLatY=32,int nLatZ=32);
	YSRESULT DeleteVertexAtTheSamePosition(int &nDeleted,int nLatX=32,int nLatY=32,int nLatZ=32);
	YSRESULT MakeVertexMappingToDeleteVertexAtTheSamePositionByKdTree(const double &tol);
	YSRESULT DeleteVertexAtTheSamePositionByKdTree(int &nDeleted,const double &tol=YsTolerance);
	YSRESULT RepairPolygonOrientationBasedOnNormal(void);  // search table required
	YSRESULT RepairPolygonOrientationAssumingClosedShell(void);  // search table required
	YSRESULT RepairPolygonOrientationBasedOnNormal(YsArray <YsShellPolygonHandle> &invertedPlHd);
	YSRESULT RepairPolygonOrientationAssumingClosedShell
	    (YsArray <YsShellPolygonHandle> &invertedPlHdList);  // search table required
	YSBOOL IsOverlappingPolygon(YsShellPolygonHandle plHd);  // search table required
	YSRESULT OmitPolygonAfterBlend(void);
	void SetMatrix(const YsMatrix4x4 &mat);
	YSRESULT Triangulate(YsArray <YsShellPolygonHandle> &newPlHd,YsShellPolygonHandle plHd,YSBOOL killOriginalPolygon);

	void SetTrustPolygonNormal(YSBOOL trust);
	YSBOOL TrustPolygonNormal(void) const;

	YSRESULT FreezeVertex(YsShellVertexHandle vtHd);
	YSRESULT FreezePolygon(YsShellPolygonHandle plHd);
	YSRESULT MeltVertex(YsShellVertexHandle vtHd);
	YSRESULT MeltPolygon(YsShellPolygonHandle plHd);
	YSRESULT DeleteFrozenVertex(YsShellVertexHandle vtHd);
	YSRESULT DeleteFrozenPolygon(YsShellPolygonHandle plHd);
	YSRESULT DisposeFrozenPolygon(void);
	YSRESULT DisposeFrozenVertex(void);

public:
	YsShellSpecialAttribute *GetSpecialAttributeVertex(YsShellVertexHandle vtHd);
	const YsShellSpecialAttribute *GetSpecialAttributeVertex(YsShellVertexHandle vtHd) const;
	YsShellSpecialAttribute *GetSpecialAttributePolygon(YsShellPolygonHandle plHd);
	const YsShellSpecialAttribute *GetSpecialAttributePolygon(YsShellPolygonHandle plHd) const;

	YSRESULT SetSpecialAttributeVertex(YsShellVertexHandle vtHd,YsShellSpecialAttribute *attr);
	YSRESULT SetSpecialAttributePolygon(YsShellPolygonHandle plHd,YsShellSpecialAttribute *attr);
protected:
	virtual YSRESULT AttachSpecialAttributeToNewVertex(YsShellVertex &vtx,YsShellVertexHandle vtHd);
	virtual YSRESULT AttachSpecialAttributeToNewPolygon(YsShellPolygon &plg,YsShellPolygonHandle plHd);
	virtual YSRESULT DetachSpecialAttributeBeforeDeletingVertex(YsShellVertex &vtx);
	virtual YSRESULT DetachSpecialAttributeBeforeDeletingPolygon(YsShellPolygon &plg);

protected:
	YsShellPolygonHandle AddPolygonVertex(int nv,const YsVec3 v[],int fa3,const int fa4[]);

	mutable YsShellSearchTable *searchTable;

	YSBOOL FindRepairFlipDirectionOfOneReliablePolygon
	    (YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellPolygonHandle plHd);
	YSRESULT RecursivelyRepairFlipDirection
	    (int &nDone,YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellPolygonHandle plHd);
	YSRESULT RepairFlipDirectionOfOnePolygon
	    (YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellRepairFlipDirectionInfo &info);
	YSRESULT AddNeighborToList
	    (YsArray <YsShellRepairFlipDirectionInfo,4096> &todo,YsShellPolygonHandle plHd);

public:
	YSRESULT SaveFreeAttribute4OfPolygon(YsArray <int> &attrib4) const;
	YSRESULT RestoreFreeAttribute4OfPolygon(YsArray <int> &attrib4) const;
	YSRESULT ClearFreeAttribute4OfPolygon(int attrib4);
	YSRESULT SaveFreeAttribute4OfVertex(YsArray <int> &attrib4) const;
	YSRESULT RestoreFreeAttribute4OfVertex(YsArray <int> &attrib4) const;
	YSRESULT ClearFreeAttribute4OfVertex(int attrib4);

protected:
	YSRESULT SaveFreeAttribute4OfPolygonAndNeighbor(
	    YsArray <YsPair <YsShellPolygonHandle,int>,64> &savePlAtt4,
	    int nPl,const YsShellPolygonHandle plHdList[],int freeAtt4Value) const;
	YSRESULT RestoreFreeAttribute4OfPolygon(int nSavePlAtt4,const YsPair <YsShellPolygonHandle,int> savePlAtt4[]) const;
	YSRESULT SaveFreeAttribute4OfVertexAndNeighbor(
	    YsArray <YsPair <YsShellVertexHandle,int>,64> &saveVtAtt4,
	    int nVt,const YsShellVertexHandle vtHdList[],int freeAtt4Value) const;
	YSRESULT RestoreFreeAttribute4OfVertex(int nSaveVtAtt4,const YsPair <YsShellVertexHandle,int> saveVtAtt4[]) const;

// Non modifiers
public:
	void Encache(void) const;
	void Decache(void) const;

	void RewindVtxPtr(void) const;
	YsShellVertexHandle StepVtxPtr(void) const;
	void RewindPlgPtr(void) const;
	YsShellPolygonHandle StepPlgPtr(void) const;

	YsShellVertexHandle FindNextVertex(YsShellVertexHandle current) const;
	YsShellVertexHandle FindPrevVertex(YsShellVertexHandle current) const;
	YsShellPolygonHandle FindNextPolygon(YsShellPolygonHandle current) const;
	YsShellPolygonHandle FindPrevPolygon(YsShellPolygonHandle current) const;
	int GetVertexIdFromHandle(YsShellVertexHandle vtx) const;
	int GetPolygonIdFromHandle(YsShellPolygonHandle plg) const;
	YsShellVertexHandle GetVertexHandleFromId(int vtId) const;
	YsShellPolygonHandle GetPolygonHandleFromId(int plId) const;


	YSRESULT GetNormalOfPolygon(YsVec3 &nom,int plId) const;
	YSRESULT GetColorOfPolygon(YsColor &col,int plId) const;
	YSRESULT GetNormalOfPolygon(YsVec3 &nom,YsShellPolygonHandle plHd) const;
	YSRESULT GetColorOfPolygon(YsColor &col,YsShellPolygonHandle plHd) const;
	YSRESULT ComputeNormalOfPolygonFromGeometry(YsVec3 &nom,YsShellPolygonHandle plHd) const;
	YSRESULT MakePolygonKeyNormalPairFromGeometry(YsPair <unsigned int,YsVec3> &plKeyNomPair,YsShellPolygonHandle plHd) const;
	YSRESULT MakePolygonKeyNormalPairListFromGeometry(YsArray <YsPair <unsigned int,YsVec3> > &plKeyNomPair,int nPl,const YsShellPolygonHandle plHdList[]) const;
	double ComputeDihedralAngle(YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2) const;
	double ComputeEdgeLength(YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const;
	double ComputeEdgeLength(int nVt,const YsShellVertexHandle vtHd[]) const;
	double ComputeAngleCos(const YsShellVertexHandle vtHd[3]) const;  // Straight -> 0.0
	double ComputeMinimumAngleCos(YsShellPolygonHandle plHd) const;
	double ComputeMinimumAngleCos(int nPlVt,const YsShellVertexHandle plVtHd[]) const;
	double ComputeVolume(void) const;

	const YsVec3 &GetCenterOfPolygon(YsVec3 &cen,YsShellPolygonHandle plHd) const;
	const YsVec3 &GetCenterOfEdge(YsVec3 &mid,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	const YsVec3 &GetCenterOfVertexGroup(YsVec3 &cen,int nVt,const YsShellVertexHandle vtHdList[]) const;
	const YsVec3 &GetCenterOfTriangle(YsVec3 &cen,YsShellVertexHandle trVtHd1,YsShellVertexHandle trVtHd2,YsShellVertexHandle trVtHd3) const;
	const YsVec3 &GetArbitraryInsidePointOfPolygon(YsVec3 &isp,YsShellPolygonHandle plHd) const;
	YSRESULT GetArbitraryInsidePointOfPolygonOnPlane(YsVec3 &isp,YsShellPolygonHandle plHd,const YsPlane &pln) const;

	const double GetEdgeSquareLength(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	const double GetEdgeSquareLength(YsShellVertexHandle edVtHd[2]) const;

	const double GetEdgeLength(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	const double GetEdgeLength(YsShellVertexHandle edVtHd[2]) const;
	const double GetEdgeLength(int nVt,const YsShellVertexHandle edVtHd[]) const;
	const double GetLoopLength(int nVt,const YsShellVertexHandle edVtHd[]) const;

	YSRESULT SetNormalOfPolygon(int plId,const YsVec3 &nom);
	YSRESULT SetColorOfPolygon(int plId,const YsColor &col);
	YSRESULT SetNormalOfPolygon(YsShellPolygonHandle plHd,const YsVec3 &nom);
	YSRESULT SetColorOfPolygon(YsShellPolygonHandle plHd,const YsColor &col);

	YSRESULT ValidateVtId(int nVtId,const int vtId[]) const;
	YSRESULT ValidatePlId(int nPlId,const int plId[]) const;

	YSRESULT CheckPotentialPolygon(int nVt,const YsShellVertexHandle vt[],const double &cosThr);

	int GetNumPolygon(void) const;
	int GetNumVertex(void) const;
	int GetMaxNumVertexOfPolygon(void) const;

	YSRESULT GetVertexPosition(YsVec3 &pos,int vtId) const;
	YSRESULT GetVertexPosition(YsVec3 &pos,YsShellVertexHandle vtHd) const;
	int GetVertexReferenceCount(int vtId) const;
	int GetVertexReferenceCount(YsShellVertexHandle vtHd) const;

	// 10/22/2001
	YSRESULT GetAverageNormalAtVertex(YsVec3 &nom,YsShellVertexHandle vtHd) const;

	unsigned GetSearchKey(YsShellVertexHandle vtHd) const;
	unsigned GetSearchKey(YsShellPolygonHandle plHd) const;

	void GetBoundingBox(YsVec3 &min,YsVec3 &max) const;

	int GetNumVertexOfPolygon(int plId) const;
	int GetNumVertexOfPolygon(YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(int &nPlVt,const YsShellVertexHandle *&plVtHd,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(int idx[],int maxcount,int plId) const;
	YSRESULT GetVertexListOfPolygon(YsVec3 vtx[],int maxcount,int plId) const;
	const YsShellVertexHandle *GetVertexListOfPolygon(YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(YsShellVertexHandle vtHd[],int maxcount,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(YsVec3 vtx[],int maxcount,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(YsArray <YsShellVertexHandle> &vtHd,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(YsArray <YsVec3> &vtx,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListFromPolygonList(YsArray <YsShellVertexHandle> &vtHdList,int nPl,const YsShellPolygonHandle plHdList[]) const;
	YSRESULT GetVertexListOfPolygon(YsArray <YsShellVertexHandle,16> &vtHd,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListOfPolygon(YsArray <YsVec3,16> &vtx,YsShellPolygonHandle plHd) const;
	YSRESULT GetVertexListFromPolygonList(YsArray <YsShellVertexHandle,16> &vtHdList,int nPl,const YsShellPolygonHandle plHdList[]) const;
	double GetAreaOfPolygon(YsShellPolygonHandle plHd) const;
	double GetHeightOfPolygon(YsShellPolygonHandle plHd,YsShellVertexHandle baseEdVtHd[2]) const;
	double GetPolygonEdgeLength(YsShellPolygonHandle plHd,int edId) const;
	inline YSRESULT GetPolygonEdgeVertex(YsShellVertexHandle edVtHd[2],YsShellPolygonHandle plHd,int edId) const;
	inline YSRESULT GetPolygonEdgeVertex(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellPolygonHandle plHd,int edId) const;

	inline YsShellVertexHandle PlVt(YsShellPolygonHandle plHd,int idx) const;
	inline const YsVec3 &PlVtPos(YsShellPolygonHandle plHd,int idx) const;
	inline YsShellVertexHandle PlVtLp(YsShellPolygonHandle plHd,int idx) const;
	inline const YsVec3 &PlVtPosLp(YsShellPolygonHandle plHd,int idx) const;


	YSRESULT GetPolygonGroupByNormalSimilarity
	   (YsArray <YsShellPolygonHandle,16> &plHdList,YsShellPolygonHandle seedPlHd,
	    YsKeyStore *excludePlKey=NULL,const double &cosThr=1.0-YsTolerance) const;
	YSRESULT GetCircumferentialAngleOfPolygonGroup
	   (double &minAngle,YsShellVertexHandle &minVtHd,double &maxAngle,YsShellVertexHandle &maxVtHd,
	    const YsVec3 &org,const YsVec3 &axs,int nPl,const YsShellPolygonHandle *plHdList) const;

	void GetMatrix(YsMatrix4x4 &mat) const;

	YsShellPolygon *GetPolygon(int id);
	YsShellVertex *GetVertex(int id);
	const YsShellPolygon *GetPolygon(int id) const;
	const YsShellVertex *GetVertex(int id) const;
	YsShellPolygon *GetPolygon(YsShellPolygonHandle plg);
	YsShellVertex *GetVertex(YsShellVertexHandle vtx);
	const YsShellPolygon *GetPolygon(YsShellPolygonHandle plg) const;
	const YsShellVertex *GetVertex(YsShellVertexHandle vtx) const;

	YSBOOL IsFrozen(YsShellPolygonHandle plHd) const;
	YSBOOL IsFrozen(YsShellVertexHandle vtHd) const;

	YSSIDE CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,int plIdSkip=-1,YSBOOL watch=YSFALSE) const;
	YSSIDE CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,YsShellPolygonHandle plHdSkip,YSBOOL watch=YSFALSE) const;

	YSSIDE CheckInsideSolid(const YsVec3 &pnt,YSBOOL watch=YSFALSE) const;

	YSRESULT InvertPolygon(int plId);
	YSRESULT InvertPolygon(YsShellPolygonHandle plHd);


	YSRESULT ShootFiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShellPolygonHandle> &plHdLst,
	     const YsVec3 &p1,const YsVec3 &p2,const class YsShellLattice *ltc=NULL) const;
	YSRESULT ShootInfiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShellPolygonHandle> &plHdLst,
	     const YsVec3 &org,const YsVec3 &vec,const class YsShellLattice *ltc=NULL) const;




	int ShootRay(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const;
	YsShellPolygonHandle ShootRayH(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const;
	    // Return PlId or PlHd
	void ShootRay(YsArray <YsVec3> &intersect,const YsVec3 &p1,const YsVec3 &vec) const;


	YSRESULT BuildOctree(class YsShellOctree &oct,int maxDepth,double bbxMargine=1.02) const;

	YSBOOL IsStarShape(const YsVec3 &withRespectTo) const;

	YSRESULT AutoComputeNormal(YsShellPolygonHandle plHd,YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal);
	YSRESULT AutoComputeNormalAll(YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal);

	YSRESULT AutoComputeVertexNormalAll(YSBOOL weighted);

protected:
// Members to copy in operator= {
	YSBOOL matrixIsSet;
	YsMatrix4x4 mat;
	YsListContainer <YsShellPolygon> plg;
	YsListContainer <YsShellVertex> vtx;
// }

	// In order to help implement Undo/Redo
	YsListContainer <YsShellPolygon> frozenPlg;
	YsListContainer <YsShellVertex> frozenVtx;

	YSRESULT BlendPolygonVtId
	    (int &nvNew,YsShellVertexHandle vtIdNew[],int nBuf,
	     int nvOrg,const YsShellVertexHandle vtIdOrg[],int nv,const YsShellVertexHandle vtId[]);
	YSBOOL EliminateHangnail(int &nv,YsShellVertexHandle v[]);


	unsigned uniqueSearchKey;
	unsigned GetUniqueSearchKey(void);
	void RewindSearchKey(void);

	YSBOOL trustPolygonNormal;   // Added 2000/11/10

// File IO
public:
	YsShellReadSrfVariable *readSrfVar;

	YSRESULT BeginReadSrf(void);
	YSRESULT BeginMergeSrf(void);
	YSRESULT ReadSrfOneLine
	 (const char str[],
	  YsArray <YsShellPolygonHandle> *noShadingPolygon=NULL,YsArray <YsShellVertexHandle> *roundVtx=NULL);
	YSRESULT EndReadSrf(void);

	YSRESULT LoadSrf(const char fn[],
	    YsArray <YsShellPolygonHandle> *noShadingPolygon=NULL,YsArray <YsShellVertexHandle> *roundVtx=NULL);
	YSRESULT MergeSrf(const char fn[],
	    YsArray <YsShellPolygonHandle> *noShadingPolygon=NULL,YsArray <YsShellVertexHandle> *roundVtx=NULL);
	YSRESULT LoadTri(const char fn[]);
	YSRESULT LoadMsh(const char fn[]);
	YSRESULT MergeMsh(const char fn[]);
	YSRESULT LoadMs3(const char fn[]);
	YSRESULT MergeMs3(const char fn[]);
	YSRESULT LoadDat(const char fn[]);
	YSRESULT MergeDat(const char fn[]);
	YSRESULT SaveSrf
	   (const char fn[],
	    int nNoShadingPolygon=0,const YsShellPolygonHandle noShadingPolygon[]=NULL,
	    int nRoundVtx=0,const YsShellVertexHandle roundVtx[]=NULL) const;
	YSRESULT SaveMsh(int &nIgnored,const char fn[]) const;
	YSRESULT SaveMs3(int &nIgnored,const char fn[]) const;

	YSRESULT LoadStl(const char fn[]);
	YSRESULT MergeStl(const char fn[]);
	YSRESULT SaveStl(int &nIgnored,const char fn[],const char title[]=NULL) const;  // <- This function ignores non-triangular faces.
	YSRESULT SaveStl(const char fn[],const char title[]=NULL) const;  // <- This function ignores non-triangular faces.
	YSRESULT SaveBinStl(int &nIgnored,const char fn[],const char title[]=NULL) const;
	YSRESULT SaveBinStl(const char fn[],const char title[]=NULL) const;

	friend YSRESULT YsBlendShell1to2
	    (YsShell &,const YsShell &,const YsShell &,int,YSBOOL);



public:
	static void CleanUpVertexPolygonStore(void);
protected:
	YsListItem <YsShellPolygon> *CreatePolygon(void);
	YsListItem <YsShellVertex> *CreateVertex(void);

	// Bounding box becomes valid when
	//    GetBoundingBox function computes bounding box.
	// becomes invalid when
	//    a vertex position is modified
	//    a matrix is set
	//    a vertex is deleted
	//    the shell is cleaned up
	// automatically updated when
	//    a vertex is added outside the bounding box
	mutable YSBOOL bbxIsValid;
	mutable YsVec3 bbx1,bbx2;
};

inline YSRESULT YsShell::GetPolygonEdgeVertex(YsShellVertexHandle edVtHd[2],YsShellPolygonHandle plHd,int edId) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
	if(0<=edId && edId<nPlVt-1)
	{
		edVtHd[0]=plVtHd[edId];
		edVtHd[1]=plVtHd[edId+1];
		return YSOK;
	}
	else if(edId==nPlVt-1)
	{
		edVtHd[0]=plVtHd[nPlVt-1];
		edVtHd[1]=plVtHd[0];
		return YSOK;
	}
	edVtHd[0]=NULL;
	edVtHd[1]=NULL;
	return YSERR;
}

inline YSRESULT YsShell::GetPolygonEdgeVertex(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellPolygonHandle plHd,int edId) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
	if(0<=edId && edId<nPlVt-1)
	{
		edVtHd1=plVtHd[edId];
		edVtHd2=plVtHd[edId+1];
		return YSOK;
	}
	else if(edId==nPlVt-1)
	{
		edVtHd1=plVtHd[nPlVt-1];
		edVtHd2=plVtHd[0];
		return YSOK;
	}
	edVtHd1=NULL;
	edVtHd2=NULL;
	return YSERR;
}


inline YsShellVertexHandle YsShell::PlVt(YsShellPolygonHandle plHd,int idx) const
{
	return plHd->dat.GetVertexHandle(idx);
}

inline const YsVec3 &YsShell::PlVtPos(YsShellPolygonHandle plHd,int idx) const
{
	return plHd->dat.GetVertexHandle(idx)->dat.GetPosition();
}

inline YsShellVertexHandle YsShell::PlVtLp(YsShellPolygonHandle plHd,int idx) const
{
	return plHd->dat.GetVertexHandle(idx%plHd->dat.GetNumVertex());
}

inline const YsVec3 &YsShell::PlVtPosLp(YsShellPolygonHandle plHd,int idx) const
{
	return plHd->dat.GetVertexHandle(idx%plHd->dat.GetNumVertex())->dat.GetPosition();
}


////////////////////////////////////////////////////////////


// Declaration /////////////////////////////////////////////

class YsShellEdge
{
public:
	int edId;
	YsShellVertexHandle edVt1,edVt2;
	YsArray <YsShellPolygonHandle> usedBy;
};

class YsShellEdgeList : public YsBaseClass
{
	enum
	{
		HashTableSize=16384,
	};

public:
	YsShellEdgeList();
	YsShellEdgeList(const YsShellEdgeList &from);
	~YsShellEdgeList();

	int GetNumEdge(void) const;
	YSRESULT CleanUp(void);
	YSRESULT Build(const YsShell &shl);
	YSRESULT GetEdge(YsShellVertexHandle &edVt1,YsShellVertexHandle &edVt2,int edId) const;
	YSRESULT LookUpEdge(int &edId,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YSBOOL takeReverse=YSTRUE) const;
	YSRESULT LookUpEdge(YsShellEdge *&edge,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YSBOOL takeReverse=YSTRUE) const;

	const YsShellEdge *GetEdge(int edId) const;

protected:
	YSRESULT AddEdge(YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2,YsShellPolygonHandle plId,const YsShell &shl);

	int nEdge;
	YsArray <YsShellEdge *> edgeHash[HashTableSize]; // Hash by (edVtKey1+edVtKey2)%HashTableSize
	YsList <YsShellEdge> *edgeList;
	const YsShell *shell;
};


////////////////////////////////////////////////////////////

class YsShellVertexStore : public YsKeyStore
{
public:
	YsShellVertexStore(const YsShell &shl);
	const YsShell &GetShell(void) const;
	YSRESULT AddVertex(YsShellVertexHandle vtHd);
	YSRESULT AddVertex(int nVt,const YsShellVertexHandle vtHd[]);
	YSRESULT DeleteVertex(YsShellVertexHandle vtHd);
	YSBOOL IsIncluded(YsShellVertexHandle vtHd) const;
protected:
	const YsShell *shl;
};

class YsShellPolygonStore : public YsKeyStore
{
public:
	YsShellPolygonStore(const YsShell &shl);
	const YsShell &GetShell(void) const;
	YSRESULT AddPolygon(YsShellPolygonHandle plHd);
	YSRESULT AddPolygon(int nPl,const YsShellPolygonHandle plHd[]);
	YSRESULT DeletePolygon(YsShellPolygonHandle plHd);
	YSBOOL IsIncluded(YsShellPolygonHandle plHd) const;
protected:
	const YsShell *shl;
};

class YsShellEdgeStore : public YsTwoKeyStore
{
public:
	YsShellEdgeStore(const YsShell &shl);
	const YsShell &GetShell(void) const;
	YSRESULT AddEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);
	YSRESULT AddEdge(const YsShellVertexHandle edVtHd[2]);
	YSRESULT DeleteEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2);
	YSRESULT DeleteEdge(const YsShellVertexHandle edVtHd[2]);
	YSBOOL IsIncluded(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	YSBOOL IsIncluded(const YsShellVertexHandle edVtHd[2]);
protected:
	const YsShell *shl;
};

////////////////////////////////////////////////////////////


YSBOOL YsCheckShellCollision(YsShell &sh1,YsShell &sh2);
	// Note:This function will use freeAttribute3 of YsShellPolygon

YSRESULT YsBlendShell
    (YsShell &newShell,
     const YsShell &sh1,
     const YsShell &sh2,
     YSBOOLEANOPERATION boolop=YSBOOLBLEND);
    // Note:This function will use freeAttribute2 and 3.

YSRESULT YsSeparateShell
    (YsShell &sh1,YsShell &sh2,const YsShell &org,const YsShell &ref);
    // Note:This function will use freeAttribute2,3 and 4.
    // sh1=org-ref
    // sh2=org&ref

YSRESULT YsReplaceShellEdge
    (YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2,int nNewEdVt,const YsShellVertexHandle newEdVt[]);
YSRESULT YsAddShell(YsShell &shl1,const YsShell &shl2);

YSRESULT YsBlendShell2
    (YsShell &newShell,const YsShell &shl0,const YsShell &shl1,YSBOOLEANOPERATION boolop,
     int ltcX,int ltcY,int ltcZ,int ltcHashSize);

YSRESULT YsBlendShellPolygon
    (YsArray <YsShellPolygonHandle> &newPlHd,YsShell &shl,const YsShellPolygonHandle plHd[2],YsShellLattice *ltc);

/* } */
#endif
