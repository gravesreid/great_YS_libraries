#ifndef YSSHELLSEARCH_IS_INCLUDED
#define YSSHELLSEARCH_IS_INCLUDED
/* { */

class YsShellVtxToVtxTable : protected YsHashTable <YsShellVertexHandle>
{
public:
	using YsHashTable <YsShellVertexHandle>::EnableAutoResizing;

	static int nAlloc;

	YsShellVtxToVtxTable(int hashSize);
	~YsShellVtxToVtxTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT DeleteVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YsShellVertexHandle FindVertex(const YsShell &shl,unsigned searchKey) const;
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
};

////////////////////////////////////////////////////////////

class YsShellPlgToPlgTable : protected YsHashTable <YsShellPolygonHandle>
{
public:
	using YsHashTable <YsShellPolygonHandle>::EnableAutoResizing;

	static int nAlloc;

	YsShellPlgToPlgTable(int hashSize);
	~YsShellPlgToPlgTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
	YsShellPolygonHandle FindPolygon(const YsShell &shl,unsigned searchKey) const;
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
};


////////////////////////////////////////////////////////////

class YsShellVtxToPlgTableEntry
{
public:
	static int nAlloc;
	inline YsShellVtxToPlgTableEntry();
	inline ~YsShellVtxToPlgTableEntry();

#ifndef YSLOWMEM
	YsArray <YsShellPolygonHandle> plHdList;
#else
	YsArray <YsShellPolygonHandle,1,short> plHdList;
#endif
};

YsShellVtxToPlgTableEntry::YsShellVtxToPlgTableEntry()
{
	nAlloc++;
}

YsShellVtxToPlgTableEntry::~YsShellVtxToPlgTableEntry()
{
	nAlloc--;
}


class YsShellVtxToPlgTable : protected YsHashTable <YsShellVtxToPlgTableEntry *>
{
public:
	using YsHashTable <YsShellVtxToPlgTableEntry *>::EnableAutoResizing;
	static int nAlloc;

	YsShellVtxToPlgTable(int hashSize);
	~YsShellVtxToPlgTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle vtHd);
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle> *FindPolygonList(const YsShell &shl,YsShellVertexHandle vtHd) const;
#else
	const YsArray <YsShellPolygonHandle,1,short> *FindPolygonList(const YsShell &shl,YsShellVertexHandle vtHd) const;
#endif
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
protected:
	YSRESULT CleanUp(void);
};

////////////////////////////////////////////////////////////

// Here the structure is getting complicated
//   If edVtKey1,edVtKey2 is fixed, searchKey is fixed edVtKey1+edVtKey2.
//   If searchKey is fixed, can find unique group of edges, that is YsShellEdgToPlgTableEdgeList
//   If the group of edges is found, can find polygon list.

class YsShellEdgToPlgTablePolygonList  // Group of polygons that shares a same edge
{
public:
	static int nAlloc;
	inline YsShellEdgToPlgTablePolygonList();
	inline ~YsShellEdgToPlgTablePolygonList();

	YsShellVertexHandle edVtHd1,edVtHd2;
#ifndef YSLOWMEM
	YsArray <YsShellPolygonHandle,2> plgList;
#else
	YsArray <YsShellPolygonHandle,2,short> plgList;
#endif

	YsShellEdgToPlgTablePolygonList *prev,*next;
};

YsShellEdgToPlgTablePolygonList::YsShellEdgToPlgTablePolygonList()
{
	nAlloc++;
}

YsShellEdgToPlgTablePolygonList::~YsShellEdgToPlgTablePolygonList()
{
	nAlloc--;
}



class YsShellEdgToPlgTableEdgeList  // Group of edges that shares same edVtKey1+edVtKey2
{
public:
	static int nAlloc;

	YsShellEdgToPlgTableEdgeList();
	~YsShellEdgToPlgTableEdgeList();
	YsListContainer <YsShellEdgToPlgTablePolygonList> plgGroupList;

};

typedef YsShellEdgToPlgTablePolygonList * YsShellEdgeEnumHandle;


class YsShellEdgToPlgTable : protected YsHashTable <YsShellEdgToPlgTableEdgeList *>
{
public:
	using YsHashTable <YsShellEdgToPlgTableEdgeList *>::EnableAutoResizing;
	static int nAlloc;

	YsShellEdgToPlgTable(int hashSize);
	~YsShellEdgToPlgTable();
	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd);
