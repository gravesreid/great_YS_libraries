#ifndef YSSHELLOCTREE_IS_INCLUDED
#define YSSHELLOCTREE_IS_INCLUDED
/* { */


// Body of YsShellOctree is written in ysshelllattice.cpp

class YsShellOctree : public YsOctree <YSSIDE>
{
public:
	YsShellOctree();

	YSSIDE CheckInside(const YsVec3 &pos) const;

	int nInsideOutsideComputed;
	int nDerivedFromNeighbor;
	const YsShell *shl;
protected:
	virtual YSRESULT TraverseFunc(YsOctreeNode <YSSIDE> *node,const YsVec3 &min,const YsVec3 &max,int param1,int param2);
	// param1 -> Command (Use only negative number for toolkit functions)
	//   -1...Building Inside/Outside Node

	YSRESULT SpreadInsideOutside(YsOctreeNode <YSSIDE> *node,int vx,int vy,int vz,YSSIDE side,int depthLimit);
	YSRESULT SpreadInsideOutsideInverse(YsOctreeNode <YSSIDE> *node,int vx,int vy,int vz,YSSIDE side,int depthLimit);
};



/* } */
#endif
