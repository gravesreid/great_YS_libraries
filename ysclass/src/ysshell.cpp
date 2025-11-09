#define YS_COMPILING_YSSHELL_CPP


#include "ysclass.h"
#include "ysshell.h"

#include <time.h>





// Implementation //////////////////////////////////////////
static int YsShellItemMemoryLeakTracker=0;

YsShellItem::YsShellItem()
{
	YsShellItemMemoryLeakTracker++;

	special=NULL;
	Initialize();
}

void YsShellItem::Initialize(void)
{
	freeAttribute0=0;
	freeAttribute1=0;
	freeAttribute2=0;
	freeAttribute3=0;
	freeAttribute4=0;
	special=NULL;
}

YsShellItem::~YsShellItem()
{
	YsShellItemMemoryLeakTracker--;

	if(YsShellItemMemoryLeakTracker==0)
	{
		YsDiagOut("YsShellItem class reports all allocated items are deleted now.\n");
	}
	else if(YsShellItemMemoryLeakTracker<0)
	{
		YsDiagOut("YsShellItem class reports items are deleted too many times.\n");
	}
}

unsigned YsShellItem::GetSearchKey(void) const
{
	return searchKey;
}

void YsShellItem::CopyAttribute(const YsShellItem &from)
{
	freeAttribute0=from.freeAttribute0;
	freeAttribute1=from.freeAttribute1;
	freeAttribute2=from.freeAttribute2;
	freeAttribute3=from.freeAttribute3;
	freeAttribute4=from.freeAttribute4;
}


YSRESULT YsShellItem::SetSpecialAttribute(YsShellSpecialAttribute *attrib)
{
	special=attrib;
	return YSOK;
}

YsShellSpecialAttribute *YsShellItem::GetSpecialAttribute(void)
{
	return special;
}

const YsShellSpecialAttribute *YsShellItem::GetSpecialAttribute(void) const
{
	return special;
}

void YsShellItem::SpecialAttributeDetached(void)
{
	special=NULL;
}


// Implementation //////////////////////////////////////////
YsShellVertex::YsShellVertex()
{
	Initialize();
}

void YsShellVertex::Initialize(void)
{
	YsShellItem::Initialize();
	refCount=0;
	mapper=NULL;
	v2HdMap=NULL;
}

YSRESULT YsShellVertex::IncrementReferenceCount(void)
{
	refCount++;
	return YSOK;
}

YSRESULT YsShellVertex::DecrementReferenceCount(void)
{
	refCount--;
	if(refCount<0)
	{
		YsErrOut(
		    "YsShellVertex::DecrementReferenceCount(void)\n"
		    "  Reference count became negative\n");
		return YSERR;
	}
	return YSOK;
}

YSRESULT YsShellVertex::SetPosition(const YsVec3 &p)
{
	pos=p;
	return YSOK;
}

YSRESULT YsShellVertex::SetNormal(const YsVec3 &n)
{
#ifndef YSLOWMEM
	nom=n;
#else
	YsErrOut("YsShellVertex::SetNormal()\n");
	YsErrOut("  Running light memory mode.\n");
	YsErrOut("  To use this function, link ysclass.lib instead of ysclasslm.lib\n");
#endif
	return YSOK;
}

const YsVec3 &YsShellVertex::GetPosition(void) const
{
	return pos;
}

const YsVec3 &YsShellVertex::GetNormal(void) const
{
#ifndef YSLOWMEM
	return nom;
#else
	YsErrOut("YsShellVertex::GetNormal()\n");
	YsErrOut("  Running light memory mode.\n");
	YsErrOut("  To use this function, link ysclass.lib instead of ysclasslm.lib\n");
	return YsOrigin();
#endif
}

int YsShellVertex::GetReferenceCount(void) const
{
	return refCount;
}

YSRESULT YsShellVertex::SetReferenceCount(int n)  // This function must be eliminated soon
{
	refCount=n;
	return YSOK;
}

// Implementation //////////////////////////////////////////
YsShellPolygon::YsShellPolygon()
{
	Initialize();
}

void YsShellPolygon::Initialize(void)
{
	YsShellItem::Initialize();
	idx.Set(0,NULL);
	nom.Set(0.0,0.0,0.0);
#ifndef YSLOWMEM
	col.SetIntRGB(128,128,128);
#endif
}

YSRESULT YsShellPolygon::SetNormal(const YsVec3 &n)
{
	nom=n;
	return YSOK;
}

const YsVec3 &YsShellPolygon::GetNormal(void) const
{
	return nom;
}

YSRESULT YsShellPolygon::SetColor(const YsColor &c)
{
#ifndef YSLOWMEM
	col=c;
#endif
	return YSOK;
}

const YsColor &YsShellPolygon::GetColor(void) const
{
#ifndef YSLOWMEM
	return col;
#else
	return YsBlue();
#endif
}

YSRESULT YsShellPolygon::SetVertexList(const YsShell &shl,int nVtId,const int vtId[])
{
	int i;
	idx.Set(0,NULL);
	for(i=0; i<nVtId; i++)
	{
		idx.AppendItem(shl.GetVertexHandleFromId(vtId[i]));
	}
	return YSOK;
}

YSRESULT YsShellPolygon::SetVertexList(int nVtx,const YsShellVertexHandle vtx[])
{
	idx.Set(nVtx,vtx);
	return YSOK;
}

int YsShellPolygon::GetNumVertex(void) const
{
	return idx.GetNumItem();
}

const YsShellVertexHandle *YsShellPolygon::GetVertexArray(void) const
{
	return idx.GetArray();
}

int YsShellPolygon::GetVertexId(int i) const
{
	return idx[i]->GetContainer()->GetIdFromItem(idx[i]);
}

YsShellVertexHandle YsShellPolygon::GetVertexHandle(int i) const
{
	return idx[i];
}

void YsShellPolygon::Invert(void)
{
	int i,n;
	YsShellVertexHandle a,b;
	n=idx.GetNumItem();
	for(i=0; i<n/2; i++)
	{
		a=idx.GetItem(i);
		b=idx.GetItem(n-1-i);
		idx.SetItem(i,b);
		idx.SetItem(n-1-i,a);
	}
	nom=-nom;
}

////////////////////////////////////////////////////////////

YsShellReadSrfVariable::YsShellReadSrfVariable()
{
	mode=0;
	plHd=NULL;
	color.SetDoubleRGB(0.0,0.0,0.0);
	nom.Set(0.0,0.0,0.0);
}

// Implementation //////////////////////////////////////////
YsListAllocator <YsShellVertex> YsShell::vertexStore;
YsListAllocator <YsShellPolygon> YsShell::polygonStore;



YsShell::YsShell() :
vtx(vertexStore), plg(polygonStore),
frozenVtx(vertexStore), frozenPlg(polygonStore)
{
	matrixIsSet=YSFALSE;
	mat.Initialize();
	readSrfVar=NULL;

	searchTable=NULL;

	trustPolygonNormal=YSFALSE;   // Added 2000/11/10

	RewindSearchKey();

	bbxIsValid=YSFALSE;
	bbx1.Set(0.0,0.0,0.0);
	bbx2.Set(0.0,0.0,0.0);
}

YsShell::YsShell(const YsShell &from) :
vtx(vertexStore), plg(polygonStore),
frozenVtx(vertexStore), frozenPlg(polygonStore)
{
	*this=from;

	bbxIsValid=YSFALSE;
}