#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *GetEdgToPlgTable
	    (const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const;
	const YsArray <YsShellPolygonHandle,2> *GetEdgToPlgTable
	    (const YsShell &shl,unsigned vtKey1,unsigned vtKey2) const;
	const YsArray <YsShellPolygonHandle,2> *GetEdgToPlgTable
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *GetEdgToPlgTable
	    (const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const;
	const YsArray <YsShellPolygonHandle,2,short> *GetEdgToPlgTable
	    (const YsShell &shl,unsigned vtKey1,unsigned vtKey2) const;
	const YsArray <YsShellPolygonHandle,2,short> *GetEdgToPlgTable
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#endif
	YSRESULT SelfDiagnostic(const YsShell &shl) const;

	int GetNumEdge(void) const;

	YSRESULT RewindEdgeEnumHandle(YsShellEdgeEnumHandle &handle) const;
	YSRESULT FindNextEdge(YsShellEdgeEnumHandle &handle) const;
	YSRESULT GetEdge(YsShellVertexHandle &edVtHd1,YsShellVertexHandle &edVtHd2,YsShellEdgeEnumHandle &handle) const;
protected:
	YSRESULT CleanUp(void);

	int nEdge;
	YsShellEdgToPlgTablePolygonList *head,*tail;
};

////////////////////////////////////////////////////////////

// Same idea as YsShellEdgToPlgTable
//   If vertex list is fixed, searchKey is fixed. (searchKey=sigma(vertex keys))
//   If searchKey is fixed, one group of polygon group is fixed, that have same searchKey)
//   Among the group, group of polygons can be found. (must have same number of vertices, and same vertices)

// class YsShellVtxLstToPlgTablePolygonGroup // Group of polygons that has a same set of vertices
// {
// public:
// 	YsArray <unsigned,4> plVtKey;
// 	YsArray <YsShellPolygonHandle> plgList;  // Not likely having more than one item in this list as long as no overlap
// };
// 
// class YsShellVtxLstToPlgTableGroupList  // Group of Polygon Groups that has same searchKey
// {
// public:
// 	YsShellVtxLstToPlgTableGroupList();
// 	~YsShellVtxLstToPlgTableGroupList();
// 	YsListContainer <YsShellVtxLstToPlgTablePolygonGroup> plgGroupList;
// };
// 
// class YsShellVtxLstToPlgTable : protected YsHashTable <YsShellVtxLstToPlgTableGroupList *>
// {
// public:
// 	using YsHashTable <YsShellVtxLstToPlgTableGroupList *>::EnableAutoResizing;
// 
// 	YsShellVtxLstToPlgTable(int hashSize);
// 	~YsShellVtxLstToPlgTable();
// 	YSRESULT Prepare(void);
// 	YSRESULT Resize(int newHashSize);
// 
// 	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd);
// 	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd);
// 	const YsArray <YsShellPolygonHandle> *GetVtxLstToPlgTable
// 	    (const YsShell &shl,int nVtHd,const YsShellVertexHandle vtHd[]) const;
// 	const YsArray <YsShellPolygonHandle> *GetVtxLstToPlgTable
// 	    (const YsShell &shl,int nVtKey,const unsigned vtKey[]) const;
// 	YSRESULT SelfDiagnostic(const YsShell &shl) const;
// 
// 	mutable YSBOOL overlapWarning;
// 
// protected:
// 	YSRESULT CleanUp(void);
// 	unsigned GetVertexKeyArray(YsArray <unsigned,4> &keyArray,const YsShell &shl,YsShellPolygonHandle plHd) const;
// 	YSBOOL TwoPolygonsAreEqual(const YsArray <unsigned,4> &key1,const YsArray <unsigned,4> &key2) const;
// };


////////////////////////////////////////////////////////////

class YsShellVtxToEdgTableElem
{
public:
	static int nAlloc;

	YsShellVtxToEdgTableElem();
	~YsShellVtxToEdgTableElem();

	YsShellVertexHandle connVtHd;
	unsigned refCount;
};

inline YsShellVtxToEdgTableElem::YsShellVtxToEdgTableElem()
{
	nAlloc++;
}

inline YsShellVtxToEdgTableElem::~YsShellVtxToEdgTableElem()
{
	nAlloc--;
}

inline int operator==(const YsShellVtxToEdgTableElem &a,const YsShellVtxToEdgTableElem &b)
{
	return a.connVtHd==b.connVtHd;
}

class YsShellVtxToEdgTable : protected YsFixedLengthToMultiHashTable <YsShellVtxToEdgTableElem,1,8>
{
public:
	using YsFixedLengthToMultiHashTable <YsShellVtxToEdgTableElem,1,8>::EnableAutoResizing;

	static int nAlloc;

	YsShellVtxToEdgTable(int hashSize);
	~YsShellVtxToEdgTable();

	YSRESULT Prepare(void);
	YSRESULT Resize(int newHashSize);
	YSRESULT SelfDiagnostic(const YsShell &shl) const;

	YSRESULT AddEdge(const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2);
	YSRESULT DeleteEdge(const YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2);
	const YsArray <YsShellVtxToEdgTableElem,8> *FindEdge(const YsShell &shl,YsShellVertexHandle vtHd) const;
};

////////////////////////////////////////////////////////////


class YsShellSearchTable
{
public:
	YsShellSearchTable();
	YsShellSearchTable(int hashSize);

	static YSRESULT CollectGarbage(void);

	YSRESULT Prepare(void);
	YSRESULT SelfDiagnostic(const YsShell &shl) const;
	void EnableAutoResizing(int minSize,int maxSize);
	void EnableAutoResizing(void);

	YSRESULT AddVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT DeleteVertex(const YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT AddPolygon(const YsShell &shl,YsShellPolygonHandle plHd);
	YSRESULT DeletePolygon(const YsShell &shl,YsShellPolygonHandle plHd);

	YsShellVertexHandle FindVertex(const YsShell &shl,unsigned searchKey) const;

	YsShellPolygonHandle FindPolygon(const YsShell &shl,unsigned searchKey) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle> *FindPolygonListByVertex
	    (const YsShell &shl,YsShellVertexHandle vtHd) const;
#else
	const YsArray <YsShellPolygonHandle,1,short> *FindPolygonListByVertex
	    (const YsShell &shl,YsShellVertexHandle vtHd) const;
#endif
	YSRESULT FindPolygonListByVertex
	    (int &nPl,const YsShellPolygonHandle *&plHdList,const YsShell &shl,YsShellVertexHandle vtHd) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *FindPolygonListByEdge
	    (const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *FindPolygonListByEdge
	    (const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const;
#endif
	YSRESULT FindPolygonListByEdge
	    (int &nPl,const YsShellPolygonHandle *&plHdList,
	     const YsShell &shl,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *FindPolygonListByEdge
	    (const YsShell &shl,unsigned edVt1,unsigned edVt2) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *FindPolygonListByEdge
	    (const YsShell &shl,unsigned edVt1,unsigned edVt2) const;
#endif
	YSRESULT FindPolygonListByEdge
	    (int &nPl,const YsShellPolygonHandle *&plHdList,
	     const YsShell &shl,unsigned edVtKey1,unsigned edVtKey2) const;

#ifndef YSLOWMEM
	const YsArray <YsShellPolygonHandle,2> *FindPolygonListByEdge
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#else
	const YsArray <YsShellPolygonHandle,2,short> *FindPolygonListByEdge
	    (const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
#endif
	YSRESULT FindPolygonListByEdge
	    (int &nPl,const YsShellPolygonHandle *&plHdList,
	     const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
	int GetNumPolygonUsingEdge(const YsShell &shl,const YsShellEdgeEnumHandle &handle) const;
	int GetNumPolygonUsingEdge(const YsShell &shl,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	int GetNumPolygonUsingEdge(const YsShell &shl,unsigned int edVtKey1,unsigned int edVtKey2) const;

	YsShellPolygonHandle GetNeighborPolygon(const YsShell &shl,YsShellPolygonHandle plHd,int n) const;
	YsShellPolygonHandle GetNeighborPolygon(const YsShell &shl,YsShellPolygonHandle plHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const;
	YSBOOL CheckPolygonNeighbor(YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2,const YsShell &shl) const;

// 	const YsArray <YsShellPolygonHandle> *FindPolygonListByVertexList
// 	    (const YsShell &shl,int nVtx,const YsShellVertexHandle vtHd[]) const;
// 	YSRESULT FindPolygonListByVertexList
// 	    (int &nPl,const YsShellPolygonHandle *&plHdList,
// 	     const YsShell &shl,int nVtx,const YsShellVertexHandle vtHd[]) const;

	const YsArray <YsShellVtxToEdgTableElem,8> *FindEdge(const YsShell &shl,YsShellVertexHandle vtHd) const;

	YSRESULT GetConnectedVertexList
    	(YsArray <YsShellVertexHandle,16> &connLst,const YsShell &shl,YsShellVertexHandle vtHd) const;

	int GetNumEdge(const YsShell &shl) const;

	YSRESULT RewindEdgeEnumHandle
	    (const YsShell &shl,YsShellEdgeEnumHandle &handle) const;
	YSRESULT FindNextEdge
	    (const YsShell &shl,YsShellEdgeEnumHandle &handle) const;
	YSRESULT GetEdge
	    (const YsShell &shl,unsigned &edVtKey1,unsigned &edVtKey2,YsShellEdgeEnumHandle &handle) const;
	YSRESULT GetEdge
	    (const YsShell &shl,
	     YsShellVertexHandle &edVtKey1,YsShellVertexHandle &edVtKey2,YsShellEdgeEnumHandle &handle) const;

	YSBOOL CheckPolygonExist(const YsShell &shl,int nPlVt,const YsShellVertexHandle plVtHd[]) const;
	YSRESULT FindPolygonListByVertexList
	    (YsArray <YsShellPolygonHandle,16> &plHdList,
	     const YsShell &shl,int nPlVt,const YsShellVertexHandle plVtHd[]) const;

// 	YSBOOL GetOverlapWarning(void) const;
// 	void ClearOverlapWarning(void) const;

protected:
	YsShellVtxToVtxTable vtxToVtx;
	YsShellVtxToEdgTable vtxToEdg;
	YsShellPlgToPlgTable plgToPlg;
	YsShellVtxToPlgTable vtxToPlg;
	YsShellEdgToPlgTable edgToPlg;
// 	YsShellVtxLstToPlgTable vtxLstToPlg;
};




/* } */
#endif
