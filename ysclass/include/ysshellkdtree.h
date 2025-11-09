#ifndef YSSHELLKDTREE_IS_INCLUDED
#define YSSHELLKDTREE_IS_INCLUDED
/* { */

class YsShell3dTree : protected Ys3dTree <YsShellVertexHandle>
{
public:
	using Ys3dTree <YsShellVertexHandle>::GetRootNode;
	using Ys3dTree <YsShellVertexHandle>::PreAllocateNode;

	YSRESULT Build(const class YsShell &shl);
	YSRESULT AddVertex(const class YsShell &shl,YsShellVertexHandle vtHd);
	YSRESULT MakeVertexList(YsArray <YsShellVertexHandle,128> &lst,const YsVec3 &min,const YsVec3 &max) const;
protected:
	virtual YSRESULT Enumerate(const YsVec3 &pos,const YsShellVertexHandle &attrib) const;
	mutable YsArray <YsShellVertexHandle,128> *vtxList;
};


/* } */
#endif