YsShell::~YsShell()
{
	YsShellVertexHandle dmyVtHd[1];

	YsListItem <YsShellPolygon> *plgSeek;
	plgSeek=NULL;
	while((plgSeek=plg.FindNext(plgSeek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingPolygon(plgSeek->dat);
		plgSeek->dat.SetVertexList(0,dmyVtHd);
	}
	plg.CleanUp();

	plgSeek=NULL;
	while((plgSeek=frozenPlg.FindNext(plgSeek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingPolygon(plgSeek->dat);
		plgSeek->dat.SetVertexList(0,dmyVtHd);
	}
	frozenPlg.CleanUp();

	YsListItem <YsShellVertex> *vtxSeek;
	vtxSeek=NULL;
	while((vtxSeek=vtx.FindNext(vtxSeek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingVertex(vtxSeek->dat);
	}
	vtx.CleanUp();

	vtxSeek=NULL;
	while((vtxSeek=frozenVtx.FindNext(vtxSeek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingVertex(vtxSeek->dat);
	}
	frozenVtx.CleanUp();
}

void YsShell::CleanUp(void)
{
	YsListItem <YsShellPolygon> *plgSeek;
	YsShellVertexHandle dmyVtHd[1];
	plgSeek=NULL;
	while((plgSeek=plg.FindNext(plgSeek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingPolygon(plgSeek->dat);
		plgSeek->dat.SetVertexList(0,dmyVtHd);
	}
	plg.CleanUp();

	YsListItem <YsShellVertex> *vtxSeek;
	vtxSeek=NULL;
	while((vtxSeek=vtx.FindNext(vtxSeek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingVertex(vtxSeek->dat);
	}
	vtx.CleanUp();

	if(searchTable!=NULL)
	{
		searchTable->Prepare();
	}
	RewindSearchKey();
	bbxIsValid=YSFALSE;
}

void YsShell::Encache(void) const
{
	plg.Encache();
	vtx.Encache();
}

void YsShell::Decache(void) const
{
	plg.Decache();
	vtx.Decache();
}


YSRESULT YsShell::AttachSearchTable(YsShellSearchTable *search) const
{
	if(searchTable==NULL)
	{
		if(search!=NULL)
		{
			searchTable=search;

			YsShellVertexHandle vtHd;
			YsShellPolygonHandle plHd;

			search->Prepare();
			vtHd=NULL;
			while((vtHd=FindNextVertex(vtHd))!=NULL)
			{
				search->AddVertex(*this,vtHd);
			}
			plHd=NULL;
			while((plHd=FindNextPolygon(plHd))!=NULL)
			{
				search->AddPolygon(*this,plHd);
			}

			return YSOK;
		}
		else
		{
			return YSERR;
		}
	}
	else
	{
		YsErrOut("YsShell::AttachSearchTable()\n");
		YsErrOut("  Search table is already attached.\n");
		return YSERR;
	}
}

YSRESULT YsShell::DetachSearchTable(void) const
{
	if(searchTable!=NULL)
	{
		searchTable=NULL;
		return YSOK;
	}
	else
	{
		YsErrOut("YsShell::DetachSearchTable()\n");
		YsErrOut("  Search table is not attached.\n");
		return YSERR;
	}
}

const YsShellSearchTable *YsShell::GetSearchTable(void) const
{
	return searchTable;
}



void YsShell::RewindVtxPtr(void) const
{
	vtx.RewindPointer();
}

YsShellVertexHandle YsShell::StepVtxPtr(void) const
{
	return vtx.StepPointer();
}

void YsShell::RewindPlgPtr(void) const
{
	plg.RewindPointer();
}

YsShellPolygonHandle YsShell::StepPlgPtr(void) const
{
	return plg.StepPointer();
}



YsShellVertexHandle YsShell::FindNextVertex(YsShellVertexHandle vtHd) const
{
	return vtx.FindNext(vtHd);
}

YsShellVertexHandle YsShell::FindPrevVertex(YsShellVertexHandle vtHd) const
{
	return vtx.FindPrev(vtHd);
}

YsShellPolygonHandle YsShell::FindNextPolygon(YsShellPolygonHandle plHd) const
{
	return plg.FindNext(plHd);
}

YsShellPolygonHandle YsShell::FindPrevPolygon(YsShellPolygonHandle plHd) const
{
	return plg.FindPrev(plHd);
}

int YsShell::GetVertexIdFromHandle(YsShellVertexHandle vtHd) const
{
	if(vtHd!=NULL)
	{
		return vtx.GetIdFromItem(vtHd);
	}
	else
	{
		return -1;
	}
}

int YsShell::GetPolygonIdFromHandle(YsShellPolygonHandle plHd) const
{
	if(plHd!=NULL)
	{
		return plg.GetIdFromItem(plHd);
	}
	else
	{
		return -1;
	}
}

YsShellVertexHandle YsShell::GetVertexHandleFromId(int vtId) const
{
	return vtx.GetItemFromId(vtId);
}

YsShellPolygonHandle YsShell::GetPolygonHandleFromId(int plId) const
{
	return plg.GetItemFromId(plId);
}

YsShellVertexHandle YsShell::AddVertexH(const YsVec3 &vec0)
{
	YsListItem <YsShellVertex> *neo;
	neo=CreateVertex();
	if(neo!=NULL)
	{
		YsVec3 vec;
		if(matrixIsSet==YSTRUE)
		{
			mat.MulInverse(vec,vec0,1.0);
		}
		else
		{
			vec=vec0;
		}

		neo->dat.SetPosition(vec);

		if(searchTable!=NULL)
		{
			searchTable->AddVertex(*this,neo);
		}

		if(bbxIsValid==YSTRUE)
		{
			YsBoundingBoxMaker3 bbx;
			bbx.Begin(bbx1);
			bbx.Add(bbx2);
			bbx.Add(vec);
			bbx.Get(bbx1,bbx2);
		}
		return neo;
	}
	else
	{
		YsErrOut("YsShell::AddVertex\nLowMemoryWarning\n");
		return NULL;
	}
}

YsShellPolygonHandle YsShell::AddPolygonH(int nv,const int vtId[])
{
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle,16> vtxList;
	int i;
	vtxList.Set(0,NULL);
	for(i=0; i<nv; i++)
	{
		vtxList.AppendItem(GetVertexHandleFromId(vtId[i]));
	}
	plHd=AddPolygonH(nv,vtxList.GetArray());
	return plHd;
}

YsShellPolygonHandle YsShell::AddPolygonH(int nv,const YsShellVertexHandle vtHd[])
{
	YsListItem <YsShellPolygon> *neo;
	neo=CreatePolygon();
	if(neo!=NULL)
	{
		int i;
		neo->dat.SetVertexList(nv,vtHd);

		for(i=0; i<nv; i++)
		{
			GetVertex(vtHd[i])->IncrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->AddPolygon(*this,neo);
		}
		return neo;
	}
	else
	{
		YsErrOut("YsShell::AddPolygon\nLowMemoryWarning\n");
		return NULL;
	}
}

YsShellPolygonHandle YsShell::AddPolygonVertexH(int nv,const YsVec3 v[])
{
	return AddPolygonVertex(nv,v,-1,NULL);
}

int YsShell::AddVertex(const YsVec3 &vec)
{
	YsShellVertexHandle vtHd;
	vtHd=AddVertexH(vec);
	return vtx.GetN()-1; // GetVertexIdFromHandle(vtHd);
}

// int YsShell::AddPolygon(int nv,const int v[])  // Deleted on 2005/01/19
// {
// 	YsShellPolygonHandle plHd;
// 	plHd=AddPolygonH(nv,v);
// 	return plg.GetN()-1; // GetPolygonIdFromHandle(plHd);
// }

int YsShell::AddPolygonVertex(int nv,const YsVec3 v[])
{
	// return GetPolygonIdFromHandle(AddPolygonVertex(nv,v,-1,NULL));

	AddPolygonVertex(nv,v,-1,NULL);
	return plg.GetN()-1;
}

YSRESULT YsShell::ModifyPolygon(int plId,const int nv,const int v[])
{
	int i;
	YsShellPolygonHandle plHd;
	YsShellPolygon *plg;
	YsArray <YsShellVertexHandle,16> vtxList;

	plHd=GetPolygonHandleFromId(plId);
	plg=GetPolygon(plHd);
	vtxList.Set(0,NULL);
	for(i=0; i<nv; i++)
	{
		vtxList.AppendItem(GetVertexHandleFromId(v[i]));
	}
	return ModifyPolygon(plHd,nv,vtxList.GetArray());
}

YSRESULT YsShell::ModifyPolygon(YsShellPolygonHandle plHd,int nv,const YsShellVertexHandle v[])
{
	YsShellPolygon *plg;
	YsShellVertex *vtx;

	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		int i,nVtxOld;
		nVtxOld=GetNumVertexOfPolygon(plHd);

		for(i=0; i<nVtxOld; i++)
		{
			vtx=GetVertex(plg->GetVertexHandle(i));
			vtx->DecrementReferenceCount();
		}

		for(i=0; i<nv; i++)
		{
			vtx=GetVertex(v[i]);
			vtx->IncrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->DeletePolygon(*this,plHd);
		}

		plg->SetVertexList(nv,v);

		if(searchTable!=NULL)
		{
			searchTable->AddPolygon(*this,plHd);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::ValidateVtId(int nVtId,const int vtId[]) const
{
	int i,nVtx;
	nVtx=GetNumVertex();
	for(i=0; i<nVtId; i++)
	{
		if(nVtx<=vtId[i])
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShell::ValidatePlId(int nPlId,const int plId[]) const
{
	int i,nPlg;
	nPlg=GetNumVertex();
	for(i=0; i<nPlId; i++)
	{
		if(nPlg<=plId[i])
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShell::CheckPotentialPolygon(int nVt,const YsShellVertexHandle vt[],const double &cosThr)
{
	if(nVt>=3)
	{
		int i;
		YsVec3 p[3],v[2];
		for(i=0; i<nVt; i++)
		{
			GetVertexPosition(p[0],vt[i]);
			GetVertexPosition(p[1],vt[(i+1)%nVt]);
			GetVertexPosition(p[2],vt[(i+2)%nVt]);
			v[0]=p[1]-p[0];
			v[1]=p[2]-p[1];
			if(v[0].Normalize()!=YSOK || v[1].Normalize()!=YSOK || v[0]*v[1]>=cosThr)
			{
				return YSERR;
			}
		}

		for(i=0; i<nVt; i++)  // Detect double refering
		{
			int j;
			YsShellVertexHandle ed1[2],ed2[2];
			ed1[0]=vt[i];
			ed1[1]=vt[(i+1)%nVt];
			for(j=i+1; j<nVt; j++)
			{
				ed2[0]=vt[j];
				ed2[1]=vt[(j+1)%nVt];
				if((ed1[0]==ed2[0] && ed1[1]==ed2[1]) ||
				   (ed1[0]==ed2[1] && ed1[1]==ed2[0]))
				{
					return YSERR;
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell::ModifyVertexPosition(int vtId,const YsVec3 &neoPos)
{
	return ModifyVertexPosition(GetVertexHandleFromId(vtId),neoPos);
}

YSRESULT YsShell::ModifyVertexPosition(YsShellVertexHandle vtHd,const YsVec3 &neoPos0)
{
	YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		YsVec3 neoPos;
		if(matrixIsSet==YSTRUE)
		{
			mat.MulInverse(neoPos,neoPos0,1.0);
		}
		else
		{
			neoPos=neoPos0;
		}
		vtx->SetPosition(neoPos);
		bbxIsValid=YSFALSE;
		return YSOK;
	}
	return YSERR;
}

YsShellPolygonHandle YsShell::AddPolygonVertex(int nv,const YsVec3 v[],int fa3,const int fa4[])
{
	int i;
	YsShellVertexHandle *vtHd;
	YsShellPolygonHandle plHd;
	YsOneMemoryBlock <YsShellVertexHandle,256> memBlock;
	YsShellVertex *vtx;

	vtHd=memBlock.GetMemoryBlock(nv);

	if(vtHd!=NULL)
	{
		for(i=0; i<nv; i++)
		{
			vtHd[i]=AddVertexH(v[i]);
			if(fa3>=0 && fa4!=NULL)
			{
				vtx=GetVertex(vtHd[i]);
				vtx->freeAttribute3=fa3;
				vtx->freeAttribute4=fa4[i];
			}
		}
		plHd=AddPolygonH(nv,vtHd);

		return plHd;
	}
	else
	{
		YsErrOut("YsShell::AddPolygonVertex\nLowMemoryWarning\n");
		return NULL;
	}
}

YSBOOL YsShell::FindRepairFlipDirectionOfOneReliablePolygon
     (YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellPolygonHandle plHd)
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::FindRepairFlipDirectionOfOneReliablePolygon()\n");
		YsErrOut("  This function needs search table.\n");
		return YSFALSE;
	}

	// Avoid overlapping polygons
	// Take advantage of search table!

	YsOneMemoryBlock <YsVec3,128> vtxBlock;
	YsVec3 *vtx,tri[3];
	int nVtxPlg;
	YsVec3 v1,v2,nom,cen;
	YsShellPolygon *plg;

	plg=GetPolygon(plHd);

	if(IsOverlappingPolygon(plHd)==YSTRUE)
	{
		plg->freeAttribute4=1;  // Can do nothing for overlapping polygon
		SetNormalOfPolygon(plHd,YsOrigin());
		return YSFALSE;
	}

	nVtxPlg=GetNumVertexOfPolygon(plHd);
	vtx=vtxBlock.GetMemoryBlock(nVtxPlg);
	GetVertexListOfPolygon(vtx,nVtxPlg,plHd);

	if(YsGetLargestTriangleFromPolygon3(tri,nVtxPlg,vtx)==YSOK)
	{
		v1=tri[1]-tri[0];
		v2=tri[2]-tri[1];
		if(v1.Normalize()==YSOK && v2.Normalize()==YSOK)
		{
			nom=v1^v2;
			cen=(tri[0]+tri[1]+tri[2])/3.0;
			if(nom.Normalize()==YSOK)
			{
				int numIntersect;
				YSSIDE side;
				side=CountRayIntersection(numIntersect,cen,nom,plHd);
				if(side==YSINSIDE)
				{
					nom=-nom;
					InvertPolygon(plHd);
					invertedPlHdList.AppendItem(plHd);
					SetNormalOfPolygon(plHd,nom);
					plg->freeAttribute4=1;
					return YSTRUE;
				}
				else if(side==YSOUTSIDE)
				{
					SetNormalOfPolygon(plHd,nom);
					plg->freeAttribute4=1;
					return YSTRUE;
				}
			}
		}
	}
	return YSFALSE;
}

YSRESULT YsShell::RecursivelyRepairFlipDirection
    (int &nDone,YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellPolygonHandle firstPlHd)
{
	YsArray <YsShellRepairFlipDirectionInfo,4096> *todo;

	todo=new YsArray <YsShellRepairFlipDirectionInfo,4096>;
	if(todo==NULL)
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::RecursivelyRepairFlipDirection()");
		return YSERR;
	}

	AddNeighborToList(*todo,firstPlHd);
	while(todo->GetNumItem()>0)
	{
		int n;
		YsShellRepairFlipDirectionInfo info;

		n=todo->GetNumItem();
		info=todo->GetItem(n-1);

		YsShellPolygon *plg;
		plg=GetPolygon(info.plHd);
		if(plg->freeAttribute4==0)
		{
			nDone++;
			RepairFlipDirectionOfOnePolygon(invertedPlHdList,info);
			plg->freeAttribute4=1;
			AddNeighborToList(*todo,info.plHd);
		}

		todo->Delete(n-1);
	}

	delete todo;
	return YSOK;
}

YSRESULT YsShell::RepairFlipDirectionOfOnePolygon
    (YsArray <YsShellPolygonHandle> &invertedPlHdList,YsShellRepairFlipDirectionInfo &info)
{
	int i,n;
	YsOneMemoryBlock <YsShellVertexHandle,128> plVtHdBlock;
	YsOneMemoryBlock <YsVec3,128> plVtxBlock;
	YsShellVertexHandle *plVtHd;
	YsVec3 *plVtx,tri[3],nom,v1,v2;

	n=GetNumVertexOfPolygon(info.plHd);
	plVtHd=plVtHdBlock.GetMemoryBlock(n);
	plVtx=plVtxBlock.GetMemoryBlock(n);

	GetVertexListOfPolygon(plVtHd,n,info.plHd);

	for(i=0; i<n; i++)
	{
		if(plVtHd[i]==info.vtHd1 && plVtHd[(i+1)%n]==info.vtHd2)
		{
			break;
		}
		else if(plVtHd[i]==info.vtHd2 && plVtHd[(i+1)%n]==info.vtHd1)
		{
			InvertPolygon(info.plHd);
			invertedPlHdList.AppendItem(info.plHd);
			break; // Is it needed? 2003/07/14
		}
	}

	// At this point, must be already flipped.
	GetVertexListOfPolygon(plVtx,n,info.plHd);
	YsGetLargestTriangleFromPolygon3(tri,n,plVtx);

	v1=tri[1]-tri[0];
	v2=tri[2]-tri[1];

	if(v1.Normalize()==YSOK && v2.Normalize()==YSOK)
	{
		nom=v1^v2;
		if(nom.Normalize()==YSOK)
		{
			SetNormalOfPolygon(info.plHd,nom);
		}
	}

	return YSOK;
}

YSRESULT YsShell::AddNeighborToList
	    (YsArray <YsShellRepairFlipDirectionInfo,4096> &todo,YsShellPolygonHandle plHd)
{
	int i,n;
	YsOneMemoryBlock <YsShellVertexHandle,128> plVtHdBlock;
	YsShellVertexHandle *plVtHd;

	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	YsShellVertexHandle vtHd1,vtHd2;
	YsShellRepairFlipDirectionInfo info;

	n=GetNumVertexOfPolygon(plHd);
	plVtHd=plVtHdBlock.GetMemoryBlock(n);

	GetVertexListOfPolygon(plVtHd,n,plHd);

	for(i=0; i<n; i++)
	{
		vtHd1=plVtHd[i];
		vtHd2=plVtHd[(i+1)%n];

		if(searchTable->FindPolygonListByEdge(nFoundPlHd,foundPlHd,*this,vtHd1,vtHd2)==YSOK && nFoundPlHd==2)
		{
			YsShellPolygonHandle neighborPlHd;
			if(foundPlHd[0]==plHd)
			{
				neighborPlHd=foundPlHd[1];
			}
			else
			{
				neighborPlHd=foundPlHd[0];
			}

			YsShellPolygon *plg;
			plg=GetPolygon(neighborPlHd);
			if(plg->freeAttribute4==0)
			{
				info.plHd=neighborPlHd;
				info.vtHd1=vtHd2;  // Note : Edge direction must be inverted for the neighboring polygon.
				info.vtHd2=vtHd1;  //
				todo.Append(info);
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfPolygon(YsArray <int> &attrib4) const
{
	int i,n;
	YsShellPolygonHandle plHd;

	n=GetNumPolygon();
	if(attrib4.Set(n,NULL)==YSOK)
	{
		plHd=NULL;
		i=0;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			YsShellPolygon *plg;
			plg=(YsShellPolygon *)GetPolygon(plHd);
			attrib4.SetItem(i,plg->freeAttribute4);
			i++;
		}

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShell::RestoreFreeAttribute4OfPolygon(YsArray <int> &attrib4) const
{
	int i;
	YsShellPolygonHandle plHd;

	plHd=NULL;
	i=0;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		YsShellPolygon *plg;
		plg=(YsShellPolygon *)GetPolygon(plHd);
		plg->freeAttribute4=attrib4.GetItem(i);
		i++;
	}

	return YSOK;
}

YSRESULT YsShell::ClearFreeAttribute4OfPolygon(int attrib4)
{
	YsShellPolygonHandle plHd;

	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		YsShellPolygon *plg;
		plg=GetPolygon(plHd);
		plg->freeAttribute4=attrib4;
	}

	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfVertex(YsArray <int> &attrib4) const
{
	int i,n;
	YsShellVertexHandle vtHd;

	n=GetNumVertex();
	if(attrib4.Set(n,NULL)==YSOK)
	{
		vtHd=NULL;
		i=0;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsShellVertex *plg;
			plg=(YsShellVertex *)GetVertex(vtHd);
			attrib4.SetItem(i,plg->freeAttribute4);
			i++;
		}

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShell::RestoreFreeAttribute4OfVertex(YsArray <int> &attrib4) const
{
	int i;
	YsShellVertexHandle vtHd;

	vtHd=NULL;
	i=0;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsShellVertex *plg;
		plg=(YsShellVertex *)GetVertex(vtHd);
		plg->freeAttribute4=attrib4.GetItem(i);
		i++;
	}

	return YSOK;
}

YSRESULT YsShell::ClearFreeAttribute4OfVertex(int attrib4)
{
	YsShellVertexHandle vtHd;

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsShellVertex *vtx;
		vtx=GetVertex(vtHd);
		vtx->freeAttribute4=attrib4;
	}

	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfPolygonAndNeighbor(
    YsArray <YsPair <YsShellPolygonHandle,int>,64 > &savePlAtt4,
    int nPl,const YsShellPolygonHandle plHdList[],int freeAtt4Value) const
{
	int i,j,nPlVt;
	const YsShellSearchTable *search;

	search=GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsShell::SaveFreeAttribute4OfPolygonAndNeighbor()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}


	savePlAtt4.Set(0,NULL);

	for(i=0; i<nPl; i++)
	{
		YsPair <YsShellPolygonHandle,int> plAtt4Pair;
		plAtt4Pair.a=plHdList[i];
		plAtt4Pair.b=GetPolygon(plHdList[i])->freeAttribute4;
		GetPolygon(plHdList[i])->freeAttribute4=freeAtt4Value;

		savePlAtt4.Append(plAtt4Pair);

		nPlVt=GetNumVertexOfPolygon(plHdList[i]);
		for(j=0; j<nPlVt; j++)
		{
			YsShellPolygonHandle neiPlHd;
			neiPlHd=search->GetNeighborPolygon(*this,plHdList[i],j);
			if(neiPlHd!=NULL)
			{
				plAtt4Pair.a=neiPlHd;
				plAtt4Pair.b=GetPolygon(neiPlHd)->freeAttribute4;
				GetPolygon(neiPlHd)->freeAttribute4=freeAtt4Value;

				savePlAtt4.Append(plAtt4Pair);
			}
		}
	}

	return YSOK;
}

YSRESULT YsShell::RestoreFreeAttribute4OfPolygon(
    int nSavePlAtt4,const YsPair <YsShellPolygonHandle,int> savePlAtt4[]) const
{
	int i;
	for(i=nSavePlAtt4-1; i>=0; i--)
	{
		GetPolygon(savePlAtt4[i].a)->freeAttribute4=savePlAtt4[i].b;
	}
	return YSOK;
}

YSRESULT YsShell::SaveFreeAttribute4OfVertexAndNeighbor(
    YsArray <YsPair <YsShellVertexHandle,int>,64> &saveVtAtt4,
    int nVt,const YsShellVertexHandle vtHdList[],int freeAtt4Value) const
{
	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsShell::SaveFreeAttribute4OfVertexAndNeighbor()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}

	int i,j;
	YsArray <YsShellVertexHandle,16> connVtHd;

	saveVtAtt4.Set(0,NULL);

	for(i=0; i<nVt; i++)
	{
		YsPair <YsShellVertexHandle,int> vtAtt4Pair;
		vtAtt4Pair.a=vtHdList[i];
		vtAtt4Pair.b=GetVertex(vtHdList[i])->freeAttribute4;
		GetVertex(vtHdList[i])->freeAttribute4=freeAtt4Value;

		saveVtAtt4.Append(vtAtt4Pair);

		if(search->GetConnectedVertexList(connVtHd,*this,vtHdList[i])==YSOK)
		{
			forYsArray(j,connVtHd)
			{
				vtAtt4Pair.a=connVtHd[j];
				vtAtt4Pair.b=GetVertex(connVtHd[j])->freeAttribute4;
				GetVertex(connVtHd[j])->freeAttribute4=freeAtt4Value;

				saveVtAtt4.Append(vtAtt4Pair);
			}
		}
	}

	return YSOK;
}

YSRESULT YsShell::RestoreFreeAttribute4OfVertex(int nSaveVtAtt4,const YsPair <YsShellVertexHandle,int> saveVtAtt4[]) const
{
	int i;
	for(i=nSaveVtAtt4-1; i>=0; i--)
	{
		GetVertex(saveVtAtt4[i].a)->freeAttribute4=saveVtAtt4[i].b;
	}
	return YSOK;
}


YSRESULT YsShell::GetNormalOfPolygon(YsVec3 &nom,int plId) const
{
	return GetNormalOfPolygon(nom,GetPolygonHandleFromId(plId));
}

YSRESULT YsShell::GetColorOfPolygon(YsColor &col,int plId) const
{
	return GetColorOfPolygon(col,GetPolygonHandleFromId(plId));
}

const YsVec3 &YsShell::GetCenterOfPolygon(YsVec3 &cen,YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	cen=YsOrigin();

	nPlVt=GetNumVertexOfPolygon(plHd);
	plVtHd=GetVertexListOfPolygon(plHd);
	if(nPlVt>0 && plVtHd!=NULL)
	{
		int i;
		YsVec3 vtPos;
		for(i=0; i<nPlVt; i++)
		{
			GetVertexPosition(vtPos,plVtHd[i]);
			cen+=vtPos;
		}
		cen/=double(nPlVt);
	}
	return cen;
}

const YsVec3 &YsShell::GetCenterOfEdge(YsVec3 &mid,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 pos[2];
	if(GetVertexPosition(pos[0],edVtHd1)==YSOK && GetVertexPosition(pos[1],edVtHd2)==YSOK)
	{
		mid=(pos[0]+pos[1])/2.0;
		return mid;
	}
	mid=YsOrigin();
	return mid;
}

const YsVec3 &YsShell::GetCenterOfVertexGroup(YsVec3 &cen,int nVt,const YsShellVertexHandle vtHdList[]) const
{
	int i;
	YsVec3 p;
	cen=YsOrigin();
	if(nVt>0)
	{
		for(i=0; i<nVt; i++)
		{
			GetVertexPosition(p,vtHdList[i]);
			cen+=p;
		}
		cen/=(double)nVt;
	}
	return cen;
}

const YsVec3 &YsShell::GetCenterOfTriangle(YsVec3 &cen,YsShellVertexHandle trVtHd1,YsShellVertexHandle trVtHd2,YsShellVertexHandle trVtHd3) const
{
	YsVec3 pos[3];
	if(GetVertexPosition(pos[0],trVtHd1)==YSOK && 
	   GetVertexPosition(pos[1],trVtHd2)==YSOK &&
	   GetVertexPosition(pos[2],trVtHd3)==YSOK)
	{
		cen=(pos[0]+pos[1]+pos[2])/3.0;
		return cen;
	}
	cen=YsOrigin();
	return cen;
}

const YsVec3 &YsShell::GetArbitraryInsidePointOfPolygon(YsVec3 &isp,YsShellPolygonHandle plHd) const
{
	if(GetNumVertexOfPolygon(plHd)==3)
	{
		return GetCenterOfPolygon(isp,plHd);
	}
	else
	{
		YsArray <YsVec3> plVtPos;
		GetVertexListOfPolygon(plVtPos,plHd);
		YsGetArbitraryInsidePointOfPolygon3(isp,plVtPos.GetN(),plVtPos);
	}
	return isp;
}

const double YsShell::GetEdgeSquareLength(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd1)==YSOK &&
	   GetVertexPosition(p2,edVtHd2)==YSOK)
	{
		return (p1-p2).GetSquareLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeSquareLength(YsShellVertexHandle edVtHd[2]) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd[0])==YSOK &&
	   GetVertexPosition(p2,edVtHd[1])==YSOK)
	{
		return (p1-p2).GetSquareLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeLength(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd1)==YSOK &&
	   GetVertexPosition(p2,edVtHd2)==YSOK)
	{
		return (p1-p2).GetLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeLength(YsShellVertexHandle edVtHd[2]) const
{
	YsVec3 p1,p2;
	if(GetVertexPosition(p1,edVtHd[0])==YSOK &&
	   GetVertexPosition(p2,edVtHd[1])==YSOK)
	{
		return (p1-p2).GetLength();
	}
	return 0.0;
}

const double YsShell::GetEdgeLength(int nVt,const YsShellVertexHandle edVtHd[]) const
{
	int i;
	double l;
	YsVec3 buf[2];

	l=0.0;
	if(nVt>=2)
	{
		GetVertexPosition(buf[0],edVtHd[0]);
		for(i=1; i<nVt; i++)
		{
			GetVertexPosition(buf[i&1],edVtHd[i]);
			l+=(buf[0]-buf[1]).GetLength();
		}
	}
	return l;
}

const double YsShell::GetLoopLength(int nVt,const YsShellVertexHandle edVtHd[]) const
{
	int i;
	double l;
	YsVec3 buf[2];

	l=0.0;
	if(nVt>=2)
	{
		GetVertexPosition(buf[1],edVtHd[nVt-1]);
		for(i=0; i<nVt; i++)
		{
			GetVertexPosition(buf[i&1],edVtHd[i]);
			l+=(buf[0]-buf[1]).GetLength();
		}
	}
	return l;
}

YSRESULT YsShell::GetArbitraryInsidePointOfPolygonOnPlane(
    YsVec3 &isp,YsShellPolygonHandle plHd,const YsPlane &pln) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	int i,nItsc;
	YsVec3 itsc[2],tst;

	nItsc=0;
	if(GetVertexListOfPolygon(nPlVt,plVtHd,plHd)==YSOK)
	{
		YsShellVertexHandle edVtHd[2];
		YsVec3 edVtPos[2];
		edVtHd[1]=plVtHd[nPlVt-1];
		for(i=0; i<nPlVt; i++)
		{
			edVtHd[0]=edVtHd[1];
			edVtHd[1]=plVtHd[i];
			GetVertexPosition(edVtPos[0],edVtHd[0]);
			GetVertexPosition(edVtPos[1],edVtHd[1]);

			if(pln.GetPenetration(tst,edVtPos[0],edVtPos[1])==YSOK)
			{
				if(nItsc<2)
				{
					itsc[nItsc++]=tst;
				}
				else
				{
					return YSERR;
				}
			}
		}
		if(nItsc==2)
		{
			isp=(itsc[0]+itsc[1])/2.0;
			return YSOK;
		}
	}
	return YSERR;

}

YSRESULT YsShell::GetNormalOfPolygon(YsVec3 &nom,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		nom=plg->GetNormal();
		if(matrixIsSet==YSTRUE)   // Modified 2000/11/10
		{
			mat.Mul(nom,nom,0.0);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::ComputeNormalOfPolygonFromGeometry(YsVec3 &nom,YsShellPolygonHandle plHd) const
{
	YsArray <YsVec3> plVtPos;
	if(GetVertexListOfPolygon(plVtPos,plHd)==YSOK)
	{
		if(plVtPos.GetN()==3)
		{
			nom=(plVtPos[1]-plVtPos[0])^(plVtPos[2]-plVtPos[0]);
			return nom.Normalize();
		}
		else if(YsFindLeastSquareFittingPlaneNormal(nom,plVtPos.GetN(),plVtPos)==YSOK)
		{
			return YSOK;
		}
		else if(YsGetAverageNormalVector(nom,plVtPos.GetN(),plVtPos)==YSOK)
		{
			return YSOK;
		}
	}

	nom=YsOrigin();
	return YSERR;
}

YSRESULT YsShell::MakePolygonKeyNormalPairFromGeometry(YsPair <unsigned int,YsVec3> &plKeyNomPair,YsShellPolygonHandle plHd) const
{
	plKeyNomPair.a=GetSearchKey(plHd);
	return ComputeNormalOfPolygonFromGeometry(plKeyNomPair.b,plHd);
}

YSRESULT YsShell::MakePolygonKeyNormalPairListFromGeometry(YsArray <YsPair <unsigned int,YsVec3> > &plKeyNomPair,int nPl,const YsShellPolygonHandle plHdList[]) const
{
	int i;
	plKeyNomPair.Set(nPl,NULL);
	for(i=0; i<nPl; i++)
	{
		MakePolygonKeyNormalPairFromGeometry(plKeyNomPair[i],plHdList[i]);
	}
	return YSOK;
}

double YsShell::ComputeDihedralAngle(YsShellPolygonHandle plHd1,YsShellPolygonHandle plHd2) const
{
	YsVec3 nom[2];
	if(GetNormalOfPolygon(nom[0],plHd1)==YSOK &&
	   GetNormalOfPolygon(nom[1],plHd2)==YSOK)
	{
		return acos(nom[0]*nom[1]);
	}
	return 0.0;
}

double YsShell::ComputeEdgeLength(YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2) const
{
	YsVec3 p[2];
	if(GetVertexPosition(p[0],vtHd1)==YSOK &&
	   GetVertexPosition(p[1],vtHd2)==YSOK)
	{
		return (p[0]-p[1]).GetLength();
	}
	return 0.0;
}

double YsShell::ComputeEdgeLength(int nVt,const YsShellVertexHandle vtHd[]) const
{
	int i;
	YsVec3 p[2];
	double d;

	d=0.0;

	GetVertexPosition(p[1],vtHd[0]);
	for(i=1; i<nVt; i++)
	{
		p[0]=p[1];
		GetVertexPosition(p[1],vtHd[i]);
		d+=(p[0]-p[1]).GetLength();
	}

	return d;
}

double YsShell::ComputeAngleCos(const YsShellVertexHandle vtHd[3]) const
{
	YsVec3 p[3],v[2];
	GetVertexPosition(p[0],vtHd[0]);
	GetVertexPosition(p[1],vtHd[1]);
	GetVertexPosition(p[2],vtHd[2]);

	v[0]=p[1]-p[0];
	v[1]=p[2]-p[1];
	if(v[0].Normalize()==YSOK && v[1].Normalize()==YSOK)
	{
		return v[0]*v[1];
	}
	return 1.0;
}

double YsShell::ComputeMinimumAngleCos(YsShellPolygonHandle plHd) const
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	if(GetVertexListOfPolygon(nPlVt,plVtHd,plHd)==YSOK)
	{
		return ComputeMinimumAngleCos(nPlVt,plVtHd);
	}
	else
	{
		return -1.0;
	}
}

double YsShell::ComputeMinimumAngleCos(int nPlVt,const YsShellVertexHandle plVtHd[]) const
{
	if(nPlVt>=3)
	{
		int i;
		double maxCos;
		YsVec3 p1,p2,v1,v2;

		maxCos=-1.0;
		GetVertexPosition(p1,plVtHd[nPlVt-2]);
		GetVertexPosition(p2,plVtHd[nPlVt-1]);
		v2=p2-p1;
		v2.Normalize();
		for(i=0; i<nPlVt; i++)
		{
			p1=p2;
			GetVertexPosition(p2,plVtHd[i]);

			v1=v2;
			v2=p2-p1;
			if(v1.IsNormalized()==YSTRUE && v2.Normalize()==YSOK)
			{
				double c;
				c=-v1*v2;
				if(maxCos<c)
				{
					maxCos=c;
				}
			}
		}
		return maxCos;
	}
	else
	{
		return -1.0;
	}
}

double YsShell::ComputeVolume(void) const
{
	double vol;
	YsShellPolygonHandle plHd;

	vol=0.0;
	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		int i;
		int nPlVtHd;
		const YsShellVertexHandle *plVtHd;
		YsVec3 tri[3];
		double signedVol;

		nPlVtHd=GetNumVertexOfPolygon(plHd);
		plVtHd=GetVertexListOfPolygon(plHd);

		GetVertexPosition(tri[0],plVtHd[0]);
		signedVol=0.0;
		for(i=1; i<nPlVtHd-1; i++)
		{
			GetVertexPosition(tri[1],plVtHd[i]);
			GetVertexPosition(tri[2],plVtHd[i+1]);
			signedVol+=((tri[1]-tri[0])^(tri[2]-tri[0]))*(tri[0])/6.0;
		}
		vol+=signedVol;
	}

	return vol;
}

YSRESULT YsShell::GetColorOfPolygon(YsColor &col,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		col=plg->GetColor();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SetNormalOfPolygon(int plId,const YsVec3 &nom)
{
	return SetNormalOfPolygon(GetPolygonHandleFromId(plId),nom);
}

YSRESULT YsShell::SetColorOfPolygon(int plId,const YsColor &col)
{
	return SetColorOfPolygon(GetPolygonHandleFromId(plId),col);
}

YSRESULT YsShell::SetNormalOfPolygon(YsShellPolygonHandle plHd,const YsVec3 &nom)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		if(matrixIsSet==YSTRUE)  // Modified 2000/11/10
		{
			YsVec3 nomTfm;
			mat.MulInverse(nomTfm,nom,0.0);
			plg->SetNormal(nomTfm);
		}
		else
		{
			plg->SetNormal(nom);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SetColorOfPolygon(YsShellPolygonHandle plHd,const YsColor &col)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		plg->SetColor(col);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::DeleteVertex(int vtId)
{
	return DeleteVertex(GetVertexHandleFromId(vtId));
}

YSRESULT YsShell::DeleteVertex(YsShellVertexHandle vtHd)
{
	YsShellVertex *v;
	v=GetVertex(vtHd);
	if(v!=NULL)
	{
		if(v->GetReferenceCount()>0)
		{
			YsErrOut("YsShell::DeleteVertex\n  Tried to delete a Vertex in use\n");
			return YSERR;
		}

		if(searchTable!=NULL)
		{
			searchTable->DeleteVertex(*this,vtHd);
		}

		YsListItem <YsShellVertex> *ptr;
		ptr=(YsListItem <YsShellVertex> *)vtHd;
		DetachSpecialAttributeBeforeDeletingVertex(ptr->dat);
		vtx.Delete(ptr);

		bbxIsValid=YSFALSE;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::DeletePolygon(int plId)
{
	return DeletePolygon(GetPolygonHandleFromId(plId));
}

YSRESULT YsShell::DeletePolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *p;
	YsShellVertex *vtx;
	p=GetPolygon(plHd);

	if(p!=NULL)
	{
		int i,nVtx;

		nVtx=GetNumVertexOfPolygon(plHd);
		for(i=0; i<nVtx; i++)
		{
			vtx=GetVertex(p->GetVertexHandle(i));
			vtx->DecrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->DeletePolygon(*this,plHd);
		}

		YsListItem <YsShellPolygon> *ptr;
		ptr=(YsListItem <YsShellPolygon> *)plHd;
		DetachSpecialAttributeBeforeDeletingPolygon(ptr->dat);
		plg.Delete(ptr);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::DeleteVertexAtTheSamePosition(int nLatX,int nLatY,int nLatZ)
{
	int nDeleted;
	return DeleteVertexAtTheSamePosition(nDeleted,nLatX,nLatY,nLatZ);
}

YSRESULT YsShell::DeleteVertexAtTheSamePosition(int &nDeleted,int nLatX,int nLatY,int nLatZ)
{
	YsShellVertexHandle vtHdA,vtHdB;
	YsShellVertex *vtxA,*vtxB;
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle> vtxList;
	YsShellLattice ltc(16384);

	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		vtxA->mapper=vtHdA;
	}

	ltc.SetDomain(*this,nLatX,nLatY,nLatZ);

	int i,j,k;
	const YsShellLatticeElem *elem;
	YsVec3 posA,posB;
	YsLoopCounter ctr;

	ctr.BeginCounter(nLatX*nLatY*nLatZ);
	for(k=0; k<nLatZ; k++)
	{
		for(j=0; j<nLatY; j++)
		{
			for(i=0; i<nLatX; i++)
			{
				ctr.Increment();

				int n,m;
				elem=ltc.GetBlock(i,j,k);
				for(n=0; n<elem->vtxList.GetNumItem(); n++)
				{
					vtHdA=elem->vtxList.GetItem(n);
					GetVertexPosition(posA,vtHdA);
					vtxA=GetVertex(vtHdA);
					for(m=n+1; m<elem->vtxList.GetNumItem(); m++)
					{
						vtHdB=elem->vtxList.GetItem(m);
						vtxB=GetVertex(vtHdB);
						GetVertexPosition(posB,vtHdB);

						if(posA==posB)
						{
							if(vtxA->mapper<vtxB->mapper)
							{
								vtxB->mapper=vtxA->mapper;
							}
							else
							{
								vtxA->mapper=vtxB->mapper;
							}
						}
					}
				}
			}
		}
	}
	ctr.EndCounter();


//	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
//	{
//		vtxA=GetVertex(vtHdA);
//		if(vtxA->mapper==vtHdA)  // means, if not mapped yet
//		{
//			for(vtHdB=FindNextVertex(vtHdA); vtHdB!=NULL; vtHdB=FindNextVertex(vtHdB))
//			{
//				vtxB=GetVertex(vtHdB);
//				if(vtxA->GetPosition()==vtxB->GetPosition())
//				{
//					vtxB->mapper=vtHdA;
//				}
//			}
//		}
//	}

	int nPlg;
	nPlg=GetNumPolygon();
	ctr.BeginCounter(nPlg);
	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		int i;
		YSBOOL needModify;
		YsShellPolygon *plg;
		YsShellVertex *plVt;
		YsShellVertexHandle plVtHd;

		ctr.Increment();

		needModify=YSFALSE;
		plg=GetPolygon(plHd);
		for(i=0; i<plg->GetNumVertex(); i++)
		{
			plVtHd=plg->GetVertexHandle(i);
			plVt=GetVertex(plVtHd);
			if(plVt->mapper!=plVtHd)
			{
				needModify=YSTRUE;
				break;
			}
		}

		if(needModify==YSTRUE)
		{
			vtxList.Set(0,NULL);
			for(i=0; i<plg->GetNumVertex(); i++)
			{
				plVtHd=plg->GetVertexHandle(i);
				plVt=GetVertex(plVtHd);

				while(plVtHd!=plVt->mapper)
				{
					plVtHd=plVt->mapper;
					plVt=GetVertex(plVtHd);
				}

				vtxList.AppendItem(plVt->mapper);
			}

			ModifyPolygon(plHd,vtxList.GetNumItem(),vtxList.GetArray());
		}
	}
	ctr.EndCounter();


	YsArray <YsShellVertexHandle> toDel;
	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		if(vtxA->mapper!=vtHdA)
		{
			toDel.AppendItem(vtHdA);
		}
	}

	nDeleted=0;
	for(i=0; i<toDel.GetNumItem(); i++)
	{
		DeleteVertex(toDel.GetItem(i));
		nDeleted++;
	}

	return YSOK;
}

YSRESULT YsShell::MakeVertexMappingToDeleteVertexAtTheSamePositionByKdTree(const double &tol)
{
	YsShellVertexHandle vtHdA;
	YsShellVertex *vtxA,*vtxB;

	YsShell3dTree kdTree;
	YsArray <YsShellVertexHandle,128> vtxList;

	int i,nVtx;
	YsVec3 posA,posB,bbx[2];
	YsLoopCounter ctr;

	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		vtxA->mapper=vtHdA;
	}

	kdTree.PreAllocateNode(GetNumVertex());  // 2004/06/18
	kdTree.Build(*this);

	nVtx=GetNumVertex();
	ctr.BeginCounter(nVtx);
	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		ctr.Increment();

		vtxA=GetVertex(vtHdA);
		if(vtxA->mapper==vtHdA)
		{
			GetVertexPosition(posA,vtHdA);

			bbx[0]=posA-YsXYZ()*tol;
			bbx[1]=posA+YsXYZ()*tol;

			if(kdTree.MakeVertexList(vtxList,bbx[0],bbx[1])==YSOK)
			{
				if(vtxList.GetN()==0)
				{
					YsErrOut("  KdTree seems to be broken.\n");
				}
				else if(vtxList.GetN()>=2)
				{
					YsShellVertexHandle topMostVtHd;
					topMostVtHd=vtxList.GetTopItem();
					for(i=0; i<vtxList.GetN(); i++)
					{
						if(vtxList[i]<topMostVtHd)
						{
							topMostVtHd=vtxList[i];
						}
					}

					for(i=0; i<vtxList.GetN(); i++)
					{
						vtxB=GetVertex(vtxList[i]);
						vtxB->mapper=topMostVtHd;
					}
				}
			}
			else
			{
				YsErrOut("  KdTree Search has been failed.\n");
			}
		}
	}
	ctr.EndCounter();

	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		YsShellVertexHandle tracker;
		YsShellVertex *vtx;
		vtx=GetVertex(vtHdA);

		tracker=vtx->mapper;
		while(tracker!=GetVertex(tracker)->mapper)
		{
			tracker=GetVertex(tracker)->mapper;
			if(tracker==vtHdA)
			{
				YsErrOut("Internal Error!\nMapping Loop detected!\n");
				break;
			}
		}
		vtx->mapper=tracker;
	}

	return YSOK;
}

YSRESULT YsShell::DeleteVertexAtTheSamePositionByKdTree(int &nDeleted,const double &tol)
{
	YsShellVertexHandle vtHdA;
	YsShellVertex *vtxA;
	YsShellPolygonHandle plHd;
	YsArray <YsShellVertexHandle,128> vtxList;
	int i;
	YsLoopCounter ctr;

	MakeVertexMappingToDeleteVertexAtTheSamePositionByKdTree(tol);

	int nPlg;
	nPlg=GetNumPolygon();
	ctr.BeginCounter(nPlg);
	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		int i;
		YSBOOL needModify;
		YsShellPolygon *plg;
		YsShellVertex *plVt;
		YsShellVertexHandle plVtHd;

		ctr.Increment();

		needModify=YSFALSE;
		plg=GetPolygon(plHd);
		for(i=0; i<plg->GetNumVertex(); i++)
		{
			plVtHd=plg->GetVertexHandle(i);
			plVt=GetVertex(plVtHd);
			if(plVt->mapper!=plVtHd)
			{
				needModify=YSTRUE;
				break;
			}
		}

		if(needModify==YSTRUE)
		{
			vtxList.Set(0,NULL);
			for(i=0; i<plg->GetNumVertex(); i++)
			{
				plVtHd=plg->GetVertexHandle(i);
				plVt=GetVertex(plVtHd);

				while(plVtHd!=plVt->mapper)
				{
					plVtHd=plVt->mapper;
					plVt=GetVertex(plVtHd);
				}

				vtxList.AppendItem(plVt->mapper);
			}

			ModifyPolygon(plHd,vtxList.GetNumItem(),vtxList.GetArray());
		}
	}
	ctr.EndCounter();


	YsArray <YsShellVertexHandle> toDel;
	for(vtHdA=FindNextVertex(NULL); vtHdA!=NULL; vtHdA=FindNextVertex(vtHdA))
	{
		vtxA=GetVertex(vtHdA);
		if(vtxA->mapper!=vtHdA)
		{
			toDel.AppendItem(vtHdA);
		}
	}

	nDeleted=0;
	for(i=0; i<toDel.GetNumItem(); i++)
	{
		DeleteVertex(toDel.GetItem(i));
		nDeleted++;
	}

	return YSOK;
}

YSRESULT YsShell::RepairPolygonOrientationBasedOnNormal(void)
{
	YsArray <YsShellPolygonHandle> invertedPlHd;
	return RepairPolygonOrientationBasedOnNormal(invertedPlHd);
}

YSRESULT YsShell::RepairPolygonOrientationBasedOnNormal(YsArray <YsShellPolygonHandle> &invertedPlHdList)
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::RepairPolygonOrientationBasedOnNormal()\n");
		YsErrOut("  Search Table is required for this operation.\n");
		YsErrOut("  Use AttachSearchTable(YsShellSearchTable *search);\n");
		YsErrOut("  to attach a search table.\n");
		return YSERR;
	}

	YsPrintf("YsShell::RepairPolygonOrientationBasedOnNormal\n");

	invertedPlHdList.Set(0,NULL);

	YsShellPolygonHandle plHd;
	YsArray <int> freeAttrib4Save;
	if(SaveFreeAttribute4OfPolygon(freeAttrib4Save)==YSOK)
	{
		int n;
		YsShellPolygon *plg;
		YsLoopCounter ctr;
		YsArray <YsVec3> plVtPos;
		int nDone;

		ClearFreeAttribute4OfPolygon(0);

		n=GetNumPolygon();

		YsPrintf("  Repairing Flip Direction (Pass 1)\n");
		plHd=NULL;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				YsVec3 nom;
				GetNormalOfPolygon(nom,plHd);
				if(nom.GetSquareLength()>YsSqr(YsTolerance))
				{
					YsVec3 tri[3],nomFromFlip;
					GetVertexListOfPolygon(plVtPos,plHd);
					YsGetLargestTriangleFromPolygon3(tri,plVtPos.GetN(),plVtPos);
					nomFromFlip=(tri[1]-tri[0])^(tri[2]-tri[0]);
					if(nom*nomFromFlip<0.0)
					{
						invertedPlHdList.Append(plHd);
						InvertPolygon(plHd);
						plg->freeAttribute4=1;
					}
					RecursivelyRepairFlipDirection(nDone,invertedPlHdList,plHd);
				}
			}
		}
		ctr.EndCounter();

		YsPrintf("  Repairing Flip Direction (Pass 2)\n");
		plHd=NULL;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				YsVec3 tri[3],nomFromFlip;
				GetVertexListOfPolygon(plVtPos,plHd);
				YsGetLargestTriangleFromPolygon3(tri,plVtPos.GetN(),plVtPos);
				nomFromFlip=(tri[1]-tri[0])^(tri[2]-tri[0]);
				nomFromFlip.Normalize();
				SetNormalOfPolygon(plHd,nomFromFlip);
				plg->freeAttribute4=1;
				RecursivelyRepairFlipDirection(nDone,invertedPlHdList,plHd);
			}
		}
		ctr.EndCounter();

		YsPrintf("  Verifying coverage\n");
		YSRESULT res;
		res=YSOK;
		plHd=NULL;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			YsShellPolygon *plg;
			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				res=YSERR;
			}
		}
		ctr.EndCounter();

		if(res!=YSOK)
		{
			YsErrOut("YsShell::RepairPolygonOrientationBasedOnNormal()\n");
			YsErrOut("  One or more polygon could not be covered.\n");
			YsErrOut("  Maybe because:\n");
			YsErrOut("    (1)Shell is not closed.\n");
			YsErrOut("    (2)Distorted polygon(s) exist\n");
		}

		RestoreFreeAttribute4OfPolygon(freeAttrib4Save);
		return res;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell::RepairPolygonOrientationAssumingClosedShell(void)
{
	YsArray <YsShellPolygonHandle> invertedPlHd;
	return RepairPolygonOrientationAssumingClosedShell(invertedPlHd);
}

YSRESULT YsShell::RepairPolygonOrientationAssumingClosedShell
	    (YsArray <YsShellPolygonHandle> &invertedPlHdList)
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::RepairPolygonOrientationAssumingClosedShell()\n");
		YsErrOut("  Search Table is required for this operation.\n");
		YsErrOut("  Use AttachSearchTable(YsShellSearchTable *search);\n");
		YsErrOut("  to attach a search table.\n");
		return YSERR;
	}

	YsPrintf("YsShell::RepairFlipDirectionAssumingClosedShell\n");

	invertedPlHdList.Set(0,NULL);

	YsShellPolygonHandle plHd;
	YsArray <int> freeAttrib4Save;
	if(SaveFreeAttribute4OfPolygon(freeAttrib4Save)==YSOK)
	{
		int i,n,nOverlap;
		YsShellPolygon *plg;
		YsLoopCounter ctr;

		ClearFreeAttribute4OfPolygon(0);

		n=GetNumPolygon();

		YsPrintf("  Marking Overlapping Polygon\n");
		plHd=NULL;
		i=0;
		nOverlap=0;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			if(IsOverlappingPolygon(plHd)==YSTRUE)
			{
				plg=GetPolygon(plHd);
				plg->freeAttribute4=1;
				SetNormalOfPolygon(plHd,YsOrigin());
				nOverlap++;
			}

			i++;
		}
		ctr.EndCounter();
		YsPrintf("[%d overlap]\n",nOverlap);

		YsPrintf("  Repairing Flip Direction\n");
		plHd=NULL;
		i=0;

		int nTst,nDone;
		nTst=0;
		nDone=0;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				nTst++;
				if(FindRepairFlipDirectionOfOneReliablePolygon(invertedPlHdList,plHd)==YSTRUE)
				{
					nDone++;
					RecursivelyRepairFlipDirection(nDone,invertedPlHdList,plHd);
				}
			}

			i++;
		}
		ctr.EndCounter();
		YsPrintf("#TST=%d #DONE=%d\n",nTst,nDone);


		YsPrintf("  Verifying coverage\n");
		YSRESULT res;
		res=YSOK;
		plHd=NULL;
		i=0;
		ctr.BeginCounter(n);
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			YsShellPolygon *plg;
			plg=GetPolygon(plHd);
			if(plg->freeAttribute4==0)
			{
				res=YSERR;
			}

			i++;
		}
		ctr.EndCounter();

		if(res!=YSOK)
		{
			YsErrOut("YsShell::RepairFlipDirectionAssumingClosedShell()\n");
			YsErrOut("  One or more polygon could not be covered.\n");
			YsErrOut("  Maybe because:\n");
			YsErrOut("    (1)Shell is not closed.\n");
			YsErrOut("    (2)Distorted polygon(s) exist\n");
		}

		RestoreFreeAttribute4OfPolygon(freeAttrib4Save);
		return res;
	}
	else
	{
		return YSERR;
	}
}

YSBOOL YsShell::IsOverlappingPolygon(YsShellPolygonHandle plHd)  // search table required
{
	if(searchTable==NULL)
	{
		YsErrOut("YsShell::IsOverlappingPolygon()\n");
		YsErrOut("  Search table required.\n");
		return YSFALSE;
	}

	YsOneMemoryBlock <YsShellVertexHandle,128> vtHdBlock;
	YsShellVertexHandle *vtHd;
	int nVtxPlg;
	YsArray <YsShellPolygonHandle,16> plgList;

	nVtxPlg=GetNumVertexOfPolygon(plHd);

	vtHd=vtHdBlock.GetMemoryBlock(nVtxPlg);

	GetVertexListOfPolygon(vtHd,nVtxPlg,plHd);

	searchTable->FindPolygonListByVertexList(plgList,*this,nVtxPlg,vtHd);
	if(plgList.GetN()>=2)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

const YsShell &YsShell::operator=(const YsShell &from)
{
	if(this!=&from)
	{
		from.Encache();

		CleanUp();

		YsShellVertexHandle fromVtHd;
		YsShellPolygonHandle fromPlHd;

		for(fromVtHd=from.FindNextVertex(NULL); fromVtHd!=NULL; fromVtHd=from.FindNextVertex(fromVtHd))
		{
			YsShellVertex *neoVtx;
			YsShellVertexHandle neoVtHd;
			const YsShellVertex *fromVtx;

			fromVtx=from.GetVertex(fromVtHd);
			neoVtHd=AddVertexH(fromVtx->GetPosition());
			neoVtx=GetVertex(neoVtHd);

			neoVtx->CopyAttribute(*fromVtx);
		}

		Encache();

		YsArray <int> vtIdList;
		for(fromPlHd=from.FindNextPolygon(NULL); fromPlHd!=NULL; fromPlHd=from.FindNextPolygon(fromPlHd))
		{
			YsShellPolygon *neoPlg;
			YsShellPolygonHandle neoPlHd;
			const YsShellPolygon *fromPlg;


			fromPlg=from.GetPolygon(fromPlHd);

			vtIdList.Set(fromPlg->GetNumVertex(),NULL);
			from.GetVertexListOfPolygon((int *)vtIdList,vtIdList.GetN(),from.GetPolygonIdFromHandle(fromPlHd));
			neoPlHd=AddPolygonH(vtIdList.GetN(),(int *)vtIdList);
			neoPlg=GetPolygon(neoPlHd);

			neoPlg->SetNormal(fromPlg->GetNormal());  // Added 2000/11/10
			neoPlg->SetColor(fromPlg->GetColor());    // Added 2000/11/10

			neoPlg->CopyAttribute(*fromPlg);
		}

		mat=from.mat;
		matrixIsSet=from.matrixIsSet;
		trustPolygonNormal=from.trustPolygonNormal;

		readSrfVar=NULL;
	}

	return *this;
}

int YsShell::GetNumPolygon(void) const
{
	return plg.GetN();
}

YsShellPolygon *YsShell::GetPolygon(int id)
{
	YsListItem <YsShellPolygon> *seeker;
	seeker=plg.GetItemFromId(id);
	if(seeker!=NULL)
	{
		return &seeker->dat;
	}
	else
	{
		return NULL;
	}
}

const YsShellPolygon *YsShell::GetPolygon(int id) const
{
	const YsListItem <YsShellPolygon> *seeker;
	seeker=plg.GetItemFromId(id);
	if(seeker!=NULL)
	{
		return &seeker->dat;
	}
	else
	{
		return NULL;
	}
}

YsShellPolygon *YsShell::GetPolygon(YsShellPolygonHandle plHd)
{
	if(plHd!=NULL)
	{
		if(plHd->GetContainer()==&plg) // 2006/02/07 plHd->dat.belongTo==this)
		{
			return (YsShellPolygon *)(&plHd->dat);
		}
		else
		{
			YsErrOut("YsShell::GetPolygon()\n");
			YsErrOut("  Trying to access to a polygon by a handle for another shell!!\n");
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

YsShellVertex *YsShell::GetVertex(YsShellVertexHandle vtHd)
{
	if(vtHd!=NULL)
	{
		if(vtHd->GetContainer()==&vtx) // 2006/02/07 dat.belongTo==this)
		{
			return (YsShellVertex *)(&vtHd->dat);
		}
		else
		{
			YsErrOut("YsShell::GetVertex()\n");
			YsErrOut("  Trying to access to a vertex by a handle for another shell!!\n");
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

const YsShellPolygon *YsShell::GetPolygon(YsShellPolygonHandle plHd) const
{
	if(plHd!=NULL)
	{
		if(plHd->GetContainer()==&plg) // 2006/02/07 dat.belongTo==this)
		{
			return &plHd->dat;
		}
		else
		{
			YsErrOut("YsShell::GetPolygon() const\n");
			YsErrOut("  Trying to access to a polygon by a handle for another shell!!\n");
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

const YsShellVertex *YsShell::GetVertex(YsShellVertexHandle vtHd) const
{
	if(vtHd!=NULL)
	{
		if(vtHd->GetContainer()==&vtx) // 2006/02/07 dat.belongTo==this)
		{
			return &vtHd->dat;
		}
		else
		{
			YsErrOut("YsShell::GetVertex() const\n");
			YsErrOut("  Trying to access to a vertex by a handle for another shell!!\n");
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

YSBOOL YsShell::IsFrozen(YsShellPolygonHandle plHd) const
{
	if(plHd->GetContainer()==&frozenPlg)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsShell::IsFrozen(YsShellVertexHandle vtHd) const
{
	if(vtHd->GetContainer()==&frozenVtx)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

void YsShell::GetBoundingBox(YsVec3 &min,YsVec3 &max) const
{
	if(bbxIsValid==YSTRUE)
	{
		min=bbx1;
		max=bbx2;
	}
	else
	{
		YsShellVertexHandle vtHd;
		YsBoundingBoxMaker3 bbx;

		vtHd=FindNextVertex(NULL);
		if(vtHd!=NULL)
		{
			YsVec3 vtxPos;

			GetVertexPosition(vtxPos,vtHd);
			bbx.Begin(vtxPos);
			while(vtHd!=NULL)
			{
				GetVertexPosition(vtxPos,vtHd);
				bbx.Add(vtxPos);
				vtHd=FindNextVertex(vtHd);
			}
			bbx.Get(min,max);

			bbx1=min;
			bbx2=max;
			bbxIsValid=YSTRUE;
		}
		else
		{
			min.Set(0.0,0.0,0.0);
			max.Set(0.0,0.0,0.0);
			bbxIsValid=YSFALSE;  // <- May not necessary.  Defensive coding.
		}
	}
}

int YsShell::GetNumVertex(void) const
{
	return vtx.GetN();
}

int YsShell::GetMaxNumVertexOfPolygon(void) const
{
	int n,t;
	int i;
	n=0;
	for(i=0; i<GetNumPolygon(); i++)
	{
		t=GetNumVertexOfPolygon(i);
		n=YsGreater(n,t);
	}
	return n;
}

YSRESULT YsShell::GetVertexPosition(YsVec3 &pos,int vtId) const
{
	return GetVertexPosition(pos,GetVertexHandleFromId(vtId));
}

YSRESULT YsShell::GetVertexPosition(YsVec3 &pos,YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		if(matrixIsSet==YSTRUE)
		{
			pos=mat*vtx->GetPosition();
		}
		else
		{
			pos=vtx->GetPosition();
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

int YsShell::GetVertexReferenceCount(int vtId) const
{
	return GetVertexReferenceCount(GetVertexHandleFromId(vtId));
}

// 10/22/2001
YSRESULT YsShell::GetAverageNormalAtVertex(YsVec3 &nom,YsShellVertexHandle vtHd) const
{
	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsShell::GetAverageNormalAtVertex()\n");
		YsErrOut("  Search Table is required for this operation.\n");
		YsErrOut("  Use AttachSearchTable(YsShellSearchTable *search);\n");
		YsErrOut("  to attach a search table.\n");
		return YSERR;
	}

	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	if(search->FindPolygonListByVertex(nFoundPlHd,foundPlHd,*this,vtHd)==YSOK && nFoundPlHd>0)
	{
		int i;
		YsVec3 sub;
		nom=YsOrigin();
		for(i=0; i<nFoundPlHd; i++)
		{
			GetNormalOfPolygon(sub,foundPlHd[i]);
			nom+=sub;
		}
		nom.Normalize();
		return YSOK;
	}
	else
	{
		nom=YsOrigin();
		return YSOK;
	}
}

unsigned YsShell::GetSearchKey(YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetSearchKey();
	}
	else
	{
		return 0;
	}
}

unsigned YsShell::GetSearchKey(YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetSearchKey();
	}
	else
	{
		return 0;
	}
}

int YsShell::GetVertexReferenceCount(YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetReferenceCount();
	}
	else
	{
		return -1;
	}
}

YsShellVertex *YsShell::GetVertex(int id)
{
	YsShellVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(id);
	return GetVertex(vtHd);
}

const YsShellVertex *YsShell::GetVertex(int id) const
{
	YsShellVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(id);
	return GetVertex(vtHd);
}

int YsShell::GetNumVertexOfPolygon(int plId) const
{
	return GetNumVertexOfPolygon(GetPolygonHandleFromId(plId));
}

YSRESULT YsShell::GetVertexListOfPolygon(int &nPlVt,const YsShellVertexHandle *&plVtHd,YsShellPolygonHandle plHd) const
{
	nPlVt=GetNumVertexOfPolygon(plHd);
	plVtHd=GetVertexListOfPolygon(plHd);
	if(nPlVt>0 && plVtHd!=NULL)
	{
		return YSOK;
	}
	return YSERR;
}

int YsShell::GetNumVertexOfPolygon(YsShellPolygonHandle plHd) const
{
	if(plHd!=NULL)
	{
		return GetPolygon(plHd)->GetNumVertex();
	}
	else
	{
		return 0;
	}
}

YSRESULT YsShell::GetVertexListOfPolygon(int idx[],int maxcount,int plId) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plId);
	if(plg!=NULL)
	{
		int i,nVtId;
		nVtId=plg->GetNumVertex();

		for(i=0; i<nVtId && i<maxcount; i++)
		{
			idx[i]=plg->GetVertexId(i);
		}
		if(nVtId<=maxcount)
		{
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsVec3 v[],int maxcount,int plId) const
{
	YsShellPolygonHandle plHd;
	plHd=GetPolygonHandleFromId(plId);
	return GetVertexListOfPolygon(v,maxcount,plHd);
}

// Discarded on 2001/04/26
//const int *YsShell::GetVertexListOfPolygon(int plId) const
//{
//	const YsShellPolygon *plg;
//	plg=GetPolygon(plId);
//	if(plg!=NULL)
//	{
//		return plg->GetIndexArray();
//	}
//	return NULL;
//}

YSRESULT YsShell::GetVertexListOfPolygon(YsShellVertexHandle vtHd[],int maxcount,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		int i,nVtId;
		nVtId=plg->GetNumVertex();

		for(i=0; i<nVtId && i<maxcount; i++)
		{
			vtHd[i]=plg->GetVertexHandle(i);
		}
		if(nVtId<=maxcount)
		{
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsVec3 v[],int maxcount,YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		int i,nVtId;
		YsShellVertexHandle vtHd;
		nVtId=plg->GetNumVertex();

		for(i=0; i<nVtId && i<maxcount; i++)
		{
			vtHd=plg->GetVertexHandle(i);

			const YsShellVertex *vtx;
			vtx=GetVertex(vtHd);
			if(vtx!=NULL)
			{
				if(matrixIsSet==YSTRUE)
				{
					v[i]=mat*vtx->GetPosition();
				}
				else
				{
					v[i]=vtx->GetPosition();
				}
			}
			else
			{
				return YSERR;
			}
		}
		if(nVtId<=maxcount)
		{
			return YSOK;
		}
	}
	return YSERR;
}

const YsShellVertexHandle *YsShell::GetVertexListOfPolygon(YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetVertexArray();
	}
	return NULL;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsArray <YsShellVertexHandle> &vtHd,YsShellPolygonHandle plHd) const
{
	int nVtxPlg;
	nVtxPlg=GetNumVertexOfPolygon(plHd);
	if(nVtxPlg>0)
	{
		vtHd.Set(nVtxPlg,GetVertexListOfPolygon(plHd));
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsArray <YsVec3> &vtx,YsShellPolygonHandle plHd) const
{
	int nVtxPlg;
	nVtxPlg=GetNumVertexOfPolygon(plHd);
	if(nVtxPlg>0)
	{
		int i;
		const YsShellVertexHandle *vtHd;

		vtx.Set(nVtxPlg,NULL);
		vtHd=GetVertexListOfPolygon(plHd);
		for(i=0; i<nVtxPlg; i++)
		{
			YsVec3 vtxPos;
			GetVertexPosition(vtxPos,vtHd[i]);
			vtx.SetItem(i,vtxPos);
		}
		return YSOK;

	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListFromPolygonList(YsArray <YsShellVertexHandle> &vtHdList,int nPl,const YsShellPolygonHandle plHdList[]) const
{
	int i,j,nVtxMax,nVtx;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	YsArray <YsShellVertexHandle,64> saveFreeAtt4VtHd;
	YsArray <int,64> saveFreeAtt4Value;

	nVtxMax=0;
	for(i=0; i<nPl; i++)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
		for(j=0; j<nPlVt; j++)
		{
			saveFreeAtt4VtHd.Append(plVtHd[j]);
			saveFreeAtt4Value.Append(GetVertex(plVtHd[j])->freeAttribute4);
			GetVertex(plVtHd[j])->freeAttribute4=0;
		}
		nVtxMax+=nPlVt;
	}

	vtHdList.Set(nVtxMax,NULL);
	nVtx=0;
	for(i=0; i<nPl; i++)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
		for(j=0; j<nPlVt; j++)
		{
			if(GetVertex(plVtHd[j])->freeAttribute4==0)
			{
				vtHdList[nVtx++]=plVtHd[j];
				GetVertex(plVtHd[j])->freeAttribute4=1;
			}
		}
	}
	vtHdList.Resize(nVtx);

	forYsArrayRev(i,saveFreeAtt4VtHd)
	{
		GetVertex(saveFreeAtt4VtHd[i])->freeAttribute4=saveFreeAtt4Value[i];
	}

	return YSOK;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsArray <YsShellVertexHandle,16> &vtHd,YsShellPolygonHandle plHd) const
{
	int nVtxPlg;
	nVtxPlg=GetNumVertexOfPolygon(plHd);
	if(nVtxPlg>0)
	{
		vtHd.Set(nVtxPlg,GetVertexListOfPolygon(plHd));
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListOfPolygon(YsArray <YsVec3,16> &vtx,YsShellPolygonHandle plHd) const
{
	int nVtxPlg;
	nVtxPlg=GetNumVertexOfPolygon(plHd);
	if(nVtxPlg>0)
	{
		int i;
		const YsShellVertexHandle *vtHd;

		vtx.Set(nVtxPlg,NULL);
		vtHd=GetVertexListOfPolygon(plHd);
		for(i=0; i<nVtxPlg; i++)
		{
			YsVec3 vtxPos;
			GetVertexPosition(vtxPos,vtHd[i]);
			vtx.SetItem(i,vtxPos);
		}
		return YSOK;

	}
	return YSERR;
}

YSRESULT YsShell::GetVertexListFromPolygonList(YsArray <YsShellVertexHandle,16> &vtHdList,int nPl,const YsShellPolygonHandle plHdList[]) const
{
	int i,j,nVtxMax,nVtx;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	YsArray <YsShellVertexHandle,64> saveFreeAtt4VtHd;
	YsArray <int,64> saveFreeAtt4Value;

	nVtxMax=0;
	for(i=0; i<nPl; i++)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
		for(j=0; j<nPlVt; j++)
		{
			saveFreeAtt4VtHd.Append(plVtHd[j]);
			saveFreeAtt4Value.Append(GetVertex(plVtHd[j])->freeAttribute4);
			GetVertex(plVtHd[j])->freeAttribute4=0;
		}
		nVtxMax+=nPlVt;
	}

	vtHdList.Set(nVtxMax,NULL);
	nVtx=0;
	for(i=0; i<nPl; i++)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
		for(j=0; j<nPlVt; j++)
		{
			if(GetVertex(plVtHd[j])->freeAttribute4==0)
			{
				vtHdList[nVtx++]=plVtHd[j];
				GetVertex(plVtHd[j])->freeAttribute4=1;
			}
		}
	}
	vtHdList.Resize(nVtx);

	forYsArrayRev(i,saveFreeAtt4VtHd)
	{
		GetVertex(saveFreeAtt4VtHd[i])->freeAttribute4=saveFreeAtt4Value[i];
	}

	return YSOK;
}

double YsShell::GetAreaOfPolygon(YsShellPolygonHandle plHd) const
{
	int i;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	YsVec3 totalCrsPrd,crsPrd,o,p1,p2;

	nPlVt=GetNumVertexOfPolygon(plHd);
	if(nPlVt>=3)
	{
		plVtHd=GetVertexListOfPolygon(plHd);
		totalCrsPrd=YsOrigin();
		GetVertexPosition(o,plVtHd[0]);
		GetVertexPosition(p2,plVtHd[1]);
		for(i=1; i<nPlVt-1; i++)
		{
			p1=p2;
			GetVertexPosition(p2,plVtHd[i+1]);
			crsPrd=(p2-o)^(p1-o);
			totalCrsPrd+=crsPrd;
		}
		return totalCrsPrd.GetLength()/2.0;
	}
	return 0.0;
}

double YsShell::GetPolygonEdgeLength(YsShellPolygonHandle plHd,int edId) const
{
	YsShellVertexHandle edVtHd[2];
	if(GetPolygonEdgeVertex(edVtHd,plHd,edId)==YSOK)
	{
		return GetEdgeLength(edVtHd[0],edVtHd[1]);
	}
	return 0.0;
}

double YsShell::GetHeightOfPolygon(YsShellPolygonHandle plHd,YsShellVertexHandle baseEdVtHd[2]) const
{
	int i,nPlVt;
	const YsShellVertexHandle *plVtHd;
	YsVec3 basePos[2],tst;
	double hei,d;

	GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
	GetVertexPosition(basePos[0],baseEdVtHd[0]);
	GetVertexPosition(basePos[1],baseEdVtHd[1]);

	hei=0;
	for(i=0; i<nPlVt; i++)
	{
		GetVertexPosition(tst,plVtHd[i]);
		d=YsGetPointLineDistance3(basePos[0],basePos[1],tst);
		if(d>hei)
		{
			hei=d;
		}
	}
	return hei;
}

YSRESULT YsShell::GetPolygonGroupByNormalSimilarity
   (YsArray <YsShellPolygonHandle,16> &plHdList,YsShellPolygonHandle seedPlHd,
    YsKeyStore *excludePlKey,const double &cosThr) const
{
	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search!=NULL)
	{
		YsVec3 refNom;
		YsKeyStore visited(256);
		YsArray <YsShellPolygonHandle,16> todo;

		visited.EnableAutoResizing(256,GetNumPolygon()/4+256);

		GetNormalOfPolygon(refNom,seedPlHd);

		todo.Append(seedPlHd);
		plHdList.Set(seedPlHd);
		visited.AddKey(GetSearchKey(seedPlHd));
		while(todo.GetN()>0)
		{
			int i,nPlVt;
			YsShellPolygonHandle plHd,neiPlHd;
			YsVec3 neiNom;

			plHd=todo[0];
			nPlVt=GetNumVertexOfPolygon(plHd);
			for(i=0; i<nPlVt; i++)
			{
				neiPlHd=search->GetNeighborPolygon(*this,plHd,i);
				if(neiPlHd!=NULL && visited.IsIncluded(GetSearchKey(neiPlHd))!=YSTRUE)
				{
					visited.AddKey(GetSearchKey(neiPlHd));

					if(excludePlKey!=NULL && excludePlKey->IsIncluded(GetSearchKey(neiPlHd))==YSTRUE)
					{
						continue;
					}

					GetNormalOfPolygon(neiNom,neiPlHd);
					if(neiNom*refNom>=cosThr)
					{
						todo.Append(neiPlHd);
						plHdList.Append(neiPlHd);
					}
				}
			}

			todo.DeleteBySwapping(0);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShell::GetPolygonGroupByNormalSimilarity()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}
}

YSRESULT YsShell::GetCircumferentialAngleOfPolygonGroup(
    double &minAngle,YsShellVertexHandle &minVtHd,double &maxAngle,YsShellVertexHandle &maxVtHd,
    const YsVec3 &org,const YsVec3 &axs,int nPl,const YsShellPolygonHandle *plHdList) const
{
	if(nPl==0)
	{
		minAngle=0.0;
		minVtHd=NULL;
		maxAngle=0.0;
		maxVtHd=NULL;
		return YSERR;
	}

	const YsShellSearchTable *search;
	search=GetSearchTable();
	if(search!=NULL)
	{
		int i,j,nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsKeyStore visitedVtx(nPl+1),usedVtx(nPl+1);
		YsArray <YsPair <YsShellVertexHandle,double>,16> todo;
		YsPair <YsShellVertexHandle,double> vtxAnglePair;
		YsArray <YsShellVertexHandle,16> connVtHd;

		for(i=0; i<nPl; i++)
		{
			GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[i]);
			for(j=0; j<nPlVt; j++)
			{
				usedVtx.AddKey(GetSearchKey(plVtHd[j]));
			}
		}

		GetVertexListOfPolygon(nPlVt,plVtHd,plHdList[0]);
		vtxAnglePair.a=plVtHd[0];
		vtxAnglePair.b=0.0;
		visitedVtx.AddKey(GetSearchKey(plVtHd[0]));

		todo.Append(vtxAnglePair);
		minAngle=0.0;
		minVtHd=plVtHd[0];
		maxAngle=0.0;
		maxVtHd=plVtHd[0];

		while(todo.GetN()>0)
		{
			YsVec3 v1,v2,root;
			search->GetConnectedVertexList(connVtHd,*this,todo[0].a);

			GetVertexPosition(v1,todo[0].a);
			root=org+axs*((v1-org)*axs);
			v1-=root;
			if(v1.Normalize()==YSOK)
			{
				forYsArray(i,connVtHd)
				{
					if(usedVtx.IsIncluded(GetSearchKey(connVtHd[i]))==YSTRUE &&
					   visitedVtx.IsIncluded(GetSearchKey(connVtHd[i]))!=YSTRUE)
					{
						double d,a;

						visitedVtx.AddKey(GetSearchKey(connVtHd[i]));

						GetVertexPosition(v2,connVtHd[i]);
						root=org+axs*((v2-org)*axs);
						v2-=root;

						if(v2.Normalize()==YSOK)
						{
							d=acos(v1*v2);
							if((v1^v2)*axs>=0.0)
							{
								a=todo[0].b+d;
							}
							else
							{
								a=todo[0].b-d;
							}

							if(maxAngle<a)
							{
								maxAngle=a;
								maxVtHd=connVtHd[i];
							}
							if(a<minAngle)
							{
								minAngle=a;
								minVtHd=connVtHd[i];
							}

							vtxAnglePair.a=connVtHd[i];
							vtxAnglePair.b=a;
							todo.Append(vtxAnglePair);
						}
					}
				}
			}

			todo.DeleteBySwapping(0);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShell::GetCircumferentialAngleOfPolygonGroup()\n");
		YsErrOut("  Error: Search table is not attached.\n");
		return YSERR;
	}
}

void YsShell::GetMatrix(YsMatrix4x4 &m) const
{
	m=mat;
}

YSSIDE YsShell::CountRayIntersection
    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,int plIdSkip,YSBOOL watch) const
{
	YsShellPolygonHandle plHd;

	if(watch==YSTRUE)
	{
		YsPrintf("YsShell::CountRayIntersection <- plIdSkip\n");
	}

	if(plIdSkip>=0)
	{
		plHd=GetPolygonHandleFromId(plIdSkip);
	}
	else
	{
		plHd=NULL;
	}
	return CountRayIntersection(CrossCounter,org,vec,plHd,watch);
}

YSSIDE YsShell::CountRayIntersection
	    (int &CrossCounter,const YsVec3 &org,const YsVec3 &vec,YsShellPolygonHandle plHdSkip,YSBOOL watch) const
{
	YsShellPolygonHandle plHd;
	int nVtx;
	YsVec3 crs;
	YsArray <YsVec3> vtx;
	YsPlane pln;

	CrossCounter=0;

	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		if(plHd==plHdSkip)
		{
			continue;
		}

		nVtx=GetNumVertexOfPolygon(plHd);
		GetVertexListOfPolygon(vtx,plHd);

		YsVec3 nom;
		if(TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(nom,plHd)==YSOK && nom!=YsOrigin())
		{
			pln.Set(vtx[0],nom);
		}
		else if(pln.MakeBestFitPlane(nVtx,vtx)!=YSOK)
		{
			YsErrOut("YsShell::CountRayIntersection\n  Skipping Bad polygon\n");
			// int j;
			// for(j=0; j<nVtx; j++)
			// {
			// 	YsErrOut("(%lf %lf %lf)\n",vtx[j].x(),vtx[j].y(),vtx[j].z());
			// }
			continue;
		}



		if(pln.GetIntersection(crs,org,vec)==YSOK)
		{
			if(crs==org)
			{
				YSSIDE side;
				side=YsCheckInsidePolygon3(crs,nVtx,vtx,pln.GetNormal());
				if(side==YSBOUNDARY || side==YSINSIDE)
				{
					return YSBOUNDARY;
				}
			}
			else if(vec*(crs-org)>=0.0)
			{
				YSSIDE side;
				side=YsCheckInsidePolygon3(crs,nVtx,vtx,pln.GetNormal());
				if(side==YSBOUNDARY || side==YSUNKNOWNSIDE)
				{
					return YSUNKNOWNSIDE;
				}
				if(side==YSINSIDE)
				{
					if(watch==YSTRUE)
					{
						YsPrintf("Intersect Key=%d,Id=%d,(%s)\n",
						    GetSearchKey(plHd),GetPolygonIdFromHandle(plHd),crs.Txt());
						int i;
						YsPrintf("nVtx=%d\n",nVtx);
						for(i=0; i<nVtx; i++)
						{
							YsPrintf("(%s)\n",vtx[i].Txt());
						}
					}
					CrossCounter++;
				}
			}
		}
		else if(pln.CheckOnPlane(org)==YSTRUE)
		{
			YSSIDE side;
			side=YsCheckInsidePolygon3(org,nVtx,vtx,pln.GetNormal());
			if(side==YSBOUNDARY || side==YSINSIDE)
			{
				return YSBOUNDARY;
			}
		}
	}

	if(watch==YSTRUE)
	{
		YsPrintf("Counter=%d\n",CrossCounter);
	}

	if(CrossCounter%2==0)
	{
		return YSOUTSIDE;
	}
	else
	{
		return YSINSIDE;
	}
}

YSSIDE YsShell::CheckInsideSolid(const YsVec3 &pnt,YSBOOL watch) const
{
	int c;
	double h,p;
	YsVec3 vec;
	int CrossCounter;
	YsShellPolygonHandle plHdSkip=NULL;

	YSSIDE side;

	h=YsDegToRad(0);
	p=YsDegToRad(-90);
	for(c=0; c<256; c++)
	{
		vec.Set(cos(p)*cos(h),sin(p),cos(p)*sin(h));

		if(watch==YSTRUE)
		{
			YsPrintf("(%s)-(%s) %s\n",pnt.Txt(),(pnt+vec).Txt(),vec.Txt());
		}

		side=CountRayIntersection(CrossCounter,pnt,vec,plHdSkip,watch);

		if(side!=YSUNKNOWNSIDE)
		{
			return side;
		}

		p+=YsDegToRad(30.0);
		h+=YsDegToRad(95.0);
	}

	YsErrOut("YsShell::IsInsideSolid\n  Could not determine inside/outside/boundary.\n");
	return YSUNKNOWNSIDE;
}

YSRESULT YsShell::InvertPolygon(int plId)
{
	YsShellPolygonHandle plHd;
	plHd=GetPolygonHandleFromId(plId);
	return InvertPolygon(plHd);
}

YSRESULT YsShell::InvertPolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		plg->Invert();
		return YSOK;
	}
	return YSERR;
}


YSRESULT YsShell::ShootFiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShellPolygonHandle> &plHdLst,
	     const YsVec3 &p1,const YsVec3 &p2,const YsShellLattice *ltc) const
{
	int i,nVtx;
	YsVec3 vtx[256];
	YsArray <YsShellPolygonHandle,16> plgList;
	YsShellPolygonHandle plHd;

	is.Set(0,NULL);
	plHdLst.Set(0,NULL);

	if(ltc!=NULL)
	{
		YsVec3 r[2];
		r[0]=p1;
		r[1]=p2;
		ltc->MakePossibllyCollidingPolygonList(plgList,2,r);
	}
	else
	{
		plgList.Set(GetNumPolygon(),NULL);
		plHd=NULL;
		i=0;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			plgList.SetItem(i,plHd);
			i++;
		}
	}

	for(i=0; i<plgList.GetNumItem(); i++)
	{
		plHd=plgList[i];
		if((nVtx=GetNumVertexOfPolygon(plHd))>=3 &&
		    GetVertexListOfPolygon(vtx,256,plHd)==YSOK)
		{
			YsVec3 o,n,itsc;
			YsPlane pln;

			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK && n!=YsOrigin()) ||
			   YsGetAverageNormalVector(n,nVtx,vtx)==YSOK)
			{
				o=vtx[0];
				pln.Set(o,n);
				if(pln.GetPenetration(itsc,p1,p2)==YSOK)
				{
					YSSIDE side;
					side=YsCheckInsidePolygon3(itsc,nVtx,vtx);
					if(side==YSINSIDE || side==YSBOUNDARY)
					{
						is.AppendItem(itsc);
						plHdLst.AppendItem(plHd);
					}
				}
			}
		}
	}

	return YSOK;
}

YSRESULT YsShell::ShootInfiniteRay
	    (YsArray <YsVec3> &is,YsArray <YsShellPolygonHandle> &plHdLst,
	     const YsVec3 &org,const YsVec3 &vec,const YsShellLattice *ltc) const
{
	YsVec3 bbx1,bbx2,cen;

	is.Set(0,NULL);
	plHdLst.Set(0,NULL);

	GetBoundingBox(bbx1,bbx2);
	cen=(bbx1+bbx2)/2.0;
	bbx1=cen+(bbx1-cen)*1.02;  // 2% Safety Margine  <- Actually, needed for cubes.
	bbx2=cen+(bbx2-cen)*1.02;  //                       in order to make GetPenetration return YSOK

	YsVec3 clip1,clip2;
	if(YsClipInfiniteLine3(clip1,clip2,org,vec,bbx1,bbx2)==YSOK)
	{
		double ip1,ip2;
		ip1=(clip1-org)*vec;
		ip2=(clip2-org)*vec;

		if(ip1>=0.0 && ip2>=0.0)
		{
			return ShootFiniteRay(is,plHdLst,clip1,clip2,ltc);
		}
		else if(ip1>=0.0)
		{
			return ShootFiniteRay(is,plHdLst,org,clip1,ltc);
		}
		else if(ip2>=0.0)
		{
			return ShootFiniteRay(is,plHdLst,org,clip2,ltc);
		}
	}

	// Falling into this point means either:
	//   1. even the ray didn't hit the bounding box, or
	//   2. the ray did hit the bounding box, but both intersections were behind.

	return YSOK;
}

int YsShell::ShootRay(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const
{
	YsShellPolygonHandle plHd;
	plHd=ShootRayH(intersect,p1,vec);
	return GetPolygonIdFromHandle(plHd);
}

YsShellPolygonHandle YsShell::ShootRayH(YsVec3 &intersect,const YsVec3 &p1,const YsVec3 &vec) const
{
	int nVtx;
	YsVec3 vtx[256],p2;

	p2=p1+vec;

	YsShellPolygonHandle plHd;
	YsShellPolygonHandle possiblePlHd;
	double possibleDistance;
	YsVec3 possibleCollidePoint;

	possiblePlHd=NULL;
	possibleDistance=(p2-p1).GetSquareLength();
	possibleCollidePoint.Set(0.0,0.0,0.0);

	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		if((nVtx=GetNumVertexOfPolygon(plHd))>=3 &&
		    GetVertexListOfPolygon(vtx,256,plHd)==YSOK)
		{
			YsVec3 o,n,i;
			YsPlane pln;

			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK && n!=YsOrigin()) ||
			   YsGetAverageNormalVector(n,nVtx,vtx)==YSOK)
			{
				o=vtx[0];
				pln.Set(o,n);
				if(pln.GetPenetration(i,p1,p2)==YSOK)
				{
					YSSIDE side;
					side=YsCheckInsidePolygon3(i,nVtx,vtx);
					if((side==YSINSIDE || side==YSBOUNDARY) &&
					   (p1-i).GetSquareLength()<possibleDistance)
					{
						possibleDistance=(p1-i).GetSquareLength();
						possibleCollidePoint=i;
						possiblePlHd=plHd;
					}
				}
			}
		}
	}

	if(possiblePlHd!=NULL)
	{
		intersect=possibleCollidePoint;
	}

	return possiblePlHd;
}

void YsShell::ShootRay(YsArray <YsVec3> &intersect,const YsVec3 &p1,const YsVec3 &vec) const
{
	YsShellPolygonHandle plHd;
	int nVtx;
	YsVec3 vtx[256],p2;

	p2=p1+vec;

	intersect.Set(0,NULL);

	for(plHd=FindNextPolygon(NULL); plHd!=NULL; plHd=FindNextPolygon(plHd))
	{
		if((nVtx=GetNumVertexOfPolygon(plHd))>=3 &&
		    GetVertexListOfPolygon(vtx,256,plHd)==YSOK)
		{
			YsVec3 o,n,i;
			YsPlane pln;
			if((TrustPolygonNormal()==YSTRUE && GetNormalOfPolygon(n,plHd)==YSOK) ||
			    YsGetAverageNormalVector(n,nVtx,vtx)==YSOK)
			{
				o=vtx[0];
				pln.Set(o,n);
				if(pln.GetPenetration(i,p1,p2)==YSOK)
				{
					YSSIDE side;
					side=YsCheckInsidePolygon3(i,nVtx,vtx);
					if(side==YSINSIDE || side==YSBOUNDARY)
					{
						intersect.AppendItem(i);
					}
				}
			}
		}
	}
}


YSRESULT YsShell::BeginReadSrf(void)
{
	if(BeginMergeSrf()==YSOK)
	{
		CleanUp();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::BeginMergeSrf(void)
{
	if(readSrfVar!=NULL)
	{
		delete readSrfVar;
		readSrfVar=NULL;
	}

	readSrfVar=new YsShellReadSrfVariable;

	if(readSrfVar!=NULL)
	{
		return YSOK;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::BegimMergeSrf()");
		return YSERR;
	}
}

YSRESULT YsShell::ReadSrfOneLine
    (const char str[],YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	int ac;

	if(str[0]=='#')  // Ignore comment line.
	{
		return YSOK;
	}

	YsArguments2(readSrfVar->args,str," \t","");
	ac=readSrfVar->args.GetN();
	if(ac>0)
	{
		if(readSrfVar->mode==0)  // Reading Vertices or other commands
		{
			YsVec3 v;
			YsShellVertexHandle vtHd;
			switch(str[0])
			{
			case 'S': // SURF
			case 's':
				break;
			case 'V': // VERTEX
			case 'v':
				v.Set(atof(readSrfVar->args[1]),atof(readSrfVar->args[2]),atof(readSrfVar->args[3]));
				vtHd=AddVertexH(v);
				readSrfVar->allVtHdList.Append(vtHd);
				if(ac>=5 && (readSrfVar->args[4][0]=='R' || readSrfVar->args[4][0]=='r') && roundVtx!=NULL)
				{
					roundVtx->Append(vtHd);
				}
				break;
			case 'E': // END
			case 'e':
				break;
			case 'F': // FACE
			case 'f':
				readSrfVar->mode=1;
				readSrfVar->plVtHdList.Set(0,NULL);
				readSrfVar->nom.Set(0.0,0.0,0.0);
				readSrfVar->noShadingPolygon=YSFALSE;
				readSrfVar->color.SetIntRGB(128,128,128);
				break;
			default:                // 2004/09/10
				return YSERR;       // 2004/09/10
			}
			return YSOK;
		}
		else // Reading Face
		{
			int i;
			switch(str[0])
			{
			case 'C': // COLOR
			case 'c':
				if(ac==2)
				{
					readSrfVar->color.Set15BitRGB(atoi(readSrfVar->args[1]));
				}
				else
				{
					readSrfVar->color.SetIntRGB
					    (atoi(readSrfVar->args[1]),atoi(readSrfVar->args[2]),atoi(readSrfVar->args[3]));
				}
				break;
			case 'B':
			case 'b':
				readSrfVar->noShadingPolygon=YSTRUE;
				break;
			case 'N': // CENTER & NORMAL
			case 'n':
				if(ac==7)
				{
					readSrfVar->nom.Set(atof(readSrfVar->args[4]),atof(readSrfVar->args[5]),atof(readSrfVar->args[6]));
				}
				else if(ac==4)
				{
					readSrfVar->nom.Set(atof(readSrfVar->args[1]),atof(readSrfVar->args[2]),atof(readSrfVar->args[3]));
				}
				break;
			case 'V': // VERTEX
			case 'v':
				for(i=1; i<ac; i++)
				{
					readSrfVar->plVtHdList.AppendItem(readSrfVar->allVtHdList[atoi(readSrfVar->args[i])]);
				}
				break;
			case 'E': // ENDFACE
			case 'e':
				readSrfVar->plHd=AddPolygonH(readSrfVar->plVtHdList.GetN(),readSrfVar->plVtHdList);
				SetNormalOfPolygon(readSrfVar->plHd,readSrfVar->nom);
				SetColorOfPolygon(readSrfVar->plHd,readSrfVar->color);
				readSrfVar->mode=0;
				if(noShadingPolygon!=NULL && readSrfVar->noShadingPolygon==YSTRUE)
				{
					noShadingPolygon->Append(readSrfVar->plHd);
				}
				break;
			default:                // 2004/09/10
				return YSERR;       // 2004/09/10
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell::EndReadSrf(void)
{
	if(readSrfVar!=NULL)
	{
		delete readSrfVar;
		readSrfVar=NULL;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::LoadSrf
    (const char fn[],YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	FILE *fp;
	YsArray <YsString,16> args;

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		if(noShadingPolygon!=NULL)
		{
			noShadingPolygon->Set(0,NULL);
		}
		if(roundVtx!=NULL)
		{
			roundVtx->Set(0,NULL);
		}
		if(BeginReadSrf()==YSOK)
		{
			YsString str;
			while(str.Fgets(fp)!=NULL)
			{
				if(ReadSrfOneLine(str,noShadingPolygon,roundVtx)!=YSOK)
				{
					str.Arguments(args);
					if(args.GetN()>0)
					{
						YSBOOL srmExtension;
						srmExtension=YSFALSE;

						args[0].Capitalize();

						switch(args[0][0])
						{
						case 'G':  // GE, GL, GF
						case 'Z':  // ZE, ZF, ZNBE,ZNBF, ZT, ZA, ZZ, ZH, ZU
						case 'Y':  // YE, YF, YN, Y1
						case 'M':  // M
						case 'C':  // CV
						case 'X':  // X
						case 'L':  // LF, LE, LL
							srmExtension=YSTRUE;
							break;
						}

						if(srmExtension!=YSTRUE)
						{
							fclose(fp);
							return YSERR;
						}
					}
				}
			}
			EndReadSrf();
			fclose(fp);
			return YSOK;
		}
		else
		{
			fclose(fp);
			return YSERR;
		}
	}
	return YSERR;
}

YSRESULT YsShell::MergeSrf
    (const char fn[],YsArray <YsShellPolygonHandle> *noShadingPolygon,YsArray <YsShellVertexHandle> *roundVtx)
{
	FILE *fp;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		if(noShadingPolygon!=NULL)
		{
			noShadingPolygon->Set(0,NULL);
		}
		if(roundVtx!=NULL)
		{
			roundVtx->Set(0,NULL);
		}
		if(BeginMergeSrf()==YSOK)
		{
			YsString str;
			while(str.Fgets(fp)!=NULL)
			{
				if(ReadSrfOneLine(str,noShadingPolygon,roundVtx)!=YSOK)
				{
					fclose(fp);
					return YSERR;
				}
			}
			EndReadSrf();
			fclose(fp);
			return YSOK;
		}
		else
		{
			fclose(fp);
			return YSERR;
		}
	}
	return YSERR;
}

YSRESULT YsShell::LoadTri(const char fn[])
{
	FILE *fp;
	int nNod,nTri;
	YsShellVertexHandle *nodVtHd;
	int ac;
	char buf[256],str[256],*av[32];

	nNod=0;
	nTri=0;
	nodVtHd=NULL;

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		while(fgets(str,256,fp)!=NULL)
		{
			strcpy(buf,str);
			YsArguments(&ac,av,32,buf);
			if(ac>0)
			{
				if(strcmp(av[0],"nofnod")==0)
				{
					int i;
					nNod=atoi(av[1]);
					nodVtHd=new YsShellVertexHandle [nNod+1];
					if(nodVtHd==NULL)
					{
						YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::LoadTri()");
					}
					for(i=0; i<nNod; i++)
					{
						nodVtHd[i]=NULL;
					}
				}
				else if(strcmp(av[0],"noftri")==0)
				{
					nTri=atoi(av[1]);
				}
				else if(strcmp(av[0],"nod")==0)
				{
					if(nodVtHd!=NULL)
					{
						int ndId;
						YsVec3 pos;
						ndId=atoi(av[1]);
						pos.Set(atof(av[2]),atof(av[3]),atof(av[4]));
						nodVtHd[ndId]=AddVertexH(pos);
					}
					else
					{
						YsErrOut("YsShell::LoadTri()\n");
						YsErrOut("  nofnod didn't come before nod\n");
						fclose(fp);
						goto ERREND;
					}
				}
				else if(strcmp(av[0],"tri")==0)
				{
					YsShellVertexHandle vtHd[3];
					vtHd[0]=nodVtHd[atoi(av[2])];
					vtHd[1]=nodVtHd[atoi(av[3])];
					vtHd[2]=nodVtHd[atoi(av[4])];
					if(vtHd[0]!=NULL && vtHd[1]!=NULL && vtHd[2]!=NULL)
					{
						AddPolygonH(3,vtHd);
					}
					else
					{
						YsErrOut("YsShell::LoadTri()\n");
						YsErrOut("  Nodes must be defined before triangles.\n");
						fclose(fp);
						goto ERREND;
					}
				}
			}
		}
	}
	fclose(fp);
	return YSOK;

ERREND:
	if(nodVtHd!=NULL)
	{
		delete [] nodVtHd;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YSRESULT YsShell::LoadMsh(const char fn[])
{
	CleanUp();
	return MergeMsh(fn);
}

YSRESULT YsShell::MergeMsh(const char fn[])
{
	FILE *fp;
	int nNod,nEl2;
	YsShellVertexHandle *nodVtHd;
	int ac;
	char buf[256],str[256],*av[32];

	nNod=0;
	nEl2=0;
	nodVtHd=NULL;

	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		while(fgets(str,256,fp)!=NULL)
		{
			int i;
			for(i=0; str[i]!=0; i++)
			{
				if(str[i]=='#')
				{
					str[i]=0;
					break;
				}
			}

			strcpy(buf,str);
			YsArguments(&ac,av,32,buf);
			if(ac>0)
			{
				if(strcmp(av[0],"nofnod")==0)
				{
					int i;
					nNod=atoi(av[1]);
					nodVtHd=new YsShellVertexHandle [nNod+1];
					if(nodVtHd==NULL)
					{
						YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::MergeMsh()");
					}
					for(i=0; i<nNod; i++)
					{
						nodVtHd[i]=NULL;
					}
				}
				else if(strcmp(av[0],"nofel2")==0)
				{
					nEl2=atoi(av[1]);
				}
				else if(strcmp(av[0],"nod")==0)
				{
					if(nodVtHd!=NULL)
					{
						int ndId;
						YsVec3 pos;
						ndId=atoi(av[1]);
						pos.Set(atof(av[2]),atof(av[3]),atof(av[4]));
						nodVtHd[ndId]=AddVertexH(pos);
					}
					else
					{
						YsErrOut("YsShell::LoadTri()\n");
						YsErrOut("  nofnod didn't come before nod\n");
						fclose(fp);
						goto ERREND;
					}
				}
				else if(strcmp(av[0],"el2")==0)
				{
					YsShellVertexHandle vtHd[4];
					if(atoi(av[2])==3)
					{
						vtHd[0]=nodVtHd[atoi(av[3])];
						vtHd[1]=nodVtHd[atoi(av[4])];
						vtHd[2]=nodVtHd[atoi(av[5])];
						if(vtHd[0]!=NULL && vtHd[1]!=NULL && vtHd[2]!=NULL)
						{
							AddPolygonH(3,vtHd);
						}
					}
					else if(atoi(av[2])==4)
					{
						vtHd[0]=nodVtHd[atoi(av[3])];
						vtHd[1]=nodVtHd[atoi(av[4])];
						vtHd[2]=nodVtHd[atoi(av[5])];
						vtHd[3]=nodVtHd[atoi(av[6])];
						if(vtHd[0]!=NULL && vtHd[1]!=NULL && vtHd[2]!=NULL && vtHd[3]!=NULL)
						{
							AddPolygonH(4,vtHd);
						}
					}
					else
					{
						YsErrOut("YsShell::LoadMsh()\n");
						YsErrOut("  Load Error\n");
						YsErrOut("  Refered to undefined node or non-tri&non-quad element\n");
						fclose(fp);
						goto ERREND;
					}
				}
			}
		}
	}
	fclose(fp);
	return YSOK;

ERREND:
	if(nodVtHd!=NULL)
	{
		delete [] nodVtHd;
	}
	return YSERR;
}

YSRESULT YsShell::LoadMs3(const char fn[])
{
	CleanUp();
	return MergeMs3(fn);
}

YSRESULT YsShell::MergeMs3(const char fn[])
{
	FILE *fp;
	YsArray <YsShellVertexHandle> vtHdList;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		char buf[256];
		YsShellVertexHandle plVtHd[4];
		while(fgets(buf,255,fp)!=NULL)
		{
			int ac;
			char *av[16];
			if(YsArguments(&ac,av,16,buf)==YSOK && ac>0)
			{
				if(av[0][0]=='v' || av[0][0]=='V')
				{
					if(ac>=4)
					{
						YsShellVertexHandle vtHd;
						YsVec3 pos;
						double x,y,z;
						x=atof(av[1]);
						y=atof(av[2]);
						z=atof(av[3]);
						pos.Set(x,y,z);
						vtHd=AddVertexH(pos);
						vtHdList.Append(vtHd);
					}
					else
					{
						goto ERREND;
					}
				}
				else if(av[0][0]=='D' || av[0][0]=='d')  // D:Delta=Triangle
				{
					if(ac>=4)
					{
						plVtHd[0]=vtHdList[atoi(av[1])];
						plVtHd[1]=vtHdList[atoi(av[2])];
						plVtHd[2]=vtHdList[atoi(av[3])];
						SetColorOfPolygon(AddPolygonH(3,plVtHd),YsBlue());
					}
					else
					{
						goto ERREND;
					}
				}
				else if(av[0][0]=='Q' || av[0][0]=='q')
				{
					if(ac>=5)
					{
						plVtHd[0]=vtHdList[atoi(av[1])];
						plVtHd[1]=vtHdList[atoi(av[2])];
						plVtHd[2]=vtHdList[atoi(av[3])];
						plVtHd[3]=vtHdList[atoi(av[4])];
						SetColorOfPolygon(AddPolygonH(4,plVtHd),YsBlue());
					}
					else
					{
						goto ERREND;
					}
				}
			}
		}
		fclose(fp);
		return YSOK;
	}

ERREND:
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return YSERR;
}

YSRESULT YsShell::LoadDat(const char fn[])
{
	CleanUp();
	return MergeDat(fn);
}

YSRESULT YsShell::MergeDat(const char fn[])
{
	FILE *fp;
	YsArray <YsShellVertexHandle> vtHdList;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		char buf[4096];
		YsArray <YsShellVertexHandle,64> plVtHd;
		while(fgets(buf,4095,fp)!=NULL)
		{
			int ac;
			char *av[1024];
			if(YsArguments(&ac,av,1024,buf)==YSOK && ac>0)
			{
				if(av[0][0]=='v' || av[0][0]=='V')
				{
					if(ac>=4)
					{
						YsShellVertexHandle vtHd;
						YsVec3 pos;
						double x,y,z;
						x=atof(av[1]);
						y=atof(av[2]);
						z=atof(av[3]);
						pos.Set(x,y,z);
						vtHd=AddVertexH(pos);
						vtHdList.Append(vtHd);
					}
					else
					{
						goto ERREND;
					}
				}
				else if(av[0][0]=='f' || av[0][0]=='F')  // D:Delta=Triangle
				{
					if(ac>=4)
					{
						int i;
						YsShellPolygonHandle plHd;
						plVtHd.Set(ac-1,NULL);
						for(i=0; i<ac-1; i++)
						{
							plVtHd[i]=vtHdList[atoi(av[i+1])];
						}
						plHd=AddPolygonH(ac-1,vtHdList);
						SetColorOfPolygon(plHd,YsBlue());
					}
					else
					{
						goto ERREND;
					}
				}
			}
		}
		fclose(fp);
		return YSOK;
	}

ERREND:
	if(fp!=NULL)
	{
		fclose(fp);
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YSRESULT YsShell::SaveSrf
   (const char fn[],
    int nNoShadingPolygon,const YsShellPolygonHandle noShadingPolygon[],
    int nRoundVtx,const YsShellVertexHandle roundVtx[]) const
{
	FILE *fp;
	YsShellVertexHandle vtHd,tstVtHd;
	YsShellPolygonHandle plHd,tstPlHd;

	YsHashTable <YsShellVertexHandle> roundVtxTable(nRoundVtx+1);
	YsHashTable <YsShellPolygonHandle> noShadingPolygonTable(nNoShadingPolygon+1);

	int i;
	if(noShadingPolygon!=NULL)
	{
		for(i=0; i<nNoShadingPolygon; i++)
		{
			noShadingPolygonTable.AddElement(GetSearchKey(noShadingPolygon[i]),noShadingPolygon[i]);
		}
	}
	if(roundVtx!=NULL)
	{
		for(i=0; i<nRoundVtx; i++)
		{
			roundVtxTable.AddElement(GetSearchKey(roundVtx[i]),roundVtx[i]);
		}
	}


	Encache();


	fp=fopen(fn,"w");

	if(fp!=NULL)
	{
		fprintf(fp,"SURF\n");

		vtHd=NULL;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsVec3 pos;
			GetVertexPosition(pos,vtHd);

			if(roundVtxTable.FindElement(tstVtHd,GetSearchKey(vtHd))!=YSOK)
			{
				fprintf(fp,"V %lf %lf %lf\n",pos.x(),pos.y(),pos.z());
			}
			else
			{
				fprintf(fp,"V %lf %lf %lf R\n",pos.x(),pos.y(),pos.z());
			}
		}

		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			int i,n;
			YsVec3 cen,nom,pos;
			const YsShellVertexHandle *plVtHd;
			n=GetNumVertexOfPolygon(plHd);
			plVtHd=GetVertexListOfPolygon(plHd);

			fprintf(fp,"F\n");

			if(noShadingPolygonTable.FindElement(tstPlHd,GetSearchKey(plHd))==YSOK)
			{
				fprintf(fp,"B\n");
			}

			cen.Set(0.0,0.0,0.0);
			for(i=0; i<n; i++)
			{
				if(i%8==0)
				{
					fprintf(fp,"V");
				}

				fprintf(fp," %d",GetVertexIdFromHandle(plVtHd[i]));

				if(i%8==7 || i==n-1)
				{
					fprintf(fp,"\n");
				}

				GetVertexPosition(pos,plVtHd[i]);
				cen+=pos;
			}

			cen/=double(n);
			GetNormalOfPolygon(nom,plHd);


			YsColor col;
//			unsigned int r,g,b;
			GetColorOfPolygon(col,plHd);
//			r=YsBound(int(col.Rd()*31.0),0,31);
//			g=YsBound(int(col.Gd()*31.0),0,31);
//			b=YsBound(int(col.Bd()*31.0),0,31);
//			fprintf(fp,"C %d\n",g*1024+r*32+b);
			fprintf(fp,"C %d\n",col.Get15BitColor());

			fprintf(fp,"N %lf %lf %lf %lf %lf %lf\n",cen.x(),cen.y(),cen.z(),nom.x(),nom.y(),nom.z());
			fprintf(fp,"E\n");
		}

		fprintf(fp,"E\n");
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SaveMsh(int &nIgnored,const char fn[]) const
{
	FILE *fp;
	YsShellVertexHandle vtHd;
	YsShellPolygonHandle plHd;

	Encache();
	nIgnored=0;

	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		int id,nofel2;

		fprintf(fp,"nofnod %d\n",GetNumVertex());

		id=1;
		vtHd=NULL;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsVec3 pos;
			GetVertexPosition(pos,vtHd);
			fprintf(fp,"nod %d %lf %lf %lf\n",id,pos.x(),pos.y(),pos.z());
			id++;
		}

		nofel2=0;
		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			if(GetNumVertexOfPolygon(plHd)==3 || GetNumVertexOfPolygon(plHd)==4)
			{
				nofel2++;
			}
			else
			{
				nIgnored++;
			}
		}
		fprintf(fp,"nofel2 %d\n",nofel2);

		id=1;
		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			int i,n;
			const YsShellVertexHandle *plVtHd;
			n=GetNumVertexOfPolygon(plHd);
			plVtHd=GetVertexListOfPolygon(plHd);

			if(n==3 || n==4)
			{
				fprintf(fp,"el2 %d %d",id,n);
				for(i=0; i<n; i++)
				{
					fprintf(fp," %d",GetVertexIdFromHandle(plVtHd[i])+1);
				}
				fprintf(fp,"\n");
				id++;
			}
		}

		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SaveMs3(int &nIgnored,const char fn[]) const
{
	FILE *fp;
	YsShellVertexHandle vtHd;
	YsShellPolygonHandle plHd;

	Encache();
	nIgnored=0;

	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		vtHd=NULL;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			YsVec3 pos;
			GetVertexPosition(pos,vtHd);
			fprintf(fp,"V %lf %lf %lf\n",pos.x(),pos.y(),pos.z());
		}

		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			int n;
			const YsShellVertexHandle *plVtHd;
			n=GetNumVertexOfPolygon(plHd);
			plVtHd=GetVertexListOfPolygon(plHd);

			if(n==3)
			{
				fprintf(fp,"D %d %d %d\n",   // D of Delta
				    GetVertexIdFromHandle(plVtHd[0]),
				    GetVertexIdFromHandle(plVtHd[1]),
				    GetVertexIdFromHandle(plVtHd[2]));
			}
			else if(n==4)
			{
				fprintf(fp,"Q %d %d %d %d\n",
				    GetVertexIdFromHandle(plVtHd[0]),
				    GetVertexIdFromHandle(plVtHd[1]),
				    GetVertexIdFromHandle(plVtHd[2]),
				    GetVertexIdFromHandle(plVtHd[3]));
			}
		}

		fclose(fp);
		return YSOK;
	}
	return YSERR;
}


////////////////////////////////////////////////////////////

YSBOOL YsCheckShellCollision(YsShell &sh1,YsShell &sh2)
{
	YSBOOL result;
	YsShellPolygonHandle plId1,plId2;
	int nv1,nv2;
	YsShellPolygon *pl1,*pl2;
	YsVec3 v1[256],nom1,v2[256],nom2;
	YsCollisionOfPolygon wizard;

	plId1=NULL;
	while((plId1=sh1.FindNextPolygon(plId1))!=NULL)
	{
		pl1=sh1.GetPolygon(plId1);
		pl1->freeAttribute3=0;
	}
	plId2=NULL;
	while((plId2=sh2.FindNextPolygon(plId2))!=NULL)
	{
		pl2=sh2.GetPolygon(plId2);
		pl2->freeAttribute3=0;
	}

	result=YSFALSE;

	plId1=NULL;
	while((plId1=sh1.FindNextPolygon(plId1))!=NULL)
	{
		nv1=sh1.GetNumVertexOfPolygon(plId1);
		sh1.GetVertexListOfPolygon(v1,256,plId1);
		pl1=sh1.GetPolygon(plId1);
		sh1.GetNormalOfPolygon(nom1,plId1);

		if(sh1.TrustPolygonNormal()==YSTRUE && nom1!=YsOrigin())
		{
			wizard.SetPolygon1(nv1,v1,nom1);
		}
		else
		{
			wizard.SetPolygon1(nv1,v1);
		}

		plId2=NULL;
		while((plId2=sh2.FindNextPolygon(plId2))!=NULL)
		{
			nv2=sh2.GetNumVertexOfPolygon(plId2);
			sh2.GetVertexListOfPolygon(v2,256,plId2);
			sh2.GetNormalOfPolygon(nom2,plId2);

			if(sh2.TrustPolygonNormal()==YSTRUE && nom2!=YsOrigin())
			{
				wizard.SetPolygon2(nv2,v2,nom2);
			}
			else
			{
				wizard.SetPolygon2(nv2,v2);
			}

			if(wizard.CheckCollision()==YSTRUE)
			{
				pl2=sh2.GetPolygon(plId2);
				pl1->freeAttribute3=1;
				pl2->freeAttribute3=1;
				result=YSTRUE;
			}
		}
	}

	return result;
}





//####
// Future modification
//   1. Just throw all vertices/polygons to one shell
//   2. Slash self-intersecting edges
//   3. Decompose polygons
// Advantage
//   Can guarantee winged-edge structure
// Disadvantage
//   Again how to take care of overlapping polygons?

// Another solution
//   Right after blending shell, before identical vertices are deleted,
//   Mark floating vertices and make tables which edge a floating vertex is from
//   Then, replace edges one by one refering to the table

// Maybe this will be the best solution
//   1. Dump all vertices/polygons of two shells into one shell. Keep track of which polygon is from which
//   2. Slash polygons that are from Polygon 1{2} by shell 2{1}
//      But, make use of YsSwordNewVertexLog.
//      Create new vertices exactly the same manner that is written in NewVertexLog
//      New vertex ids are initially negative. Update them when they are added to the shell.
//      (New function must be added in YsSword class. UpdateNewVertexId(int temporaryId,int Id);
//         Note1:Ids are obtained by YsShell::GetSearchKey() function
//         Note2:Update both IDs in NewVertexLog and VTIDs in YsSwordPolygon
//         Note3:If an edge betweenVt1-betweenVt2 of NewVertexLog doesn't exist, it means
//               the edge is interior of the slashed polygon. So, just add a floating vertex)
//      After steps above, all vertices in YsSword is in the shell.
//      Remove original polygon. Add new polygons in YsSword class. No need to add new vertex.
//   3. Overlapping Polygons : Take one of two from Shell 1 (or 2)

static YSRESULT YsBlendShell1to2
    (YsShell &newShell,
     const YsShell &sh1,const YsShell &sh2,int f,YSBOOL elimOverlap)
{
	int i,j,np1,np2,nv1,nv2;
	YsShellPolygonHandle plHd;
	const YsShellPolygon *pl1;
	YsShellPolygon *plNew;
	YsVec3 v1[256],v2[256];
	int vtId[256];
	YsCollisionOfPolygon wizard;
	YsSword sword;

	sh1.Encache();
	sh2.Encache();

	np1=sh1.GetNumPolygon();
	np2=sh2.GetNumPolygon();

	for(i=0; i<np1; i++)
	{
		nv1=sh1.GetNumVertexOfPolygon(i);
		sh1.GetVertexListOfPolygon(v1,256,i);
		sh1.GetVertexListOfPolygon(vtId,256,i);
		pl1=sh1.GetPolygon(i);
		wizard.SetPolygon1(nv1,v1);
		sword.SetInitialPolygon(nv1,v1,vtId);


		// Future expansion ####
		//   Use 3D lattice to speed up intersection


		for(j=0; j<np2; j++)
		{
			nv2=sh2.GetNumVertexOfPolygon(j);
			sh2.GetVertexListOfPolygon(v2,256,j);
			wizard.SetPolygon2(nv2,v2);

			if(wizard.CheckOnTheSamePlane()==YSTRUE)
			{
				sword.SlashByOverlappingPolygon3(nv2,v2,j);
			}
			else if(wizard.CheckCollision()==YSTRUE)
			{
				sword.Slash(nv2,v2);
			}
		}

		// Future expansion ####
		//   Refer to YsSwordNewVertexLog and YsShellEdgeList,
		//   Add vertices to edges of other polygons -> guarantee winged edge structure

		int nPiece;
		const YsArray <YsVec3> *plg;
		const YsArray <int> *vtId;

		nPiece=sword.GetNumPolygon();
		for(j=0; j<nPiece; j++)
		{
			int slashedPlId;
			if(sword.IsFromOverlappingPolygon3(j,slashedPlId)!=YSTRUE)
			{
				plg=sword.GetPolygon(j);
				vtId=sword.GetVertexIdList(j);
				plHd=newShell.AddPolygonVertex(plg->GetNumItem(),plg->GetArray(),f,vtId->GetArray());
				plNew=newShell.GetPolygon(plHd);
				plNew->SetColor(pl1->GetColor());
				plNew->SetNormal(pl1->GetNormal());
				plNew->freeAttribute2=f;
				plNew->freeAttribute3=i;
				plNew->freeAttribute4=0;
			}
			else if(elimOverlap!=YSTRUE)
			{
				plg=sword.GetPolygon(j);
				plHd=newShell.AddPolygonVertexH(plg->GetNumItem(),plg->GetArray());
				plNew=newShell.GetPolygon(plHd);
				plNew->SetColor(pl1->GetColor());
				plNew->SetNormal(pl1->GetNormal());
				plNew->freeAttribute2=2;   //2 means shared polygon
				plNew->freeAttribute3=i;           // PlId of Shell[0]
				plNew->freeAttribute4=slashedPlId; // PlId of Shell[1]
			}
		}
	}
	return YSOK;
}

YSBOOL YsShell::EliminateHangnail(int &nv,YsShellVertexHandle v[])
// Idea : If edges are going like  A-B,B-C,C-B,B-D, then edge B-C,C-B can be omitted.
{
	for(int i=0; i<nv; i++)
	{
		int j,k,l;
		j=(i+1)%nv;
		k=(i+2)%nv;
		if(v[i]==v[k])
		{
			if(j>k)
			{
				int a;
				a=j;
				j=k;
				k=a;
			}
			for(l=k; l<nv; l++)
			{
				v[l]=v[l+1];
			}
			nv--;
			for(l=j; l<nv; l++)
			{
				v[l]=v[l+1];
			}
			nv--;
			return YSTRUE;
		}
	}
	return YSFALSE;
}

unsigned YsShell::GetUniqueSearchKey(void)
{
	return uniqueSearchKey++;
}

void YsShell::RewindSearchKey(void)
{
	uniqueSearchKey=1;  // Leave 0 unused.
}

// Note: vtIdNew must not be equal to vtIdOrg
YSRESULT YsShell::BlendPolygonVtId
    (int &nvNew,YsShellVertexHandle vtIdNew[],int nBuf,
     int nv1,const YsShellVertexHandle vtId1[],int nv2,const YsShellVertexHandle vtId2[])
{
	if(nBuf<nv2+nv1)
	{
		return YSERR;
	}

	int i;
	YsShellVertexHandle vtOrg1,vtOrg2,vt1,vt2;

	for(i=0; i<nv1; i++)
	{
		vtOrg1=vtId1[i];
		vtOrg2=vtId1[(i+1)%nv1];
		for(int j=0; j<nv2; j++)
		{
			vt1=vtId2[j];
			vt2=vtId2[(j+1)%nv2];
			if(vt1==vtOrg1 && vt2==vtOrg2)
			{
				int n;

				n=0;
				for(int k=0; k<nv1; k++)
				{
					vtIdNew[n++]=vtId1[(i+1+k)%nv1];
				}
				for(int l=1; l<nv2-1; l++)
				{
					vtIdNew[n++]=vtId2[(j+nv2-l)%nv2];
				}
				nvNew=n;

				while(EliminateHangnail(nvNew,vtIdNew)==YSTRUE)
				{
				}

				return YSOK;
			}
			else if(vt1==vtOrg2 && vt2==vtOrg1)
			{
				int n;

				n=0;
				for(int k=0; k<nv1; k++)
				{
					vtIdNew[n++]=vtId1[(i+1+k)%nv1];
				}
				for(int l=1; l<nv2-1; l++)
				{
					vtIdNew[n++]=vtId2[(j+1+l)%nv2];
				}
				nvNew=n;

				while(EliminateHangnail(nvNew,vtIdNew)==YSTRUE)
				{
				}

				return YSOK;
			}
		}
	}
	return YSERR;
}


YSRESULT YsShell::OmitPolygonAfterBlend(void)
//Idea : If two polygons are originated from the identical polygon, no need to remain separated
{
	YsOneMemoryBlock <YsShellVertexHandle,8192> memBlock;
	int nVtxNew,nVtxOfShell;
	YsShellVertexHandle *vtxNew;
	int stopInifiniteLoop;
	YsShellPolygonHandle plHd1,plHd2;

	stopInifiniteLoop=256;

	nVtxOfShell=GetNumVertex();
	vtxNew=memBlock.GetMemoryBlock(nVtxOfShell);

REPEAT:

	plHd1=NULL;
	while((plHd1=FindNextPolygon(plHd1))!=NULL)
	{
		plHd2=plHd1;
		while((plHd2=FindNextPolygon(plHd2))!=NULL)
		{
			YsShellPolygon *plg1,*plg2;
			int nVtx1;
			const YsShellVertexHandle *vtx1;
			int nVtx2;
			const YsShellVertexHandle *vtx2;

			plg1=GetPolygon(plHd1);
			nVtx1=plg1->GetNumVertex();
			vtx1=plg1->GetVertexArray();

			plg2=GetPolygon(plHd2);
			nVtx2=plg2->GetNumVertex();
			vtx2=plg2->GetVertexArray();

			if(plg1!=NULL && plg2!=NULL &&
               plg1->freeAttribute2==plg2->freeAttribute2 &&
               plg1->freeAttribute3==plg2->freeAttribute3)
			{
				if(BlendPolygonVtId(nVtxNew,vtxNew,nVtxOfShell,nVtx1,vtx1,nVtx2,vtx2)==YSOK)
				{
					if(ModifyPolygon(plHd1,nVtxNew,vtxNew)==YSOK)
					{
						DeletePolygon(plHd2);
					}

					if(stopInifiniteLoop>0)
					{
						stopInifiniteLoop--;
						goto REPEAT;
					}
					else
					{
						goto FINISH;
					}
				}
			}
		}
	}

FINISH:

	return YSOK;
}

void YsShell::SetMatrix(const YsMatrix4x4 &m)
{
	mat=m;
	matrixIsSet=YSTRUE;
	bbxIsValid=YSFALSE;
}

YSRESULT YsShell::Triangulate
    (YsArray <YsShellPolygonHandle> &newPlHdList,YsShellPolygonHandle plHd,YSBOOL killOriginalPolygon)
{
	newPlHdList.Set(0,NULL);

	int i,np;
	np=GetNumVertexOfPolygon(plHd);
	if(np>3)
	{
		YsVec3 nom;
		YsColor col;
		YsSword swd;
		YsArray <YsVec3,256> plVtPos;
		YsArray <YsShellVertexHandle,256> plVtHd;
		YsArray <int,256> key;
		const YsArray <int> *vtKeyList;
		YsShellPolygonHandle newPlHd;

		plVtHd.Set(np,GetVertexListOfPolygon(plHd));
		plVtPos.Set(np,NULL);
		key.Set(np,NULL);

		GetNormalOfPolygon(nom,plHd);
		GetColorOfPolygon(col,plHd);

		if(GetVertexListOfPolygon(plVtPos,np,plHd)==YSOK)
		{
			for(i=np-1; i>0; i--)
			{
				if(plVtHd[i]==plVtHd[i-1])
				{
					plVtHd.Delete(i);
					plVtPos.Delete(i);
					np--;
				}
			}
			if(plVtHd.GetN()>=3 && plVtHd.GetTopItem()==plVtHd.GetEndItem())
			{
				plVtHd.Delete(np-1);
				plVtPos.Delete(np-1);
				np--;
			}

			if(np<3)
			{
				if(killOriginalPolygon==YSTRUE)
				{
					DeletePolygon(plHd);
				}
				return YSOK;
			}

			for(i=0; i<np; i++)
			{
				key[i]=i;  // GetSearchKey(plVtHd[i]);
			}
			swd.SetInitialPolygon(np,plVtPos,key);
			if(swd.Triangulate()==YSOK)
			{
				YSRESULT res=YSOK;
				for(i=0; i<swd.GetNumPolygon(); i++)
				{
					vtKeyList=swd.GetVertexIdList(i);
					if(vtKeyList->GetNumItem()!=3)
					{
						YsErrOut("YSRESULT YsShell::Triangulate()\n");
						YsErrOut("  Non-triangular polygon remains.\n");
						res=YSERR;
						break;
					}

					YsShellVertexHandle newPlVtHd[3];
					newPlVtHd[0]=plVtHd[(*vtKeyList)[0]]; // search->FindVertex(*this,vtKeyList->GetItem(0));
					newPlVtHd[1]=plVtHd[(*vtKeyList)[1]]; // search->FindVertex(*this,vtKeyList->GetItem(1));
					newPlVtHd[2]=plVtHd[(*vtKeyList)[2]]; // search->FindVertex(*this,vtKeyList->GetItem(2));
					newPlHd=AddPolygonH(3,newPlVtHd);
					SetColorOfPolygon(newPlHd,col);
					SetNormalOfPolygon(newPlHd,nom);
					GetPolygon(newPlHd)->CopyAttribute(*GetPolygon(plHd)); // 2004/01/12
					newPlHdList.AppendItem(newPlHd);
				}

				if(res==YSOK)
				{
					if(killOriginalPolygon==YSTRUE)
					{
						DeletePolygon(plHd);
					}
				}
				else
				{
					for(i=0; i<newPlHdList.GetNumItem(); i++)
					{
						DeletePolygon(newPlHdList[i]);
					}
				}

				return res;
			}
			else
			{
				YsErrOut("YSRESULT YsShell::Triangulate()\n");
				YsErrOut("  Triangulation failed.\n");
				return YSERR;
			}
		}
		else
		{
			YsErrOut("YSRESULT YsShell::Triangulate()\n");
			YsErrOut("  Cannot retrieve geometry of the polygon.\n");
			return YSERR;
		}

		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

// Added 2000/11/10
void YsShell::SetTrustPolygonNormal(YSBOOL trust)
{
	trustPolygonNormal=trust;
}

// Added 2000/11/10
YSBOOL YsShell::TrustPolygonNormal(void) const
{
	return trustPolygonNormal;
}

////////////////////////////////////////////////////////////

// Added 2001/03/21

YSRESULT YsShell::FreezeVertex(YsShellVertexHandle vtHd)
{
	YsShellVertex *v;
	v=GetVertex(vtHd);
	if(v!=NULL)
	{
		if(v->GetReferenceCount()>0)
		{
			YsErrOut("YsShell::FreezeVertex\n  Tried to delete a Vertex in use\n");
			return YSERR;
		}

		if(searchTable!=NULL)
		{
			searchTable->DeleteVertex(*this,vtHd);
		}

		YsListItem <YsShellVertex> *ptr;
		ptr=(YsListItem <YsShellVertex> *)vtHd;
		vtx.Transfer(ptr,frozenVtx);
		// 2006/02/07 Use vtHd->GetContainer() instead of belongTo v->belongTo=NULL;

		bbxIsValid=YSFALSE;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::FreezePolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *p;
	YsShellVertex *vtx;
	p=GetPolygon(plHd);

	if(p!=NULL)
	{
		int i,nVtx;

		nVtx=GetNumVertexOfPolygon(plHd);
		for(i=0; i<nVtx; i++)
		{
			vtx=GetVertex(p->GetVertexHandle(i));
			vtx->DecrementReferenceCount();
		}

		if(searchTable!=NULL)
		{
			searchTable->DeletePolygon(*this,plHd);
		}

		YsListItem <YsShellPolygon> *ptr;
		ptr=(YsListItem <YsShellPolygon> *)plHd;
		plg.Transfer(ptr,frozenPlg);
		// 2006/02/07 Use plHd->GetContainer() instead of belongTo p->belongTo=NULL;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::MeltVertex(YsShellVertexHandle vtHd)
{
	if(vtHd!=NULL)
	{
		YsListItem <YsShellVertex> *ptr;
		ptr=(YsListItem <YsShellVertex> *)vtHd;
		if(ptr->GetContainer()==&frozenVtx)
		{
			// 2006/02/07 Use vtHd->GetContainer() instead of belongTo. ptr->dat.belongTo=this;
			frozenVtx.Transfer(ptr,vtx);
			if(searchTable!=NULL)
			{
				searchTable->AddVertex(*this,ptr);
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsShell::MeltVertex()\n  Not a vertex of the shell.\n");
			return YSERR;
		}
	}
	else
	{
		YsErrOut("YsShell::MeltVertex()\n");
		YsErrOut("  NULL pointer is given.\n");
		return YSERR;
	}
}

YSRESULT YsShell::MeltPolygon(YsShellPolygonHandle plHd)
{
	if(plHd!=NULL)
	{
		YsListItem <YsShellPolygon> *ptr;
		ptr=(YsListItem <YsShellPolygon> *)plHd;
		if(ptr->GetContainer()==&frozenPlg)
		{
			// 2006/02/07 Use plHd->GetContainer() instead of belongTo. ptr->dat.belongTo=this;
			frozenPlg.Transfer(ptr,plg);

			int i;
			for(i=0; i<ptr->dat.GetNumVertex(); i++)
			{
				GetVertex(ptr->dat.GetVertexHandle(i))->IncrementReferenceCount();
			}

			if(searchTable!=NULL)
			{
				searchTable->AddPolygon(*this,ptr);
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsShell::MeltPolygon()\n  Not a vertex of the shell.\n");
			return YSERR;
		}
	}
	else
	{
		YsErrOut("YsShell::MeltPolygon()\n  NULL pointer is passed.\n");
		return YSERR;
	}
}

YSRESULT YsShell::DeleteFrozenVertex(YsShellVertexHandle vtHd)
{
	if(vtHd!=NULL)
	{
		YsListItem <YsShellVertex> *ptr;
		ptr=(YsListItem <YsShellVertex> *)vtHd;
		if(ptr->GetContainer()==&frozenVtx)
		{
			frozenVtx.Delete(ptr);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell::DeleteFrozenPolygon(YsShellPolygonHandle plHd)
{
	if(plHd!=NULL)
	{
		YsListItem <YsShellPolygon> *ptr;
		ptr=(YsListItem <YsShellPolygon> *)plHd;
		if(ptr->GetContainer()==&frozenPlg)
		{
			frozenPlg.Delete(ptr);
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell::DisposeFrozenPolygon(void)
{
	YsListItem <YsShellPolygon> *seek;
	seek=NULL;
	while((seek=frozenPlg.FindNext(seek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingPolygon(seek->dat);
	}
	frozenPlg.CleanUp();
	return YSOK;
}

YSRESULT YsShell::DisposeFrozenVertex(void)
{
	YsListItem <YsShellVertex> *seek;
	seek=NULL;
	while((seek=frozenVtx.FindNext(seek))!=NULL)
	{
		DetachSpecialAttributeBeforeDeletingVertex(seek->dat);
	}
	frozenVtx.CleanUp();
	return YSOK;
}

////////////////////////////////////////////////////////////

// Added 2000/11/20

YsShellSpecialAttribute *YsShell::GetSpecialAttributeVertex(YsShellVertexHandle vtHd)
{
	YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

const YsShellSpecialAttribute *YsShell::GetSpecialAttributeVertex(YsShellVertexHandle vtHd) const
{
	const YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		return vtx->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

YsShellSpecialAttribute *YsShell::GetSpecialAttributePolygon(YsShellPolygonHandle plHd)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

const YsShellSpecialAttribute *YsShell::GetSpecialAttributePolygon(YsShellPolygonHandle plHd) const
{
	const YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		return plg->GetSpecialAttribute();
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShell::SetSpecialAttributeVertex(YsShellVertexHandle vtHd,YsShellSpecialAttribute *attr)
{
	YsShellVertex *vtx;
	vtx=GetVertex(vtHd);
	if(vtx!=NULL)
	{
		vtx->SetSpecialAttribute(attr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SetSpecialAttributePolygon(YsShellPolygonHandle plHd,YsShellSpecialAttribute *attr)
{
	YsShellPolygon *plg;
	plg=GetPolygon(plHd);
	if(plg!=NULL)
	{
		plg->SetSpecialAttribute(attr);
		return YSOK;
	}
	return YSERR;
}



YSRESULT YsShell::AttachSpecialAttributeToNewVertex(YsShellVertex &vtx,YsShellVertexHandle vtHd)
{
	// Sub-class must create a new special attribute
	// and attach to vtx if necessary.
	return YSOK;
}

YSRESULT YsShell::AttachSpecialAttributeToNewPolygon(YsShellPolygon &plg,YsShellPolygonHandle plHd)
{
	// Sub-class must create a new special attribute
	// and attach to plg if necessary.
	return YSOK;
}

YSRESULT YsShell::DetachSpecialAttributeBeforeDeletingVertex(YsShellVertex &vtx)
{
	// Sub-class must get special attribute from vtx,
	// delete and call vtx.SpecialAttributeDetached();
	return YSOK;
}

YSRESULT YsShell::DetachSpecialAttributeBeforeDeletingPolygon(YsShellPolygon &plg)
{
	// Sub-class must get special attribute from plg,
	// delete and call vtx.SpecialAttributeDetached();
	return YSOK;
}


////////////////////////////////////////////////////////////

YSRESULT YsShell::BuildOctree(YsShellOctree &oct,int maxDepth,double bbxMargine) const
{
	YsOneMemoryBlock <YsVec3,64> memBlk;
	int nPlVt,maxNumPlVt;
	YsVec3 *plVt;
	int nPl,plId;
	YsLoopCounter ctr;

	Encache();

	YsVec3 max,min,cen;
	GetBoundingBox(max,min);

	cen=(min+max)/2.0;
	min=cen+(min-cen)*bbxMargine;
	max=cen+(max-cen)*bbxMargine;

	oct.CleanUp();
	oct.shl=this;
	oct.SetDomain(max,min,YSUNKNOWNSIDE);

	maxNumPlVt=GetMaxNumVertexOfPolygon();
	plVt=memBlk.GetMemoryBlock(maxNumPlVt);

	YsPrintf("Building Octree\n");
	YsPrintf("  Marking boundary nodes\n");

	nPl=GetNumPolygon();
	ctr.BeginCounter(nPl);
	for(plId=0; plId<nPl; plId++)
	{
		ctr.Increment();

		nPlVt=GetNumVertexOfPolygon(plId);
		if(nPlVt>0 && GetVertexListOfPolygon(plVt,maxNumPlVt,plId)==YSOK)
		{
			oct.SubdivideByPolygon(nPlVt,plVt,maxDepth,YSBOUNDARY,YSUNKNOWNSIDE);
		}
	}
	ctr.EndCounter();

	YsPrintf("  Marking Outside/Inside\n");
	oct.GoTraverse(YSTRUE,-1,0);  // Parameter param1==-1 means mark outside/inside
	YsPrintf("  Done.\n");

	return YSOK;
}

YSBOOL YsShell::IsStarShape(const YsVec3 &pt) const
{
	// 1.The ray from pt to each vertex must not intersect with other polygons except at end points
	// 2.All triangles formed by pt and each edge must not intersect with another edge except at end points

	YsArray <int> freeAttrib4Save;

	YsShellEdgeList defEdgeList;
	const YsShellEdgeList *edgeList;

	SaveFreeAttribute4OfVertex(freeAttrib4Save);

	// if(supplyEdgeList!=NULL)
	// {
	// 	edgeList=supplyEdgeList;
	// }
	// else
	{
		defEdgeList.Build(*this);
		edgeList=&defEdgeList;
	}

	int i,j;
	int nEdg,edId;
	YsShellVertexHandle vtHd,edVtHd[2];
	YsVec3 tri[3],edg[2],intersection;
	YsPlane pln;
	YsArray <YsVec3> intersectionList;

	nEdg=edgeList->GetNumEdge();

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		YsShellVertex *vtx;
		vtx=(YsShellVertex *)GetVertex(vtHd);
		vtx->freeAttribute4=0;
	}

	for(edId=0; edId<nEdg; edId++)
	{
		if(edgeList->GetEdge(edVtHd[0],edVtHd[1],edId)==YSOK)
		{
			for(i=0; i<2; i++)
			{
				YsShellVertex *vtx;
				GetVertexPosition(edg[i],edVtHd[i]);
				vtx=(YsShellVertex *)GetVertex(edVtHd[i]);
				if(vtx->freeAttribute4==0)
				{
					vtx->freeAttribute4=1;
					ShootRay(intersectionList,pt,edg[i]-pt);
					for(j=0; j<intersectionList.GetNumItem(); j++)
					{
						if(intersectionList.GetItem(j)!=pt && intersectionList.GetItem(j)!=edg[i])
						{
							RestoreFreeAttribute4OfVertex(freeAttrib4Save);
							return YSFALSE;
						}
					}
				}

			}

			tri[0]=edg[0];
			tri[1]=edg[1];
			tri[2]=pt;
			pln.MakePlaneFromTriangle(tri[0],tri[1],tri[2]);
			for(i=0; i<nEdg; i++)
			{
				edgeList->GetEdge(edVtHd[0],edVtHd[1],i);
				GetVertexPosition(edg[0],edVtHd[0]);
				GetVertexPosition(edg[1],edVtHd[1]);
				if(pln.GetIntersection(intersection,edg[0],edg[1]-edg[0])==YSOK &&
				   YsCheckInBetween3(intersection,edg[0],edg[1])==YSTRUE &&
				   YsCheckInsidePolygon3(intersection,3,tri)!=YSOUTSIDE &&
				   tri[0]!=intersection &&
				   tri[1]!=intersection &&
				   tri[2]!=intersection)
				{
					RestoreFreeAttribute4OfVertex(freeAttrib4Save);
					return YSFALSE;
				}
			}
		}
		else
		{
			YsErrOut("YsShell::IsStarShape()\n");
			YsErrOut("  Conflict in the edgelist\n");
		}
	}

	RestoreFreeAttribute4OfVertex(freeAttrib4Save);
	return YSTRUE;
}

YSRESULT YsShell::AutoComputeNormal(YsShellPolygonHandle plHd,YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal)
{
	int nVtx;
	YsVec3 oldNom;
	YsArray <YsVec3,16> vtxList;

	if(plHd!=NULL && (nVtx=GetNumVertexOfPolygon(plHd))>=3)
	{
		vtxList.Set(nVtx,NULL);
		GetVertexListOfPolygon(vtxList.GetEditableArray(),nVtx,plHd);

		GetNormalOfPolygon(oldNom,plHd);
		if(oldNom==YsOrigin() || replaceCurrentNormal==YSTRUE)
		{
			YsVec3 nom;
			if(YsGetAverageNormalVector(nom,nVtx,vtxList.GetArray())==YSOK)
			{
				if(nom.IsNormalized()!=YSTRUE)
				{
					nom.Normalize();
				}

				if(ignoreCurrentNormal!=YSTRUE && oldNom!=YsOrigin() && nom*oldNom<0.0)
				{
					nom=-nom;
				}

				SetNormalOfPolygon(plHd,nom);
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell::AutoComputeNormalAll(YSBOOL replaceCurrentNormal,YSBOOL ignoreCurrentNormal)
{
	YSRESULT res;
	YsShellPolygonHandle plHd;

	res=YSOK;
	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		if(AutoComputeNormal(plHd,replaceCurrentNormal,ignoreCurrentNormal)!=YSOK)
		{
			res=YSERR;
		}
	}
	return res;
}

YSRESULT YsShell::AutoComputeVertexNormalAll(YSBOOL weighted)
{
#ifndef YSLOWMEM
	YsShellVertexHandle vtHd;
	int i,nPlVt;
	const YsShellVertexHandle *plVtHd;
	YsShellPolygonHandle plHd;
	YsVec3 plNom,vtNom;
	YsVec3 tri[3],v[2];  // for weighted==YSTRUE

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		GetVertex(vtHd)->SetNormal(YsOrigin());
	}

	plHd=NULL;
	while((plHd=FindNextPolygon(plHd))!=NULL)
	{
		GetVertexListOfPolygon(nPlVt,plVtHd,plHd);

		GetNormalOfPolygon(plNom,plHd);
		for(i=0; i<nPlVt; i++)
		{
			vtNom=GetVertex(plVtHd[i])->GetNormal();
			if(weighted==YSTRUE)
			{
				GetVertexPosition(tri[0],plVtHd[(i+nPlVt-1)%nPlVt]);
				GetVertexPosition(tri[1],plVtHd[i]);
				GetVertexPosition(tri[2],plVtHd[(i+1)%nPlVt]);
				v[0]=tri[1]-tri[0];
				v[1]=tri[2]-tri[1];
				if(v[0].Normalize()==YSOK && v[1].Normalize()==YSOK)
				{
					vtNom+=acos(-v[0]*v[1])*plNom;
				}
			}
			else
			{
				vtNom+=plNom;
			}
			GetVertex(plVtHd[i])->SetNormal(vtNom);
		}
	}

	vtHd=NULL;
	while((vtHd=FindNextVertex(vtHd))!=NULL)
	{
		vtNom=GetVertex(vtHd)->GetNormal();
		vtNom.Normalize();
		GetVertex(vtHd)->SetNormal(vtNom);
	}

	return YSOK;
#else
	return YSERR;
#endif
}

void YsShell::CleanUpVertexPolygonStore(void)
{
	vertexStore.CollectGarbage();
	polygonStore.CollectGarbage();
}

YsListItem <YsShellPolygon> *YsShell::CreatePolygon(void)
{
	YsListItem <YsShellPolygon> *plHd;
	plHd=plg.Create();
	plHd->dat.Initialize();
	plHd->dat.searchKey=GetUniqueSearchKey();
	// 2006/02/07 Use vtHd->GetContainer() instead of belongTo plHd->dat.belongTo=this;
	AttachSpecialAttributeToNewPolygon(plHd->dat,plHd);
	return plHd;
}

YsListItem <YsShellVertex> *YsShell::CreateVertex(void)
{
	YsListItem <YsShellVertex> *vtHd;
	vtHd=vtx.Create();
	vtHd->dat.Initialize();
	vtHd->dat.searchKey=GetUniqueSearchKey();
	// 2006/02/07 Use plHd->GetContainer() instead of belongTo vtHd->dat.belongTo=this;
	AttachSpecialAttributeToNewVertex(vtHd->dat,vtHd);
	return vtHd;
}

////////////////////////////////////////////////////////////


YSRESULT YsShell3dTree::Build(const class YsShell &shl)
{
	int i,a,nVtx;
	YsArray <YsShellVertexHandle> vtHdList;
	YsShellVertexHandle vtHd;
	YsLoopCounter ctr;

	nVtx=shl.GetNumVertex();
	if(nVtx>0)
	{
		PreAllocateNode(nVtx);

		vtHdList.Set(nVtx,NULL);

		i=0;
		vtHd=NULL;
		while((vtHd=shl.FindNextVertex(vtHd))!=NULL)
		{
			vtHdList.SetItem(i,vtHd);
			i++;
		}

		srand((int)time(NULL));

		YsPrintf("Building KDTree\n");
		YsPrintf("  Shuffling\n");
		ctr.BeginCounter(nVtx);
		for(i=0; i<nVtx; i++)
		{
			ctr.Increment();

			YsShellVertexHandle vtHd1,vtHd2;

			a=rand()%nVtx;
			vtHd1=vtHdList.GetItem(i);
			vtHd2=vtHdList.GetItem(a);

			vtHdList.SetItem(i,vtHd2);
			vtHdList.SetItem(a,vtHd1);
		}
		ctr.EndCounter();

		YsPrintf("  Adding Node Points\n");
		ctr.BeginCounter(nVtx);
		for(i=0; i<nVtx; i++)
		{
			ctr.Increment();

			YsVec3 pos;

			vtHd=vtHdList.GetItem(i);
			shl.GetVertexPosition(pos,vtHd);
			AddPoint(pos,vtHd);
		}
		ctr.EndCounter();
		YsPrintf("  Done\n");
	}

	return YSOK;
}

YSRESULT YsShell3dTree::AddVertex(const class YsShell &shl,YsShellVertexHandle vtHd)
{
	YsVec3 pos;
	shl.GetVertexPosition(pos,vtHd);
	return AddPoint(pos,vtHd);
}

YSRESULT YsShell3dTree::MakeVertexList(YsArray <YsShellVertexHandle,128> &lst,const YsVec3 &min,const YsVec3 &max) const
{
	lst.Set(0,NULL);
	vtxList=&lst;
	return Traverse(min,max);
}

YSRESULT YsShell3dTree::Enumerate(const YsVec3 &pos,const YsShellVertexHandle &attrib) const
{
	vtxList->AppendItem(attrib);
	return YSOK;
}


////////////////////////////////////////////////////////////

YsShellEdgeList::YsShellEdgeList()
{
	nEdge=0;
	edgeList=NULL;
	shell=NULL;
}

YsShellEdgeList::YsShellEdgeList(const YsShellEdgeList &from)
{
	*this=from;

	nEdge=from.nEdge;
	int i;
	for(i=0; i<8; i++)
	{
		edgeHash[i]=from.edgeHash[i];
	}
	edgeList=from.edgeList->Duplicate();
}

YsShellEdgeList::~YsShellEdgeList()
{
	if(edgeList!=NULL)
	{
		edgeList->DeleteList();
	}
}

int YsShellEdgeList::GetNumEdge(void) const
{
	return nEdge;
}

YSRESULT YsShellEdgeList::CleanUp(void)
{
	int i;

	for(i=0; i<HashTableSize; i++)
	{
		edgeHash[i].Set(0,NULL);
	}

	edgeList->DeleteList();

	nEdge=0;
	edgeList=NULL;
	shell=NULL;

	return YSOK;
}

YSRESULT YsShellEdgeList::Build(const YsShell &shl)
{
	YsShellPolygonHandle plHd;
	int nPlVt;
	const YsShellVertexHandle *plVtHd;

	shell=&shl;

	plHd=NULL;
	while((plHd=shl.FindNextPolygon(plHd))!=NULL)
	{
		nPlVt=shl.GetNumVertexOfPolygon(plHd);
		plVtHd=shl.GetVertexListOfPolygon(plHd);
		if(nPlVt>0 && plVtHd!=NULL)
		{
			int i;
			for(i=0; i<nPlVt; i++)
			{
				YsShellVertexHandle edVt1,edVt2;
				YsShellEdge *edge;

				edVt1=plVtHd[i];
				edVt2=plVtHd[(i+1)%nPlVt];
				if(LookUpEdge(edge,edVt1,edVt2,YSTRUE)==YSOK)
				{
					edge->usedBy.AppendItem(plHd);
				}
				else
				{
					if(AddEdge(edVt1,edVt2,plHd,shl)!=YSOK)
					{
						YsErrOut("YsShellEdgeList::Build()\n");
						YsErrOut("  Failed to add a new edge\n");
					}
				}
			}
		}
	}
	edgeList->Encache();
	return YSOK;
}

YSRESULT YsShellEdgeList::GetEdge(YsShellVertexHandle &edVt1,YsShellVertexHandle &edVt2,int edId) const
{
	YsList <YsShellEdge> *edge;
	edge=edgeList->Seek(edId);
	if(edge!=NULL)
	{
		edVt1=edge->dat.edVt1;
		edVt2=edge->dat.edVt2;
		return YSOK;
	}
	else
	{
		YsErrOut("YsShellEdgeList::GetEdge()\n");
		YsErrOut("  Edge not found\n");
		return YSERR;
	}
}

YSRESULT YsShellEdgeList::LookUpEdge
    (int &edId,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YSBOOL takeReverse) const
{
	YsShellEdge *edge;
	if(LookUpEdge(edge,edVt1,edVt2,takeReverse)==YSOK)
	{
		edId=edge->edId;
		return YSOK;
	}
	else
	{
		edId=-1;
		return YSERR;
	}
}

YSRESULT YsShellEdgeList::LookUpEdge
    (YsShellEdge *&edge,YsShellVertexHandle edVt1,YsShellVertexHandle edVt2, YSBOOL takeReverse) const
{
	int i;
	unsigned hashId,edVtKey1,edVtKey2;

	edVtKey1=shell->GetSearchKey(edVt1);
	edVtKey2=shell->GetSearchKey(edVt2);

	hashId=(edVtKey1+edVtKey2)%HashTableSize;
	for(i=0; i<edgeHash[hashId].GetNumItem(); i++)
	{
		YsShellEdge *candidate;
		unsigned canEdVtKey1,canEdVtKey2;

		candidate=edgeHash[hashId].GetItem(i);
		canEdVtKey1=shell->GetSearchKey(candidate->edVt1);
		canEdVtKey2=shell->GetSearchKey(candidate->edVt2);

		if(canEdVtKey1==edVtKey1 && canEdVtKey2==edVtKey2)
		{
			edge=candidate;
			return YSOK;
		}
		else if(takeReverse==YSTRUE && canEdVtKey1==edVtKey2 && canEdVtKey2==edVtKey1)
		{
			edge=candidate;
			return YSOK;
		}
	}

	edge=NULL;
	return YSERR;
}

const YsShellEdge *YsShellEdgeList::GetEdge(int edId) const
{
	YsList <YsShellEdge> *seeker;
	seeker=edgeList->Seek(edId);
	if(seeker!=NULL)
	{
		return &seeker->dat;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShellEdgeList::AddEdge
    (YsShellVertexHandle edVt1,YsShellVertexHandle edVt2,YsShellPolygonHandle plId,const YsShell &shl)
{
	int edId;
	unsigned edVtKey1,edVtKey2,hashId;
	YsList <YsShellEdge> *edge;

	edge=new YsList <YsShellEdge>;

	if(edge!=NULL)
	{
		edVtKey1=shl.GetSearchKey(edVt1);
		edVtKey2=shl.GetSearchKey(edVt2);

		edId=nEdge;
		hashId=(edVtKey1+edVtKey2)%HashTableSize;

		edge->dat.edId=edId;
		edge->dat.edVt1=edVt1;
		edge->dat.edVt2=edVt2;
		edge->dat.usedBy.AppendItem(plId);
		edgeHash[hashId].AppendItem(&edge->dat);

		nEdge++;

		edgeList=edgeList->Append(edge);

		return YSOK;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell::AddEdge()");
		return YSERR;
	}
}


////////////////////////////////////////////////////////////

YsShellVertexStore::YsShellVertexStore(const YsShell &shl)
{
	this->shl=&shl;
	EnableAutoResizing();
}

const YsShell &YsShellVertexStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellVertexStore::AddVertex(YsShellVertexHandle vtHd)
{
	return AddKey(shl->GetSearchKey(vtHd));
}

YSRESULT YsShellVertexStore::AddVertex(int nVt,const YsShellVertexHandle vtHd[])
{
	int i;
	for(i=0; i<nVt; i++)
	{
		AddKey(shl->GetSearchKey(vtHd[i]));
	}
	return YSOK;
}

YSRESULT YsShellVertexStore::DeleteVertex(YsShellVertexHandle vtHd)
{
	return DeleteKey(shl->GetSearchKey(vtHd));
}

YSBOOL YsShellVertexStore::IsIncluded(YsShellVertexHandle vtHd) const
{
	return YsKeyStore::IsIncluded(shl->GetSearchKey(vtHd));
}

////////////////////////////////////////////////////////////

YsShellPolygonStore::YsShellPolygonStore(const YsShell &shl)
{
	this->shl=&shl;
	EnableAutoResizing();
}

const YsShell &YsShellPolygonStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellPolygonStore::AddPolygon(YsShellPolygonHandle plHd)
{
	return AddKey(shl->GetSearchKey(plHd));
}

YSRESULT YsShellPolygonStore::AddPolygon(int nPl,const YsShellPolygonHandle plHd[])
{
	int i;
	for(i=0; i<nPl; i++)
	{
		AddKey(shl->GetSearchKey(plHd[i]));
	}
	return YSOK;
}

YSRESULT YsShellPolygonStore::DeletePolygon(YsShellPolygonHandle plHd)
{
	return DeleteKey(shl->GetSearchKey(plHd));
}

YSBOOL YsShellPolygonStore::IsIncluded(YsShellPolygonHandle plHd) const
{
	return YsKeyStore::IsIncluded(shl->GetSearchKey(plHd));
}

////////////////////////////////////////////////////////////

YsShellEdgeStore::YsShellEdgeStore(const YsShell &shl)
{
	this->shl=&shl;
	EnableAutoResizing();
}

const YsShell &YsShellEdgeStore::GetShell(void) const
{
	return *shl;
}

YSRESULT YsShellEdgeStore::AddEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2)
{
	unsigned int key1,key2;
	key1=shl->GetSearchKey(edVtHd1);
	key2=shl->GetSearchKey(edVtHd2);
	return AddKey(key1,key2);
}

YSRESULT YsShellEdgeStore::AddEdge(const YsShellVertexHandle edVtHd[2])
{
	unsigned int key1,key2;
	key1=shl->GetSearchKey(edVtHd[0]);
	key2=shl->GetSearchKey(edVtHd[1]);
	return AddKey(key1,key2);
}

YSRESULT YsShellEdgeStore::DeleteEdge(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2)
{
	unsigned int key1,key2;
	key1=shl->GetSearchKey(edVtHd1);
	key2=shl->GetSearchKey(edVtHd2);
	return DeleteKey(key1,key2);
}

YSRESULT YsShellEdgeStore::DeleteEdge(const YsShellVertexHandle edVtHd[2])
{
	unsigned int key1,key2;
	key1=shl->GetSearchKey(edVtHd[0]);
	key2=shl->GetSearchKey(edVtHd[1]);
	return DeleteKey(key1,key2);
}

YSBOOL YsShellEdgeStore::IsIncluded(YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	unsigned int key1,key2;
	key1=shl->GetSearchKey(edVtHd1);
	key2=shl->GetSearchKey(edVtHd2);
	return YsTwoKeyStore::IsIncluded(key1,key2);
}

YSBOOL YsShellEdgeStore::IsIncluded(const YsShellVertexHandle edVtHd[2])
{
	unsigned int key1,key2;
	key1=shl->GetSearchKey(edVtHd[0]);
	key2=shl->GetSearchKey(edVtHd[1]);
	return YsTwoKeyStore::IsIncluded(key1,key2);
}

////////////////////////////////////////////////////////////

static YSBOOL YsSharedPolygonIsBoundary
    (YsShell &newShell,int sharedPlId,
     const YsShell &sh1,int fromPlId1,
     const YsShell &sh2,int fromPlId2)
{
	int nVtxOfPlg,crsCtr1,crsCtr2;
	YsOneMemoryBlock <YsVec3,256> mem;
	YsVec3 *vtx,nom1,nom2;
	YsVec3 lat,lng,inside,ray;
	YSSIDE side1,side2;

	double factor[9][2]=
	{
		{ 0.0, 0.0},
		{ 1.0, 0.0},
		{ 1.0, 1.0},
		{ 0.0, 1.0},
		{-1.0, 1.0},
		{-1.0, 0.0},
		{-1.0,-1.0},
		{ 0.0,-1.0},
		{ 1.0,-1.0}
	};

	nVtxOfPlg=newShell.GetNumVertexOfPolygon(sharedPlId);
	vtx=mem.GetMemoryBlock(nVtxOfPlg);

	if(vtx!=NULL)
	{
		int k;

		newShell.GetVertexListOfPolygon(vtx,nVtxOfPlg,sharedPlId);
		YsGetAverageNormalVector(nom1,nVtxOfPlg,vtx);
		nom1.Normalize();
		nom2=-1.0*nom1;

		YsGetArbitraryInsidePointOfPolygon3(inside,nVtxOfPlg,vtx);

		lat=nom1.GetArbitraryPerpendicularVector();
		lng=nom2^lat;

		lat.Normalize();
		lng.Normalize();

		for(k=0; k<9; k++)
		{
			ray=nom1+lat*factor[k][0]+lng*factor[k][1];

			side1=sh1.CountRayIntersection(crsCtr1,inside,ray,fromPlId1);
			side2=sh2.CountRayIntersection(crsCtr2,inside,ray,fromPlId2);

			if(side1!=YSBOUNDARY && side1!=YSUNKNOWNSIDE &&
			   side2!=YSBOUNDARY && side2!=YSUNKNOWNSIDE)
			{
				if((crsCtr1%2)==(crsCtr2%2))
				{
					return YSTRUE;
				}
				else
				{
					return YSFALSE;
				}
			}
		}
	}

	YsErrOut("YsPolygonIsInside()@ysshell.cpp\n  Overlapping polygon may exists.\n");
	return YSTRUE;
}

YSRESULT YsBlendAnd
    (YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);

		if((plg->freeAttribute2==0 && sh2.CheckInsideSolid(cen)!=YSINSIDE)||
		   (plg->freeAttribute2==1 && sh1.CheckInsideSolid(cen)!=YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary
			       (newShell,i,
			        sh1,plg->freeAttribute3,
			        sh2,plg->freeAttribute4)!=YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
	}
	newShell.DeleteVertexAtTheSamePosition();
	newShell.OmitPolygonAfterBlend();

	return YSOK;
}





YSRESULT YsBlendOr
    (YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);
		if((plg->freeAttribute2==0 && sh2.CheckInsideSolid(cen)==YSINSIDE)||
		   (plg->freeAttribute2==1 && sh1.CheckInsideSolid(cen)==YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary
			       (newShell,i,
			        sh1,plg->freeAttribute3,
			        sh2,plg->freeAttribute4)!=YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
	}

	newShell.DeleteVertexAtTheSamePosition();

	printf("####newShell.OmitPolygonAfterBlend() is temporarily disabled in YsBlendOr()@ysshell.cpp\n");
	//newShell.OmitPolygonAfterBlend();

	return YSOK;
}

YSRESULT YsBlendMinus
    (YsShell &newShell,const YsShell &sh1,const YsShell &sh2)
{
	int i;
	int nVtx;
	YsVec3 vtx[256],cen;

	newShell.Encache();

	for(i=newShell.GetNumPolygon()-1; i>=0; i--)
	{
		YsShellPolygon *plg;
		plg=newShell.GetPolygon(i);
		nVtx=newShell.GetNumVertexOfPolygon(i);
		newShell.GetVertexListOfPolygon(vtx,256,i);

		YsGetArbitraryInsidePointOfPolygon3(cen,nVtx,vtx);
		if((plg->freeAttribute2==0 && sh2.CheckInsideSolid(cen)==YSINSIDE)||
		   (plg->freeAttribute2==1 && sh1.CheckInsideSolid(cen)!=YSINSIDE))
		{
			newShell.DeletePolygon(i);
		}
		else if(plg->freeAttribute2==2)
		{
			if(YsSharedPolygonIsBoundary
			       (newShell,i,
			        sh1,plg->freeAttribute3,
			        sh2,plg->freeAttribute4)==YSTRUE)
			{
				newShell.DeletePolygon(i);
			}
		}
		else if(plg->freeAttribute2==1)
		{
			newShell.InvertPolygon(i);
		}
	}

	newShell.DeleteVertexAtTheSamePosition();
	newShell.OmitPolygonAfterBlend();

	return YSOK;
}

////////////////////////////////////////////////////////////

extern YSRESULT YsMixShell(YsShell &newShell,const YsShell &sh1,const YsShell &sh2);

YSRESULT YsBlendShell
   (YsShell &newShell,const YsShell &sh1,const YsShell &sh2,
    YSBOOLEANOPERATION boolop)
    // Note:This function will use freeAttribute2 and 3.
{
	newShell.CleanUp();
	sh1.Encache();
	sh2.Encache();

	if(YsMixShell(newShell,sh1,sh2)==YSOK)
	{
		switch(boolop)
		{
		case YSBOOLBLEND:
			// It's done.
			return YSOK;
		case YSBOOLAND:
			return YsBlendAnd(newShell,sh1,sh2);
		case YSBOOLOR:
			return YsBlendOr(newShell,sh1,sh2);
		case YSBOOLMINUS:
			return YsBlendMinus(newShell,sh1,sh2);
		}
		return YSOK;
	}

	return YSERR;
}


YSRESULT YsSeparateShell
    (YsShell &sh1,YsShell &sh2,const YsShell &org,const YsShell &ref)
{
	sh1.CleanUp();
	sh2.CleanUp();

	if(YsBlendShell1to2(sh1,org,ref,0,YSFALSE)==YSOK &&
	   YsBlendShell1to2(sh1,ref,org,1,YSTRUE)==YSOK)
	{
		sh2=sh1;
		if(YsBlendMinus(sh1,org,ref)==YSOK &&
		   YsBlendAnd(sh2,org,ref)==YSOK)
		{
			return YSOK;
		}
	}
	return YSERR;
}


////////////////////////////////////////////////////////////


YSRESULT YsReplaceShellEdge
    (YsShell &shl,YsShellVertexHandle vtHd1,YsShellVertexHandle vtHd2,int nNewEdVt,const YsShellVertexHandle newEdVt[])
{
	const YsShellSearchTable *search;
	search=shl.GetSearchTable();
	if(search==NULL)
	{
		YsErrOut("YsReplaceShellEdge()\n");
		YsErrOut("  This function requires search table.\n");
		return YSERR;
	}


	int nFoundPlHd;
	const YsShellPolygonHandle *foundPlHd;

	if(search->FindPolygonListByEdge(nFoundPlHd,foundPlHd,shl,vtHd1,vtHd2)==YSOK && nFoundPlHd>0)
	{
		int i;
		YSBOOL inserted;
		inserted=YSFALSE;

		YsArray <YsShellPolygonHandle,2> plHdLst;
		plHdLst.Set(nFoundPlHd,foundPlHd);

		for(i=0; i<plHdLst.GetNumItem(); i++)
		{
			YsShellPolygonHandle plHd;
			int nOrgPlVt;
			const YsShellVertexHandle *orgVtHdLst;
			YsArray <YsShellVertexHandle,16> neoVtHdLst;

			plHd=plHdLst.GetItem(i);
			nOrgPlVt=shl.GetNumVertexOfPolygon(plHd);
			orgVtHdLst=shl.GetVertexListOfPolygon(plHd);

			int j;
			neoVtHdLst.Set(0,NULL);
			for(j=0; j<nOrgPlVt; j++)
			{
				if(orgVtHdLst[j]!=vtHd1 && orgVtHdLst[j]!=vtHd2)
				{
					neoVtHdLst.AppendItem(orgVtHdLst[j]);
				}
				if(orgVtHdLst[j]==vtHd1 && orgVtHdLst[(j+1)%nOrgPlVt]==vtHd2)
				{
					int k;
					for(k=0; k<nNewEdVt; k++)
					{
						neoVtHdLst.AppendItem(newEdVt[k]);
					}
					inserted=YSTRUE;
				}
				else if(orgVtHdLst[j]==vtHd2 && orgVtHdLst[(j+1)%nOrgPlVt]==vtHd1)
				{
					int k;
					for(k=nNewEdVt-1; k>=0; k--)
					{
						neoVtHdLst.AppendItem(newEdVt[k]);
					}
					inserted=YSTRUE;
				}
			}

			if(inserted==YSTRUE)
			{
				shl.ModifyPolygon(plHd,neoVtHdLst.GetNumItem(),neoVtHdLst.GetArray());
			}
			else
			{
				YsErrOut("YsReplaceShellEdge()\n");
				YsErrOut("  The search table is broken.\n");
				return YSERR;
			}
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


////////////////////////////////////////////////////////////


YSRESULT YsAddShell(YsShell &shl1,const YsShell &shl2)
{
	YsHashTable <YsShellVertexHandle> findVtx(1024);
	YsShellVertexHandle vtHd,newVtHd;
	unsigned key;

	vtHd=NULL;
	while((vtHd=shl2.FindNextVertex(vtHd))!=NULL)
	{
		YsVec3 pos;
		shl2.GetVertexPosition(pos,vtHd);
		newVtHd=shl1.AddVertexH(pos);
		key=shl2.GetSearchKey(vtHd);

		findVtx.AddElement(key,newVtHd);
	}

	int i,nPlVt;
	YsShellPolygonHandle plHd;
	const YsShellVertexHandle *plVtHd;
	YsArray <YsShellVertexHandle> newPlVtHd;

	plHd=NULL;
	while((plHd=shl2.FindNextPolygon(plHd))!=NULL)
	{
		nPlVt=shl2.GetNumVertexOfPolygon(plHd);
		plVtHd=shl2.GetVertexListOfPolygon(plHd);

		newPlVtHd.Set(0,NULL);
		for(i=0; i<nPlVt; i++)
		{
			key=shl2.GetSearchKey(plVtHd[i]);
			if(findVtx.FindElement(vtHd,key)==YSOK)
			{
				newPlVtHd.AppendItem(vtHd);
			}
			else
			{
				return YSERR;
			}
		}
		shl1.AddPolygonH(nPlVt,newPlVtHd.GetArray());
	}

	return YSOK;
}


