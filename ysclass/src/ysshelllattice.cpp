#include "ysclass.h"
#include "ysshell.h"


////////////////////////////////////////////////////////////

YsShellOctree::YsShellOctree()
{
	shl=NULL;
	nInsideOutsideComputed=0;
	nDerivedFromNeighbor=0;
}

YSSIDE YsShellOctree::CheckInside(const YsVec3 &pos) const
{
	const YsOctreeNode <YSSIDE> *node;
	node=GetNode(pos);
	if(node!=NULL)
	{
		return node->dat;
	}
	else
	{
		return YSOUTSIDE;
	}
}

YSRESULT YsShellOctree::TraverseFunc(YsOctreeNode <YSSIDE> *node,const YsVec3 &min,const YsVec3 &max,int param1,int param2)
{
	if(param1==-1) // Build Inside/Outside
	{
		if(node->dat==YSUNKNOWNSIDE && node->IsBottomMostNode()==YSTRUE)
		{
			YsVec3 cen;
			cen=(max+min)/2.0;
			node->dat=shl->CheckInsideSolid(cen);
			nInsideOutsideComputed++;
			SpreadInsideOutside(node, 1, 0, 0,node->dat,16);
			SpreadInsideOutside(node, 0, 1, 0,node->dat,16);
			SpreadInsideOutside(node, 0, 0, 1,node->dat,16);
			SpreadInsideOutside(node,-1, 0, 0,node->dat,16);
			SpreadInsideOutside(node, 0,-1, 0,node->dat,16);
			SpreadInsideOutside(node, 0, 0,-1,node->dat,16);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellOctree::SpreadInsideOutside(YsOctreeNode <YSSIDE> *node,int vx,int vy,int vz,YSSIDE side,int depthLimit)
{
	int ix,iy,iz;
	int nx,ny,nz;
	YsOctreeNode <YSSIDE> *parent,*seeker,*onestep;

	if(depthLimit>0)
	{
		// Climb up until it can move one step to the (vx,vy,vz)
		// Easily found neighbor
		//   Siblings of direct parent
		//   Direct child of some higher parent

		// So, let's check siblings first
		seeker=node;
		parent=node->GetParent();
		if(parent!=NULL)
		{
			if(parent->GetIndexNumberOf(ix,iy,iz,seeker)==YSOK)
			{
				nx=ix+vx;
				ny=iy+vy;
				nz=iz+vz;
				onestep=parent->GetChild(nx,ny,nz);
				if(onestep!=NULL)
				{
					SpreadInsideOutsideInverse(onestep,vx,vy,vz,side,depthLimit);
				}
			}
		}


		// Check direct children of some higher parent
		onestep=NULL;
		seeker=node;
		parent=node->GetParent();
		while(parent!=NULL)
		{
			if(parent->GetIndexNumberOf(ix,iy,iz,seeker)==YSOK)
			{
				nx=ix+vx;
				ny=iy+vy;
				nz=iz+vz;
				onestep=parent->GetChild(nx,ny,nz);
				if(onestep!=NULL)
				{
					SpreadInsideOutsideInverse(onestep,vx,vy,vz,side,1); // Note: depthLimit must be one
					break;
				}
			}
			seeker=parent;
			parent=parent->GetParent();
		}
		// Note : If no parent, cannot go any farther to (vx,vy,vz).
	}
	return YSOK;
}

YSRESULT YsShellOctree::SpreadInsideOutsideInverse(YsOctreeNode <YSSIDE> *node,int vx,int vy,int vz,YSSIDE side,int depthLimit)
{
	int i;
	int dirTable[6][3]=
	{
		{ 1, 0, 0},
		{ 0, 1, 0},
		{ 0, 0, 1},
		{-1, 0, 0},
		{ 0,-1, 0},
		{ 0, 0,-1}
	};

	if(depthLimit>0)
	{
		if(node->IsBottomMostNode()==YSTRUE)
		{
			if(node->dat!=YSUNKNOWNSIDE)
			{
				return YSOK;  // This node is already visited.
			}
			else
			{
				node->dat=side;
				nDerivedFromNeighbor++;
				for(i=0; i<6; i++)
				{
					if(vx==-dirTable[i][0] && vy==-dirTable[i][1] && vz==-dirTable[i][2])
					{
						// Don't take it.
					}
					else
					{
						SpreadInsideOutside(node,dirTable[i][0],dirTable[i][1],dirTable[i][2],side,depthLimit-1);
					}
				}
			}
		}
		else
		{
			// Note : Go backward direction
			if(vx==-1)
			{
				SpreadInsideOutsideInverse(node->GetChild(1,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vx==1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vy==-1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vy==1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vz==-1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,1),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,1),vx,vy,vz,side,depthLimit-1);
			}
			else if(vz==1)
			{
				SpreadInsideOutsideInverse(node->GetChild(0,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(0,1,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,0,0),vx,vy,vz,side,depthLimit-1);
				SpreadInsideOutsideInverse(node->GetChild(1,1,0),vx,vy,vz,side,depthLimit-1);
			}
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

YsShellLattice::YsShellLattice(int hashSize) : plgToCellTable(hashSize)
{
	shl=NULL;

	isSubLattice=YSFALSE;
	subLattice=NULL;

	checkInsideSolidLog=YSFALSE;
	ray1.Set(0.0,0.0,0.0);
	ray2.Set(0.0,0.0,0.0);
	indeterminantCellList.Set(0,NULL);

	enablePlgToCellTable=YSTRUE;
	enableInsideOutsideInfo=YSTRUE;

	plgToCellTable.tableName="PlgToLtcCell";
	plgToCellHashSize=hashSize;

	xyPlane=YSFALSE;
	xzPlane=YSFALSE;
	yzPlane=YSFALSE;
}

YsShellLattice::~YsShellLattice()
{
	if(subLattice!=NULL)
	{
		delete subLattice;
	}
}

void YsShellLattice::EnableAutoResizingHashTable(int minSize,int maxSize)
{
	plgToCellTable.EnableAutoResizing(minSize,maxSize);
}

void YsShellLattice::EnableAutoResizingHashTable(void)
{
	plgToCellTable.EnableAutoResizing();
}

void YsShellLattice::DisablePolygonToCellHashTable(void)
{
	enablePlgToCellTable=YSFALSE;
	plgToCellTable.PrepareTable(1);
}

void YsShellLattice::DisableInsideOutsideInfo(void)
{
	enableInsideOutsideInfo=YSFALSE;
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,int nCell)
{
	YsVec3 bbx[2],d;
	double vol,siz;
	int nx,ny,nz;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	vol/=(double)nCell;

	siz=pow(vol,1.0/3.0);

	nx=YsGreater(1,(int)(d.x()/siz));
	ny=YsGreater(1,(int)(d.y()/siz));
	nz=YsGreater(1,(int)(d.z()/siz));
	return SetDomain(shl,nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,const YsVec3 &bbxMin,const YsVec3 &bbxMax,int nCell)
{
	YsVec3 bbx[2],d;
	double vol,siz;
	int nx,ny,nz;
	YsBoundingBoxMaker3 mkBbx;

	mkBbx.Add(bbxMin);
	mkBbx.Add(bbxMax);
	mkBbx.Get(bbx[0],bbx[1]);

	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	vol/=(double)nCell;

	siz=pow(vol,1.0/3.0);

	nx=YsGreater(1,(int)(d.x()/siz));
	ny=YsGreater(1,(int)(d.y()/siz));
	nz=YsGreater(1,(int)(d.z()/siz));
	return SetDomain(shl,bbx[0],bbx[1],nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &sh,int nx,int ny,int nz)
{
	YsVec3 bbxMin,bbxMax;

	sh.GetBoundingBox(bbxMin,bbxMax);

	// >> Added 03/08/2002
	YsVec3 bbxSize,bbxExpand;
	double margine;
	bbxSize=bbxMax-bbxMin;
	margine=YsGreater(YsGreater(bbxSize.x(),bbxSize.y()),bbxSize.z());
	margine*=0.02;
	bbxExpand.Set(margine,margine,margine);
	bbxMin-=bbxExpand;
	bbxMax+=bbxExpand;
	// << Added 03/08/2002

	return SetDomain(sh,bbxMin,bbxMax,nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &sh,const YsVec3 &bbx0,const YsVec3 &bbx1,int nx,int ny,int nz)
{
	int n,i,bx,by,bz;
	YsVec3 bbxMin,bbxMax;
	YsBoundingBoxMaker3 makeBbx;
	YsLoopCounter ctr;

	YsPrintf("Building Shell Lattice\n");

	if(enablePlgToCellTable==YSTRUE)
	{
		plgToCellTable.PrepareTable();
	}
	if(subLattice!=NULL)
	{
		delete subLattice;
		subLattice=NULL;
	}

	makeBbx.Add(bbx0);
	makeBbx.Add(bbx1);
	makeBbx.Get(bbxMin,bbxMax);

	if(Create(nx,ny,nz,bbxMin,bbxMax)==YSOK)
	{
		shl=&sh;

		for(bz=0; bz<=GetNumBlockZ(); bz++)
		{
			for(by=0; by<=GetNumBlockY(); by++)
			{
				for(bx=0; bx<=GetNumBlockX(); bx++)
				{
					if(bx<GetNumBlockX() && by<GetNumBlockY() && bz<GetNumBlockZ())
					{
						GetEditableBlock(bx,by,bz)->side=YSUNKNOWNSIDE;
					}
					else
					{
						GetEditableBlock(bx,by,bz)->side=YSBOUNDARY;  // <- 2006/06/29
					}
				}
			}
		}


		xyPlane=YSTRUE;
		xzPlane=YSTRUE;
		yzPlane=YSTRUE;

		YsShellVertexHandle vtHd;
		vtHd=NULL;
		n=sh.GetNumVertex();
		i=0;
		YsPrintf("Set Vertices\n");
		ctr.BeginCounter(n);
		while((vtHd=sh.FindNextVertex(vtHd))!=NULL)
		{
			ctr.Increment();

			YsVec3 pos;
			sh.GetVertexPosition(pos,vtHd);
			if(YsEqual(pos.x(),0.0)!=YSTRUE)
			{
				yzPlane=YSFALSE;
			}
			if(YsEqual(pos.y(),0.0)!=YSTRUE)
			{
				xzPlane=YSFALSE;
			}
			if(YsEqual(pos.z(),0.0)!=YSTRUE)
			{
				xyPlane=YSFALSE;
			}

			if(AddVertex(vtHd)!=YSOK && isSubLattice!=YSTRUE)
			{
				YsErrOut("YsShellLattice::SetDomain()\n");
				YsErrOut("  Cannot add vertex\n");
				YsVec3 bbx[2];

				sh.GetBoundingBox(bbx[0],bbx[1]);
				YsErrOut("Bbx %s %s\n",bbx[0].Txt(),bbx[1].Txt());
				YsErrOut("Vtx %s\n",pos.Txt());
				return YSERR;
			}
			i++;
		}
		ctr.EndCounter();

		YsPrintf("Setting Polygons\n");
		YsShellPolygonHandle plHd;
		plHd=NULL;
		n=sh.GetNumPolygon();
		i=0;
		ctr.BeginCounter(n);
		while((plHd=sh.FindNextPolygon(plHd))!=NULL)
		{
			ctr.Increment();

			if(AddPolygon(plHd)!=YSOK)
			{
				YsErrOut("YsShellLattice::SetDomain()\n");
				YsErrOut("  Cannot add polygon\n");
				return YSERR;
			}
			i++;
		}
		ctr.EndCounter();

		if(enableInsideOutsideInfo==YSTRUE)
		{
			FillUndeterminedBlockRange(sh,0,0,0,GetNumBlockX()-1,GetNumBlockY()-1,GetNumBlockZ()-1);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShellLattice::SetDomain()\n");
		YsErrOut("  Cannot create lattice\n");
		return YSERR;
	}
}

YSRESULT YsShellLattice::SetDomainXYMode(const class YsShell &shl,int nCell)
{
	YsVec3 bbx[2],dgn;
	double areaBbx,areaPerCell;
	double lCell;
	int nx,ny;

	enableInsideOutsideInfo=YSFALSE;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	dgn=bbx[1]-bbx[0];

	areaBbx=dgn.x()*dgn.y();
	areaPerCell=areaBbx/((double)nCell+1);

	lCell=sqrt(areaPerCell);

	bbx[0].SetZ(-lCell/2.0);
	bbx[1].SetZ( lCell/2.0);

	nx=(int)(dgn.x()/lCell)+1;
	ny=(int)(dgn.y()/lCell)+1;

	return SetDomain(shl,bbx[0],bbx[1],nx,ny,1);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[],int nCell)
{
	YsVec3 bbx[2],d;
	double vol,siz;
	int nx,ny,nz;

	shl.GetBoundingBox(bbx[0],bbx[1]);
	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	vol/=(double)nCell;

	siz=pow(vol,1.0/3.0);

	nx=YsGreater(1,(int)(d.x()/siz));
	ny=YsGreater(1,(int)(d.y()/siz));
	nz=YsGreater(1,(int)(d.z()/siz));
	return SetDomain(shl,nPl,plHdList,nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &shl,int nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbxMin,const YsVec3 &bbxMax,int nCell)
{
	YsVec3 bbx[2],d;
	double vol,siz;
	int nx,ny,nz;
	YsBoundingBoxMaker3 mkBbx;

	mkBbx.Add(bbxMin);
	mkBbx.Add(bbxMax);
	mkBbx.Get(bbx[0],bbx[1]);

	d=bbx[1]-bbx[0];
	vol=d.x()*d.y()*d.z();
	vol/=(double)nCell;

	siz=pow(vol,1.0/3.0);

	nx=YsGreater(1,(int)(d.x()/siz));
	ny=YsGreater(1,(int)(d.y()/siz));
	nz=YsGreater(1,(int)(d.z()/siz));
	return SetDomain(shl,nPl,plHdList,bbx[0],bbx[1],nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &sh,int nPl,const YsShellPolygonHandle plHdList[],int nx,int ny,int nz)
{
	YsVec3 bbxMin,bbxMax;

	sh.GetBoundingBox(bbxMin,bbxMax);

	// >> Added 03/08/2002
	YsVec3 bbxSize,bbxExpand;
	double margine;
	bbxSize=bbxMax-bbxMin;
	margine=YsGreater(YsGreater(bbxSize.x(),bbxSize.y()),bbxSize.z());
	margine*=0.02;
	bbxExpand.Set(margine,margine,margine);
	bbxMin-=bbxExpand;
	bbxMax+=bbxExpand;
	// << Added 03/08/2002

	return SetDomain(sh,nPl,plHdList,bbxMin,bbxMax,nx,ny,nz);
}

YSRESULT YsShellLattice::SetDomain(const class YsShell &sh,int nPl,const YsShellPolygonHandle plHdList[],const YsVec3 &bbx0,const YsVec3 &bbx1,int nx,int ny,int nz)
{
	int n,i,bx,by,bz;
	YsVec3 bbxMin,bbxMax;
	YsBoundingBoxMaker3 makeBbx;
	YsLoopCounter ctr;

	YsPrintf("Building Shell Lattice\n");

	if(enablePlgToCellTable==YSTRUE)
	{
		plgToCellTable.PrepareTable();
	}
	if(subLattice!=NULL)
	{
		delete subLattice;
		subLattice=NULL;
	}

	makeBbx.Add(bbx0);
	makeBbx.Add(bbx1);
	makeBbx.Get(bbxMin,bbxMax);

	if(Create(nx,ny,nz,bbxMin,bbxMax)==YSOK)
	{
		shl=&sh;

		for(bz=0; bz<=GetNumBlockZ(); bz++)
		{
			for(by=0; by<=GetNumBlockY(); by++)
			{
				for(bx=0; bx<=GetNumBlockX(); bx++)
				{
					if(bx<GetNumBlockX() && by<GetNumBlockY() && bz<GetNumBlockZ())
					{
						GetEditableBlock(bx,by,bz)->side=YSUNKNOWNSIDE;
					}
					else
					{
						GetEditableBlock(bx,by,bz)->side=YSBOUNDARY;  // <- 2006/06/29
					}
				}
			}
		}


		xyPlane=YSTRUE;
		xzPlane=YSTRUE;
		yzPlane=YSTRUE;

		YsShellVertexHandle vtHd;
		vtHd=NULL;
		n=sh.GetNumVertex();
		i=0;
		YsPrintf("Set Vertices\n");
		ctr.BeginCounter(n);
		while((vtHd=sh.FindNextVertex(vtHd))!=NULL)
		{
			ctr.Increment();

			YsVec3 pos;
			sh.GetVertexPosition(pos,vtHd);
			if(YsEqual(pos.x(),0.0)!=YSTRUE)
			{
				yzPlane=YSFALSE;
			}
			if(YsEqual(pos.y(),0.0)!=YSTRUE)
			{
				xzPlane=YSFALSE;
			}
			if(YsEqual(pos.z(),0.0)!=YSTRUE)
			{
				xyPlane=YSFALSE;
			}

			if(AddVertex(vtHd)!=YSOK && isSubLattice!=YSTRUE)
			{
				YsErrOut("YsShellLattice::SetDomain()\n");
				YsErrOut("  Cannot add vertex\n");
				YsVec3 bbx[2];

				sh.GetBoundingBox(bbx[0],bbx[1]);
				YsErrOut("Bbx %s %s\n",bbx[0].Txt(),bbx[1].Txt());
				YsErrOut("Vtx %s\n",pos.Txt());
				return YSERR;
			}
			i++;
		}
		ctr.EndCounter();

		YsPrintf("Setting Polygons\n");
		YsShellPolygonHandle plHd;
		plHd=NULL;
		n=nPl;
		ctr.BeginCounter(n);
		for(i=0; i<nPl; i++)
		{
			ctr.Increment();

			if(AddPolygon(plHdList[i])!=YSOK)
			{
				YsErrOut("YsShellLattice::SetDomain()\n");
				YsErrOut("  Cannot add polygon\n");
				return YSERR;
			}
		}
		ctr.EndCounter();

		if(enableInsideOutsideInfo==YSTRUE)
		{
			FillUndeterminedBlockRange(sh,0,0,0,GetNumBlockX()-1,GetNumBlockY()-1,GetNumBlockZ()-1);
		}

		return YSOK;
	}
	else
	{
		YsErrOut("YsShellLattice::SetDomain()\n");
		YsErrOut("  Cannot create lattice\n");
		return YSERR;
	}
}

YSRESULT YsShellLattice::FillUndeterminedBlockRange(const class YsShell &shl,int bx0,int by0,int bz0,int bx1,int by1,int bz1)
{
	int n,nBlk,nBlkDone,bx,by,bz;
	YsLoopCounter ctr;

	nBlk=(bx1-bx0+1)*(by1-by0+1)*(bz1-bz0+1);
	nBlkDone=0;

	n=0;
	YsPrintf("Filling Undetermined Blocks\n");
	ctr.BeginCounter(nBlk);
	for(bz=bz0; bz<=bz1; bz++)
	{
		for(by=by0; by<=by1; by++)
		{
			for(bx=bx0; bx<=bx1; bx++)
			{
				ctr.Increment();

				if(GetBlock(bx,by,bz)->side==YSUNKNOWNSIDE)
				{
					YSSIDE side;
					YsVec3 blk1,blk2,cen;
					GetBlockRange(blk1,blk2,bx,by,bz);
					cen=(blk1+blk2)/2.0;

					side=shl.CheckInsideSolid(cen);
					FillUndeterminedBlock(bx,by,bz,side);
					n++;
				}

				nBlkDone++;
			}
		}
	}
	ctr.EndCounter();
	// YsPrintf("INFO: %d times FillUndeterminedBlock is fired.\n",n);

	return YSOK;
}

YSRESULT YsShellLattice::RepairLatticeAfterModification(YsShell &shl,int np,const YsVec3 p[])
{
	int bx0,by0,bz0,bx1,by1,bz1;
	GetBlockIndexRange(bx0,by0,bz0,bx1,by1,bz1,np,p);

	int bx,by,bz;
	for(bz=bz0; bz<=bz1; bz++)
	{
		for(by=by0; by<=by1; by++)
		{
			for(bx=bx0; bx<=bx1; bx++)
			{
				if(GetBlock(bx,by,bz)->side!=YSBOUNDARY)
				{
					YsShellLatticeElem *elm;
					elm=GetEditableBlock(bx,by,bz);
					elm->side=YSUNKNOWNSIDE;
				}
			}
		}
	}

	if(enableInsideOutsideInfo==YSTRUE)
	{
		FillUndeterminedBlockRange(shl,bx0,by0,bz0,bx1,by1,bz1);
	}

	return YSOK;
}

YSRESULT YsShellLattice::AddVertex(YsShellVertexHandle vtHd)
{
	YsVec3 vtxPos;

	if(shl->GetVertexPosition(vtxPos,vtHd)==YSOK)
	{
		int bx,by,bz;
		if(GetBlockIndex(bx,by,bz,vtxPos)==YSOK)
		{
			YsShellLatticeElem *elm;
			elm=GetEditableBlock(bx,by,bz);
			if(elm!=NULL)
			{
				elm->vtxList.AppendItem(vtHd);
				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsShellLattice::DeleteVertex(YsShellVertexHandle vtHd)
{
	YsVec3 vtxPos;

	if(shl->GetVertexPosition(vtxPos,vtHd)==YSOK)
	{
		int bx,by,bz;
		if(GetBlockIndex(bx,by,bz,vtxPos)==YSOK)
		{
			YsShellLatticeElem *elm;
			elm=GetEditableBlock(bx,by,bz);
			if(elm!=NULL)
			{
				int i;
				for(i=0; i<elm->vtxList.GetNumItem(); i++)
				{
					if(elm->vtxList[i]==vtHd)
					{
						elm->vtxList.Delete(i);
						return YSOK;
					}
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShellLattice::AddPolygon(YsShellPolygonHandle plHd)
{
	YsArray <YsVec3> plg;
	int bx,by,bz,bx1,by1,bz1,bx2,by2,bz2;
	YsCollisionOfPolygon coll;
	unsigned plgKey[1];
	YsFixedLengthArray <int,4> cell;
	YSBOOL watch;

	watch=YSFALSE;

	plgKey[0]=shl->GetSearchKey(plHd);

	if(PreparePolygon(plg,bx1,by1,bz1,bx2,by2,bz2,plHd)==YSOK)
	{
		// Check Intersection and mark
		int nCompute,nWaste;
		coll.SetPolygon1(plg.GetNumItem(),plg.GetArray());
		coll.PrecomputeProjectionOfPolygon1();

		if(watch==YSTRUE)
		{
			printf("A %d %d %d %d %d %d\n",bx1,by1,bz1,bx2,by2,bz2);
		}

		nCompute=0;
		nWaste=0;
		for(bz=bz1; bz<=bz2; bz++)
		{
			for(by=by1; by<=by2; by++)
			{
				for(bx=bx1; bx<=bx2; bx++)
				{
					nCompute++;

					YsVec3 blk1,blk2;
					YsShellLatticeElem *elm;
					GetBlockRange(blk1,blk2,bx,by,bz);

					// YsInflateBoundingBox(blk1,blk2,blk1,blk2,1.01);  // Experimentalily added on 10/24/2001

					if(watch==YSTRUE)
					{
						printf("(%s)-(%s)\n",blk1.Txt(),blk2.Txt());
						printf("%d %d %d %d\n",bx,by,bz,coll.CheckIntersectionBetweenPolygon1AndBlock(blk1,blk2));
					}

					if(coll.CheckIntersectionBetweenPolygon1AndBlock(blk1,blk2)==YSTRUE &&
					   (elm=GetEditableBlock(bx,by,bz))!=NULL)
					{
						elm->side=YSBOUNDARY;
						elm->plgList.AppendItem(plHd);

						if(enablePlgToCellTable==YSTRUE)
						{
							cell.dat[0]=bx;
							cell.dat[1]=by;
							cell.dat[2]=bz;
							cell.dat[3]=elm->plgList.GetNumItem()-1;  // Index number can become less
							plgToCellTable.AddElement(1,plgKey,cell);
						}
					}
					else
					{
						nWaste++;
					}
				}
			}
		}
		// YsPrintf("Computed %d Wasted %d\n",nCompute,nWaste);
	}

	return YSOK;
}

YSRESULT YsShellLattice::DeletePolygon(YsShellPolygonHandle plHd)
{
	// const YsArray <YsFixedLengthArray <int,4>,16> *cellList;
	const YsArray <YsFixedLengthArray <int,4>,4> *cellList;

	unsigned plgKey[1];
	int i,k,bx,by,bz;
	YsShellLatticeElem *elm;
	YSBOOL found;

	if(enablePlgToCellTable!=YSTRUE)
	{
		YsPrintf("YsShellLattice::DeletePolygon is unavailable because\n");
		YsPrintf("plgToCellTable is disabled.\n");
		return YSERR;
	}


	plgKey[0]=shl->GetSearchKey(plHd);
	cellList=plgToCellTable.FindElement(1,plgKey);
	if(cellList!=NULL && cellList->GetNumItem()>0)
	{
		for(i=0; i<cellList->GetNumItem(); i++)
		{
			bx=(*cellList)[i].dat[0];
			by=(*cellList)[i].dat[1];
			bz=(*cellList)[i].dat[2];

			elm=GetEditableBlock(bx,by,bz);

			if(elm!=NULL)
			{
				found=YSFALSE;
				k=YsSmaller((*cellList)[i].dat[3],elm->plgList.GetNumItem()-1);
				while(k>=0)
				{
					if(elm->plgList.GetItem(k)==plHd)
					{
						found=YSTRUE;
						elm->plgList.Delete(k);
						if(elm->plgList.GetNumItem()==0)
						{
							YsFixedLengthArray <int,3> cellPosition;
							cellPosition.dat[0]=bx;
							cellPosition.dat[1]=by;
							cellPosition.dat[2]=bz;
							indeterminantCellList.AppendItem(cellPosition);
						}
						break;
					}
					k--;
				}
				if(found!=YSTRUE)
				{
					YsErrOut("YsShellLattice::DeletePolygon()\n");
					YsErrOut("  Lattice and plgToCellTable do not match.\n");
					return YSERR;
				}
			}
			else
			{
				YsErrOut("YsShellLattice::DeletePolygon()\n");
				YsErrOut("  Internal Error\n");
				return YSERR;
			}
		}
		plgToCellTable.DeleteKey(1,plgKey);
		return YSOK;
	}

	YsErrOut("YsShellLattice::DeletePolygon()\n");
	YsErrOut("  Tried to delete a polygon that is not listed.\n");
	return YSERR;
}

YSRESULT YsShellLattice::MakeSortedCellPolygonBalance(YsArray <int> &balance) const
{
	YsArray <YsFixedLengthArray <unsigned short,3> > xyz;
	YsArray <int> n;
	int x,y,z,i;

	xyz.Set(GetNumBlockX()*GetNumBlockY()*GetNumBlockZ(),NULL);
	n.Set(xyz.GetN(),NULL);

	i=0;
	for(x=0; x<GetNumBlockX(); x++)
	{
		for(y=0; y<GetNumBlockY(); y++)
		{
			for(z=0; z<GetNumBlockZ(); z++)
			{
				const YsShellLatticeElem *elm;
				elm=GetBlock(x,y,z);
				xyz[i][0]=x;
				xyz[i][1]=y;
				xyz[i][2]=z;
				n[i]=elm->plgList.GetN();
				i++;
			}
		}
	}

	YsQuickSort <int,YsFixedLengthArray <unsigned short,3> > (n.GetN(),n,xyz);
	i=0;
	balance.Set(n.GetN()*4,NULL);
	for(i=0; i<n.GetN(); i++)
	{
		balance[i*4  ]=xyz[n.GetN()-1-i][0];
		balance[i*4+1]=xyz[n.GetN()-1-i][1];
		balance[i*4+2]=xyz[n.GetN()-1-i][2];
		balance[i*4+3]=n[n.GetN()-1-i];
	}

	return YSOK;
}

YSBOOL YsShellLattice::CheckSubLatticeNecessity
    (int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsVec3 bbx[2],int &percentPlg,int &percentVol)
{
	const int nHist=20;
	int i,n,nPlgTotal,hist[nHist];
	YsArray <int> cellBalance;
	MakeSortedCellPolygonBalance(cellBalance);



	nPlgTotal=0;
	for(i=0; i<cellBalance.GetN(); i+=4)
	{
		n=cellBalance[i+3];
		nPlgTotal+=n;
		if(n==0)
		{
			cellBalance.Resize(i);
			break;
		}
	}

	// printf("nPlgTotal %d (actual %d)\n",nPlgTotal,shl->GetNumPolygon());


	if(nPlgTotal>0)
	{
		for(i=0; i<nHist; i++)
		{
			hist[i]=0;
		}
		for(i=0; i<cellBalance.GetN(); i+=4)
		{
			n=cellBalance[i+3];
			hist[i*10/cellBalance.GetN()]+=n;
		}

		// for(i=0; i<nHist; i++)
		// {
		// 	YsPrintf("[%d] %d (%d%%)\n",i,hist[i],hist[i]*100/nPlgTotal);
		// }

		int nMainCell,nSubCell;
		int nPlgSub,nTest;
		nMainCell=GetNumBlockX()*GetNumBlockY()*GetNumBlockZ();

		bx1=cellBalance[0];
		by1=cellBalance[1];
		bz1=cellBalance[2];
		bx2=cellBalance[0];
		by2=cellBalance[1];
		bz2=cellBalance[2];

		nPlgSub=cellBalance[3];
		nTest=1;
		for(i=4; i<cellBalance.GetN(); i+=4)
		{
			int cx1,cy1,cz1,cx2,cy2,cz2;

			// YsPrintf("(%d %d %d) %d\n",cellBalance[i],cellBalance[i+1],cellBalance[i+2],cellBalance[i+3]);

			cx1=YsSmaller(bx1,cellBalance[i  ]);
			cy1=YsSmaller(by1,cellBalance[i+1]);
			cz1=YsSmaller(bz1,cellBalance[i+2]);
			cx2=YsGreater(bx2,cellBalance[i  ]);
			cy2=YsGreater(by2,cellBalance[i+1]);
			cz2=YsGreater(bz2,cellBalance[i+2]);

			nSubCell=(cx2-cx1+1)*(cy2-cy1+1)*(cz2-cz1+1);
			if(nMainCell/10<nSubCell)
			{
				printf("Exceeds 10%% of # of total cells (%d,%d)\n",nMainCell/10,nSubCell);
				break;
			}
			if(cellBalance[i+3]<(nPlgSub/nTest)/10)  // Polygon count drops 10% of the average
			{
				YsPrintf("Drop below 10%% of average (%d)\n",(nPlgSub/nTest)/10);
				break;
			}

			bx1=cx1;
			by1=cy1;
			bz1=cz1;
			bx2=cx2;
			by2=cy2;
			bz2=cz2;
			nPlgSub+=cellBalance[i+3];
			nTest++;
		}

		YsPrintf("Range (%d %d %d)-(%d %d %d)\n",bx1,by1,bz1,bx2,by2,bz2);

		nSubCell=(bx2-bx1+1)*(by2-by1+1)*(bz2-bz1+1);
		if(nSubCell<nMainCell/10)
		{
			int x,y,z,nPlg,k;
			YsHashTable <int> visited(shl->GetNumPolygon()/10+1);

			nPlg=0;
			for(x=bx1; x<=bx2; x++)
			{
				for(y=by1; y<=by2; y++)
				{
					for(z=bz1; z<=bz2; z++)
					{
						const YsShellLatticeElem *elm;
						elm=GetBlock(x,y,z);
						forYsArray(k,elm->plgList)
						{
							if(visited.CheckKeyExist(shl->GetSearchKey(elm->plgList[k]))!=YSTRUE)
							{
								visited.AddElement(shl->GetSearchKey(elm->plgList[k]),0);
								nPlg++;
							}
						}
					}
				}
			}

			percentPlg=nPlg*100/shl->GetNumPolygon();
			percentVol=nSubCell*100/nMainCell;
			YsPrintf("%d%% of polygons are concentrated in %d%% of the bounding volume.\n",percentPlg,percentVol);

			// Ad-hoc conditions :-P
			// 1% -> 30%
			// 5% -> 50%
			// 9% -> 70%
			// (percentVol-1)*5+30<percentPlg  =>  percentVol*5+25<percentPlg
			if(percentVol*5+25<percentPlg)
			{
				GetGridPosition(bbx[0],bx1  ,by1  ,bz1);
				GetGridPosition(bbx[1],bx2+1,by2+1,bz2+1);
				return YSTRUE;
			}
		}
	}

	percentPlg=0;
	percentVol=0;
	return YSFALSE;
}

YSBOOL YsShellLattice::MakeSubLattice(int nCell)
{
	YsVec3 bbx[2];
	int percentPlg,percentVol,nx,ny,nz;
	int bx1,by1,bz1,bx2,by2,bz2;

	if(CheckSubLatticeNecessity(bx1,by1,bz1,bx2,by2,bz2,bbx,percentPlg,percentVol)==YSTRUE)
	{
		CalculateNxNyNzFromNCell(nx,ny,nz,bbx[0],bbx[1],nCell);

		if(subLattice!=NULL)
		{
			delete subLattice;
		}
		subLattice=new YsShellLattice(plgToCellHashSize);

		subLattice->enablePlgToCellTable=enablePlgToCellTable;
		subLattice->enableInsideOutsideInfo=enableInsideOutsideInfo;

		subLattice->isSubLattice=YSTRUE;  // <- must come before SetDomain

		subLattice->SetDomain(*shl,bbx[0],bbx[1],nx,ny,nz);

		return YSTRUE;
	}

	return YSFALSE;
}

const YsShellLattice *YsShellLattice::GetSubLattice(void) const
{
	return subLattice;
}

YSRESULT YsShellLattice::AddEdgePiece(const YsShellVertexHandle edVtHd[2],unsigned int ceKey)
{
	YsVec3 p1,p2;
	YsArray <unsigned,64> blkList;

	shl->GetVertexPosition(p1,edVtHd[0]);
	shl->GetVertexPosition(p2,edVtHd[1]);

	if(MakeIntersectingBlockList(blkList,p1,p2)==YSOK)
	{
		int i;
		YsShellLatticeEdgePiece piece;
		piece.edVtHd[0]=edVtHd[0];
		piece.edVtHd[1]=edVtHd[1];
		piece.ceKey=ceKey;
		for(i=0; i<=blkList.GetN()-3; i+=3)
		{
			YsShellLatticeElem *elm;
			elm=GetEditableBlock(blkList[i],blkList[i+1],blkList[i+2]);
			if(elm!=NULL)
			{
				elm->edgList.Append(piece);
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::FixIndeterminantCell(void)
{
	int n,i;
	n=indeterminantCellList.GetNumItem();
	for(i=n-1; i>=0; i--)
	{
		int bx,by,bz;
		bx=indeterminantCellList[i].dat[0];
		by=indeterminantCellList[i].dat[1];
		bz=indeterminantCellList[i].dat[2];

		YsShellLatticeElem *elm;
		elm=GetEditableBlock(bx,by,bz);

		if(elm->plgList.GetNumItem()>0)   // <- Means the cell got a new intersection
		{
			indeterminantCellList.Delete(i);
		}
		else                              // <- Means the cell must become YSINSIDE or YSOUTSIDE
		{
			int j,nei[6][3]=
			{
				{ 1, 0, 0},
				{-1, 0, 0},
				{ 0, 1, 0},
				{ 0,-1, 0},
				{ 0, 0, 1},
				{ 0, 0,-1}
			};

			YSSIDE side;
			side=YSBOUNDARY;
			for(j=0; j<6; j++)
			{
				const YsShellLatticeElem *ref;
				int dx,dy,dz;
				dx=nei[j][0];
				dy=nei[j][1];
				dz=nei[j][2];
				ref=GetBlock(bx+dx,by+dy,bz+dz);
				if(ref!=NULL && (ref->side==YSINSIDE || ref->side==YSOUTSIDE))
				{
					side=ref->side;
					break;
				}
			}

			if(side==YSBOUNDARY)   // <- Neighbor cells are all YSBOUNDARY
			{
				YsVec3 blk1,blk2;
				GetBlockRange(blk1,blk2,bx,by,bz);
				side=CheckInsideSolid((blk1+blk2)/2.0);
				if(side==YSBOUNDARY || side==YSUNKNOWNSIDE)
				{
					YsErrOut("YsShellLattice::FixIndeterminantCell()\n");
					YsErrOut("  Cannot fix an indeterminant cell %d %d %d\n",bx,by,bz);
					YsErrOut("  SIDE=%s\n",(side==YSBOUNDARY ? "BOUNDARY" : "UNKNOWN"));
				}
			}

			if(side!=YSBOUNDARY)
			{
				elm->side=side;
				indeterminantCellList.Delete(i);
			}
		}
	}

	if(indeterminantCellList.GetNumItem()==0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSSIDE YsShellLattice::CheckInsideSolid(const YsVec3 &pnt,YSBOOL showWhichFunctionDetermined) const
{
	const YsShellLatticeElem *elm;
	int bx,by,bz;



	if(enableInsideOutsideInfo!=YSTRUE)
	{
		YsErrOut("YsShellLattice::CheckInsideSolid()\n");
		YsErrOut("  Inside/Outside information is disabled.\n");
		return YSUNKNOWNSIDE;
	}


	if(subLattice!=NULL)
	{
		YSSIDE side;
		YsVec3 subLatRange[2];
		subLattice->GetLatticeRange(subLatRange[0],subLatRange[1]);
		if(YsCheckInsideBoundingBox3(pnt,subLatRange[0],subLatRange[1])==YSTRUE)
		{
			side=subLattice->CheckInsideSolid(pnt,showWhichFunctionDetermined);
			if(side!=YSUNKNOWNSIDE)
			{
				return side;
			}
		}
	}



	if(checkInsideSolidLog==YSTRUE)
	{
		foundIntersection.Set(0,NULL);
		foundPolygon.Set(0,NULL);
	}

	if(GetBlockIndex(bx,by,bz,pnt)==YSOK && (elm=GetBlock(bx,by,bz))!=NULL)
	{
		if(elm->side==YSINSIDE || elm->side==YSOUTSIDE)
		{
			if(showWhichFunctionDetermined==YSTRUE)
			{
				YsPrintf("YsShellLattice::CheckInsideSolid()\n");
				YsPrintf("  Check 1\n");
			}
			return elm->side;
		}
		else if(elm->side==YSBOUNDARY)
		{
			if(showWhichFunctionDetermined==YSTRUE)
			{
				YsPrintf("YsShellLattice::CheckInsideSolid()\n");
				YsPrintf("  Check 2\n");
			}
			return SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary(pnt,showWhichFunctionDetermined);
		}
		else
		{
			YsErrOut("YsShellLattice::CheckInsideSolid()\n");
			YsErrOut("  Error : Undetermined Cell Remains!\n");
			if(isSubLattice!=YSTRUE)
			{
				return shl->CheckInsideSolid(pnt);
			}
			else
			{
				return YSUNKNOWNSIDE;
			}
		}
	}
	else
	{
		// Outside the lattice. Cannot do much on in.
		if(showWhichFunctionDetermined==YSTRUE)
		{
			YsPrintf("YsShellLattice::CheckInsideSolid()\n");
			YsPrintf("  Report : Outside of the grids.  Assuming outside of the solid.\n");
		}

		// Modified 2000/11/10
		// return shl->CheckInsideSolid(pnt);

		if(isSubLattice!=YSTRUE)
		{
			return YSOUTSIDE;
		}
		else
		{
			return YSUNKNOWNSIDE;
		}
	}
}

YSRESULT YsShellLattice::FindFirstIntersection
    (YsShellPolygonHandle &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,YsShellPolygonHandle plHdExclude) const
{
	if(plHdExclude!=NULL)
	{
		return FindFirstIntersection(iPlHd,itsc,org,dir,1,&plHdExclude);
	}
	else
	{
		return FindFirstIntersection(iPlHd,itsc,org,dir,0,NULL);
	}
}


YSRESULT YsShellLattice::FindFirstIntersection
    (YsShellPolygonHandle  &iPlHd,YsVec3 &itsc,
     const YsVec3 &org,const YsVec3 &dir,int nPlHdExclude,const YsShellPolygonHandle plHdExclude[]) const
{
	if(subLattice!=NULL)
	{
		YsVec3 subLatRange[2];
		subLattice->GetLatticeRange(subLatRange[0],subLatRange[1]);
		if(YsCheckInsideBoundingBox3(org,subLatRange[0],subLatRange[1])==YSTRUE &&
		   subLattice->FindFirstIntersection(iPlHd,itsc,org,dir,nPlHdExclude,plHdExclude)==YSOK &&
		   iPlHd!=NULL)
		{
			return YSOK;
		}
	}



	YsArray <YsVec3> plg;
	YsShellPolygonHandle visitTest;
	YsHashTable <YsShellPolygonHandle> visited(255);
	YSBOOL watch;

	watch=YSFALSE;

	iPlHd=NULL;
	itsc=YsOrigin();

	YsVec3 lat[2],p1,p2;
	double diagonal;
	GetLatticeRange(lat[0],lat[1]);
	diagonal=(lat[1]-lat[0]).GetLength();
	if(YsCheckInsideBoundingBox3(org,lat[0],lat[1])==YSTRUE)
	{
		p1=org;
	}
	else
	{
		YsVec3 c1,c2;

		if(YsClipInfiniteLine3(c1,c2,org,dir,lat[0],lat[1])==YSOK)
		{
			if(YsCheckInsideBoundingBox3(c1,lat[0],lat[1])!=YSTRUE)
			{
				YsErrOut("YsShellLattice::FindFirstIntersection()\n");
				YsErrOut("  YsClipInfiniteLine returned YSOK, but the point doesn't give voxel index.\n");
				return YSERR;
			}
			p1=c1;
		}
		else
		{
			// Implication: This ray doesn't even hit the bounding box of the shell.
			return YSOK;
		}
	}



	YsArray <unsigned,64> blkList;
	if(MakeIntersectingBlockList(blkList,p1,p1+dir*diagonal)==YSOK)
	{
		int i;
		for(i=0; i<blkList.GetN(); i+=3)
		{
			const YsShellLatticeElem *elm;
			YsShellPolygonHandle plHd;
			elm=GetBlock(blkList[i],blkList[i+1],blkList[i+2]);

			if(elm!=NULL && elm->plgList.GetN()>0)
			{
				int k;
				for(k=0; k<elm->plgList.GetN(); k++)
				{
					plHd=elm->plgList[k];


					if(watch==YSTRUE)
					{
						printf("PlId=%d Blk=%d %d %d\n",shl->GetPolygonIdFromHandle(plHd),blkList[i],blkList[i+1],blkList[i+2]);
					}

					if(visited.FindElement(visitTest,shl->GetSearchKey(plHd))==YSOK)
					{
						continue;
					}

					visited.AddElement(shl->GetSearchKey(plHd),plHd);

					if(YsIsIncluded <YsShellPolygonHandle> (nPlHdExclude,plHdExclude,plHd)!=YSTRUE && 
					   shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
					{
						int j;
						int nForward;
						nForward=0;
						for(j=0; j<plg.GetN(); j++)
						{
							if((plg[j]-org)*dir>-YsTolerance)
							{
								nForward++;
								break;
							}
						}

						if(nForward>0)
						{
							YsPlane pln;
							YsVec3 crs;
							if(pln.MakeBestFitPlane(plg.GetN(),plg)==YSOK)
							{
								if(watch==YSTRUE)
								{
									printf("GetItsc %d\n",pln.GetIntersection(crs,org,dir));
									printf("Pos %s\n",crs.Txt());
									printf("DotProd %lf\n",(crs-org)*dir);
								}

								if(pln.GetIntersection(crs,org,dir)==YSOK && (crs-org)*dir>-YsTolerance)
								{
									YSSIDE side;
									if(plg.GetN()==3)
									{
										side=YsCheckInsideTriangle3(crs,plg);
									}
									else
									{
									 	side=YsCheckInsidePolygon3(crs,plg.GetN(),plg,pln.GetNormal());
									}
									if(watch==YSTRUE)
									{
										printf("Side %d\n",side);
									}

								 	if(side==YSBOUNDARY || side==YSINSIDE)
								 	{
										if(iPlHd==NULL)
										{
											itsc=crs;
											iPlHd=plHd;
										}
										else
										{
											if((crs-org).GetSquareLength()<(itsc-org).GetSquareLength())
											{
												itsc=crs;
												iPlHd=plHd;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsShellLattice::GetCellListFromPolygon
    (YsArray <YsFixedLengthArray <int,4>,4> &cell,YsShellPolygonHandle plHd) const
{
	unsigned int key[1];
	const YsArray <YsFixedLengthArray <int,4>,4> *found;

	key[0]=shl->GetSearchKey(plHd);
	found=plgToCellTable.FindElement(1,key);
	if(found!=NULL)
	{
		cell.Set(found->GetN(),*found);
		return YSOK;
	}
	cell.Set(0,NULL);
	return YSERR;
}

YSRESULT YsShellLattice::SelfDiagnostic(YsArray <YsVec3> &disagree)
{
	int i,n,bx,by,bz,nTest,tx,ty,tz;
	YsLoopCounter ctr;

	disagree.Set(0,NULL);
	n=GetNumBlockX()*GetNumBlockY()*GetNumBlockZ();
	i=0;
	ctr.BeginCounter(n);
	for(bz=0; bz<GetNumBlockZ(); bz++)
	{
		for(by=0; by<GetNumBlockY(); by++)
		{
			for(bx=0; bx<GetNumBlockX(); bx++)
			{
				ctr.Increment();

				const YsShellLatticeElem *elm;
				elm=GetBlock(bx,by,bz);
				if(elm->side==YSINSIDE || elm->side==YSOUTSIDE)
				{
					nTest=1;
				}
				else
				{
					nTest=8;
				}

				YsVec3 blk1,blk2,cen;
				GetBlockRange(blk1,blk2,bx,by,bz);
				for(tx=1; tx<=nTest; tx++)
				{
					for(ty=1; ty<=nTest; ty++)
					{
						for(tz=1; tz<=nTest; tz++)
						{
							double xp,yp,zp;
							xp=double(tx)/double(nTest+1);
							yp=double(ty)/double(nTest+1);
							zp=double(tz)/double(nTest+1);

							cen.Set(blk1.x()*xp+blk2.x()*(1.0-xp),
							        blk1.y()*yp+blk2.y()*(1.0-yp),
							        blk1.z()*zp+blk2.z()*(1.0-zp));

							YSSIDE side1,side2;
							side1=CheckInsideSolid(cen);
							side2=shl->CheckInsideSolid(cen);
							if(side1!=side2)
							{
								disagree.AppendItem(cen);
							}
						}
					}
				}

				i++;
			}
		}
	}
	ctr.EndCounter();
	YsPrintf("(# of Bad Result %d)\n",disagree.GetNumItem());

	if(disagree.GetNumItem()==0)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShellLattice::MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,int np,const YsVec3 p[]) const
{
	int i;
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;

	bbx.Begin(p[0]);
	for(i=1; i<np; i++)
	{
		bbx.Add(p[i]);
	}

	bbx.Get(bbxMin,bbxMax);

	if(subLattice!=NULL &&  // 2006/08/15
	   YsCheckInsideBoundingBox3(bbxMin,subLattice->min,subLattice->max)==YSTRUE &&
	   YsCheckInsideBoundingBox3(bbxMax,subLattice->min,subLattice->max)==YSTRUE)
	{
		return subLattice->MakePossibllyCollidingPolygonList(plgList,np,p);
	}

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		return MakeListOfPolygonsThatGoThroughTheBlockRegion(plgList,bx1,by1,bz1,bx2,by2,bz2);
	}
	return YSERR;
}

YSRESULT YsShellLattice::MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plgList,const YsVec3 &vt1,const YsVec3 &vt2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;

	bbx.Begin(vt1);
	bbx.Add(vt2);

	bbx.Get(bbxMin,bbxMax);

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		return MakeListOfPolygonsThatGoThroughTheBlockRegion(plgList,bx1,by1,bz1,bx2,by2,bz2);
	}
	return YSERR;
}

YSRESULT YsShellLattice::MakePossibllyCollidingPolygonList
	    (YsArray <YsShellPolygonHandle,16> &plHdList,YsShellPolygonHandle plHd) const
{
	unsigned plgKey[1];
	// const YsArray <YsFixedLengthArray <int,4>,16> *cellList;
	const YsArray <YsFixedLengthArray <int,4>,4> *cellList;


	if(enablePlgToCellTable!=YSTRUE)
	{
		YsPrintf("YsShellLattice::MakePossibllyCollidingPolygonList(plHdList,plHd)\n");
		YsPrintf("  This function is unavailable because plgToCellTable is disabled.\n");
		return YSERR;
	}


	plgKey[0]=shl->GetSearchKey(plHd);
	cellList=plgToCellTable.FindElement(1,plgKey);

	plHdList.Set(0,NULL);

	if(cellList!=NULL)
	{
		int i;
		for(i=0; i<cellList->GetN(); i++)
		{
			const YsShellLatticeElem *elm;
			elm=GetBlock((*cellList)[i][0],(*cellList)[i][1],(*cellList)[i][2]);
			if(elm!=NULL)
			{
				plHdList.Append(elm->plgList.GetN(),elm->plgList);
			}
		}

		/* YsArray <unsigned,256> plKeyList;
		plKeyList.Set(plHdList.GetN(),NULL);
		for(i=0; i<plHdList.GetN(); i++)
		{
			plKeyList[i]=shl->GetSearchKey(plHdList[i]);
		}
		YsQuickSort <unsigned,YsShellPolygonHandle> (plKeyList.GetN(),plKeyList,plHdList); */ // Commented out 2009/03/30
		YsQuickSortAny <YsShellPolygonHandle,int> (plHdList.GetN(),plHdList,NULL);  // Replace the above block 2009/03/30

		for(i=plHdList.GetN()-1; i>0; i--)
		{
			if(plHdList[i]==plHdList[i-1] || plHdList[i]==plHd)
			{
				plHdList.DeleteBySwapping(i);
			}
		}
		if(plHdList[0]==plHd)  // The previous for-loop doesn't cover plHdList[0], covers only down to plHdList[1].
		{
			plHdList.DeleteBySwapping(0);
		}
	}

	return YSOK;
}

YSRESULT YsShellLattice::MakePossibllyIncludedVertexList
        (YsArray <YsShellVertexHandle,16> &vtxList,int np,const YsVec3 p[]) const
{
	int i;
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;


	bbx.Begin(p[0]);
	for(i=1; i<np; i++)
	{
		bbx.Add(p[i]);
	}

	bbx.Get(bbxMin,bbxMax);

	vtxList.Set(0,NULL);

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		int bx,by,bz,i;
		for(bx=bx1; bx<=bx2; bx++)
		{
			for(by=by1; by<=by2; by++)
			{
				for(bz=bz1; bz<=bz2; bz++)
				{
					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL)
					{
						for(i=0; i<elm->vtxList.GetNumItem(); i++)
						{
							vtxList.AppendItem(elm->vtxList[i]);
						}
					}
					else
					{
						YsErrOut("YsShellLattice::MakePossibllyIncludedVertexList()\n");
						YsErrOut("  Internal Error(1)\n");
						return YSERR;
					}
				}
			}
		}

		// >> 2007/06/27
		YsArray <unsigned int,16> vtKeyList;
		vtKeyList.Set(vtxList.GetN(),NULL);
		forYsArray(i,vtxList)
		{
			vtKeyList[i]=shl->GetSearchKey(vtxList[i]);
		}
		YsQuickSort <unsigned,YsShellVertexHandle> (vtKeyList.GetN(),vtKeyList,vtxList);

		for(i=vtxList.GetN()-1; i>0; i--)
		{
			if(vtxList[i]==vtxList[i-1])
			{
				vtxList.DeleteBySwapping(i);
			}
		}
		// << 2007/06/27

		return YSOK;
	}

	return YSERR;
}

YSRESULT YsShellLattice::MakeEdgePieceListByBoundingBox(
     YsArray <YsShellVertexHandle,16> &edVtHd,YsArray <unsigned int,16> &ceKey,
     const YsVec3 &vt1,const YsVec3 &vt2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 bbxMin,bbxMax;
	int bx1,by1,bz1,bx2,by2,bz2;
	YsFixedLengthToMultiHashTable <unsigned int,2,1> visitedEdge(256);

	visitedEdge.EnableAutoResizing(256,256+shl->GetNumPolygon()/10);

	bbx.Add(vt1);
	bbx.Add(vt2);
	bbx.Get(bbxMin,bbxMax);

	edVtHd.Set(0,NULL);
	ceKey.Set(0,NULL);

	if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
	   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
	{
		int bx,by,bz,i;
		for(bx=bx1; bx<=bx2; bx++)
		{
			for(by=by1; by<=by2; by++)
			{
				for(bz=bz1; bz<=bz2; bz++)
				{
					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL)
					{
						forYsArray(i,elm->edgList)
						{
							unsigned int edVtKey[2];
							edVtKey[0]=shl->GetSearchKey(elm->edgList[i].edVtHd[0]);
							edVtKey[1]=shl->GetSearchKey(elm->edgList[i].edVtHd[1]);
							if(visitedEdge.CheckKeyExist(2,edVtKey)!=YSTRUE)
							{
								visitedEdge.AddElement(2,edVtKey,0);
								edVtHd.Append(2,elm->edgList[i].edVtHd);
								ceKey.Append(elm->edgList[i].ceKey);
							}
						}
					}
					else
					{
						YsErrOut("YsShellLattice::MakeEdgePieceListByBoundingBox()\n");
						YsErrOut("  Internal Error(1)\n");
						return YSERR;
					}
				}
			}
		}
		return YSOK;
	}

	return YSERR;
}

YSBOOL YsShellLattice::CheckPolygonIntersection(YsShellPolygonHandle plHd) const
{
	int i;
	YsArray <YsShellPolygonHandle,16> collPlHdList;
	int nOrgPlVtHd;
	const YsShellVertexHandle *orgPlVtHd;
	YsCollisionOfPolygon collision;
	const YsShell &shl=*(this->shl);
	YsArray <YsVec3> plVtPos;

	shl.GetVertexListOfPolygon(plVtPos,plHd);
	collision.SetPolygon1(plVtPos.GetN(),plVtPos);

	shl.GetVertexListOfPolygon(nOrgPlVtHd,orgPlVtHd,plHd);

	MakePossibllyCollidingPolygonList(collPlHdList,plHd);
	forYsArray(i,collPlHdList)
	{
		int j,nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsVec3  edVtPos[2];
		YsShellVertexHandle edVtHd[2];

		shl.GetVertexListOfPolygon(nPlVt,plVtHd,collPlHdList[i]);

		edVtHd[1]=plVtHd[nPlVt-1];
		shl.GetVertexPosition(edVtPos[1],plVtHd[nPlVt-1]);
		for(j=0; j<nPlVt; j++)
		{
			edVtHd[0]=edVtHd[1];
			edVtHd[1]=plVtHd[j];

			edVtPos[0]=edVtPos[1];
			shl.GetVertexPosition(edVtPos[1],plVtHd[j]);

			if(YsIsIncluded <YsShellVertexHandle> (nOrgPlVtHd,orgPlVtHd,edVtHd[0])==YSTRUE ||
			   YsIsIncluded <YsShellVertexHandle> (nOrgPlVtHd,orgPlVtHd,edVtHd[1])==YSTRUE)
			{
				goto NEXTEDGE;
			}

			if(collision.CheckIntersectionBetweenPolygon1AndLineSegment(edVtPos[0],edVtPos[1])==YSTRUE)
			{
				return YSTRUE;
			}
		NEXTEDGE:
			;
		}
	}
	return YSFALSE;
}

YSRESULT YsShellLattice::GetIntersectingPolygon(YsArray <YsShellPolygonHandle> &itscPlHd,const YsVec3 &vtPos1,const YsVec3 &vtPos2) const
{
	YsArray <YsShellPolygonHandle,16> cannPlHd;

	itscPlHd.Set(0,NULL);
	if(MakePossibllyCollidingPolygonList(cannPlHd,vtPos1,vtPos2)==YSOK)
	{
		int i;
		YsCollisionOfPolygon collision;
		YsArray <YsVec3> plVtPos;

		forYsArray(i,cannPlHd)
		{
			shl->GetVertexListOfPolygon(plVtPos,cannPlHd[i]);
			collision.SetPolygon1(plVtPos.GetN(),plVtPos);

			if(collision.CheckIntersectionBetweenPolygon1AndLineSegment(vtPos1,vtPos2)==YSTRUE)
			{
				itscPlHd.Append(cannPlHd[i]);
			}
		}

		return YSOK;
	}
	return YSERR;
}


YSRESULT YsShellLattice::GetIntersectingPolygon(YsArray <YsShellPolygonHandle> &itscPlHd,YsShellVertexHandle edVtHd1,YsShellVertexHandle edVtHd2) const
{
	YsVec3 edVtPos[2];
	shl->GetVertexPosition(edVtPos[0],edVtHd1);
	shl->GetVertexPosition(edVtPos[1],edVtHd2);

	if(GetIntersectingPolygon(itscPlHd,edVtPos[0],edVtPos[1])==YSOK)
	{
		int i,j;
		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		forYsArrayRev(i,itscPlHd)
		{
			shl->GetVertexListOfPolygon(nPlVt,plVtHd,itscPlHd[i]);

			for(j=0; j<nPlVt; j++)
			{
				if(plVtHd[j]==edVtHd1 || plVtHd[j]==edVtHd2)
				{
					itscPlHd.DeleteBySwapping(j);
					break;
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::FillUndeterminedBlock(int bx,int by,int bz,YSSIDE side)
{
	int n;
	YsArray <YsFixedLengthArray <int,3>,4096> *todo;
	todo=new YsArray <YsFixedLengthArray <int,3>,4096>;
	if(todo!=NULL)
	{
		YsFixedLengthArray <int,3> b,nei;
		b.dat[0]=bx;
		b.dat[1]=by;
		b.dat[2]=bz;
		todo->AppendItem(b);

		while((n=todo->GetNumItem())>0)
		{
			int x,y,z;
			YsShellLatticeElem *elm;

			x=(*todo)[n-1].dat[0];
			y=(*todo)[n-1].dat[1];
			z=(*todo)[n-1].dat[2];
			todo->Delete(n-1);

			elm=GetEditableBlock(x,y,z);
			if(elm!=NULL && elm->side==YSUNKNOWNSIDE)
			{
				elm->side=side;

				nei.dat[0]=x+1;  nei.dat[1]=y  ;  nei.dat[2]=z  ;
				todo->Append(nei);
				nei.dat[0]=x-1;  nei.dat[1]=y  ;  nei.dat[2]=z  ;
				todo->Append(nei);
				nei.dat[0]=x  ;  nei.dat[1]=y+1;  nei.dat[2]=z  ;
				todo->Append(nei);
				nei.dat[0]=x  ;  nei.dat[1]=y-1;  nei.dat[2]=z  ;
				todo->Append(nei);
				nei.dat[0]=x  ;  nei.dat[1]=y  ;  nei.dat[2]=z+1;
				todo->Append(nei);
				nei.dat[0]=x  ;  nei.dat[1]=y  ;  nei.dat[2]=z-1;
				todo->Append(nei);
			}
		}

		delete todo;
		return YSOK;
	}
	else
	{
		return RecursivelyFillUndeterminedBlock(bx,by,bz,side);
	}
}

YSRESULT YsShellLattice::RecursivelyFillUndeterminedBlock(int bx,int by,int bz,YSSIDE side,int limitter)
{
	if(limitter>0)
	{
		YsShellLatticeElem *elm;
		elm=GetEditableBlock(bx,by,bz);
		if(elm!=NULL && elm->side==YSUNKNOWNSIDE)
		{
			elm->side=side;
			RecursivelyFillUndeterminedBlock(bx+1,by  ,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx-1,by  ,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by+1,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by-1,bz  ,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by  ,bz+1,side,limitter-1);
			RecursivelyFillUndeterminedBlock(bx  ,by  ,bz-1,side,limitter-1);
		}
	}
	return YSOK;
}

YSSIDE YsShellLattice::SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary
    (const YsVec3 &pnt,YSBOOL showDetail) const
{
	// Idea : find nearest cell that is marked as inside or outside,
	//        shoot finite ray from pnt to the center of that cell,
	//        count number of intersection.

	int searchRange,searchRangeLimit;
	int i,bx,by,bz,cx,cy,cz;
	const YsShellLatticeElem *elm;

	GetBlockIndex(bx,by,bz,pnt);


	// But, first check if it is on boundary.
	elm=GetBlock(bx,by,bz);
	for(i=0; i<elm->plgList.GetNumItem(); i++)
	{
		YsArray <YsVec3> plg;
		YsShellPolygonHandle plHd;
		YsPlane pln;

		plHd=elm->plgList[i];
		if(shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
		{
			YsVec3 prj;

			pln.MakeBestFitPlane(plg.GetNumItem(),plg.GetArray());
			if(pln.CheckOnPlane(pnt)==YSTRUE)
			{
				YSSIDE side;
				side=YsCheckInsidePolygon3(pnt,plg.GetNumItem(),plg.GetArray());
				if(side==YSBOUNDARY || side==YSINSIDE)
				{
					return YSBOUNDARY;
				}
			}
		}
	}


	searchRangeLimit=GetNumBlockX();
	searchRangeLimit=YsGreater(GetNumBlockY(),searchRangeLimit);
	searchRangeLimit=YsGreater(GetNumBlockZ(),searchRangeLimit);

	int failCount;
	failCount=0;
	for(searchRange=1; searchRange<searchRangeLimit && failCount<10; searchRange++)
	{
		int a,b;

		for(a=-searchRange; a<=searchRange && failCount<10; a++)
		{
			for(b=-searchRange; b<=searchRange && failCount<10; b++)
			{
				for(i=0; i<6; i++)
				{
					switch(i)
					{
					case 0:
						cx=bx+a;
						cy=by+b;
						cz=bz-searchRange;
						break;
					case 1:
						cx=bx+a;
						cy=by+b;
						cz=bz+searchRange;
						break;
					case 2:
						cx=bx+a;
						cy=by-searchRange;
						cz=bz+b;
						break;
					case 3:
						cx=bx+a;
						cy=by+searchRange;
						cz=bz+b;
						break;
					case 4:
						cx=bx-searchRange;
						cy=by+a;
						cz=bz+b;
						break;
					case 5:
						cx=bx+searchRange;
						cy=by+a;
						cz=bz+b;
						break;
					}

					elm=GetBlock(cx,cy,cz);
					if(elm!=NULL && (elm->side==YSINSIDE || elm->side==YSOUTSIDE))
					{
						YSSIDE side;
						side=ShootFiniteRay(pnt,bx,by,bz,cx,cy,cz,elm->side);
						if(side!=YSUNKNOWNSIDE)
						{
							return side;
						}
						else if((++failCount)>=10)
						{
							YsErrOut("Failed 10 times.  Giving Up.\n");
							break;
						}
					}
				}
			}
		}
	}
	YsErrOut("YsShellLattice::SophisticatedCheckInsideSolidWhenTheCellIsMarkedAsBoundary()\n");
	YsErrOut("  Reached dead end!\n");
	return YSUNKNOWNSIDE;
}

YSSIDE YsShellLattice::ShootFiniteRay(const YsVec3 &pnt,int bx,int by,int bz,int cx,int cy,int cz,YSSIDE sideAtC) const
{
	// Idea : Shoot a finite ray from pnt to center of cx,cy,cz
	//        Count intersection.
	//        If even, return sideAtC
	//        If odd, return (sideAtC==YSINSIDE ? YSOUTSIDE : YSINSIDE)

	YsArray <YsShellPolygonHandle,16> plgList;
	if(MakeListOfPolygonsThatGoThroughTheBlockRegion(plgList,bx,by,bz,cx,cy,cz)==YSOK)
	{
		int i,nIntersection;
		YsArray <YsVec3> plg;
		YsVec3 blk1,blk2,rayTarget;
		YsShellPolygonHandle plHd;

		GetBlockRange(blk1,blk2,cx,cy,cz);
		rayTarget=(blk1+blk2)/2.0;


		if(checkInsideSolidLog==YSTRUE)
		{
			ray1=pnt;
			ray2=rayTarget;
		}

		nIntersection=0;
		for(i=0; i<plgList.GetNumItem(); i++)
		{
			plHd=plgList[i];
			if(shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
			{
				YSBOOL hit;
				YsVec3 crs;
				if(CalculatePolygonRayIntersection(hit,crs,plg.GetN(),plg,pnt,rayTarget)==YSOK)
				{
					if(hit==YSTRUE)
					{
						if(checkInsideSolidLog==YSTRUE)
						{
							foundIntersection.AppendItem(crs);
							foundPolygon.AppendItem(plHd);
						}
						nIntersection++;
					}
				}
				else
				{
					return YSUNKNOWNSIDE;
				}
			}
			else
			{
				YsErrOut("YsShellLattice::ShootFiniteRay()\n");
				YsErrOut("  Cannot get polygon vertices\n");
				return YSUNKNOWNSIDE;
			}
		}

		if((nIntersection%2)==0)
		{
			return sideAtC;
		}
		else
		{
			return (sideAtC==YSINSIDE ? YSOUTSIDE : YSINSIDE);
		}
	}

	return YSUNKNOWNSIDE;
}

YSRESULT YsShellLattice::MakeListOfPolygonsThatGoThroughTheBlockRegion
    (YsArray <YsShellPolygonHandle,16> &plgList,int x1,int y1,int z1,int x2,int y2,int z2) const
{
	int bx,by,bz,bx1,by1,bz1,bx2,by2,bz2;
	int i;
	const YsShellLatticeElem *elm;
	YsShellPolygonHandle visitTest;
	YsHashTable <YsShellPolygonHandle> visited(255);

	bx1=YsSmaller(x1,x2);
	by1=YsSmaller(y1,y2);
	bz1=YsSmaller(z1,z2);
	bx2=YsGreater(x1,x2);
	by2=YsGreater(y1,y2);
	bz2=YsGreater(z1,z2);

	plgList.Set(0,NULL);
	for(bx=bx1; bx<=bx2; bx++)
	{
		for(by=by1; by<=by2; by++)
		{
			for(bz=bz1; bz<=bz2; bz++)
			{
				elm=GetBlock(bx,by,bz);
				if(elm!=NULL)
				{
					for(i=0; i<elm->plgList.GetN(); i++)
					{
						if(visited.FindElement(visitTest,shl->GetSearchKey(elm->plgList[i]))!=YSOK)
						{
							visited.AddElement(shl->GetSearchKey(elm->plgList[i]),elm->plgList[i]);
							plgList.Append(elm->plgList[i]);
						}
					}
				}
				else
				{
					YsErrOut("YsShellLattice::MakeListOfPolygonsThatGoThroughTheBlockRegion()\n");
					YsErrOut("  Block Index Out Of Range %d %d %d\n",bx,by,bz);
				}
			}
		}
	}

	return YSOK;
}

YSSIDE YsShellLattice::ShootRay
(YsArray <YsVec3,16> &itscPos,YsArray <YsShellPolygonHandle,16> &itscPlHd,
 const YsVec3 &org,const YsVec3 &vec,YSBOOL watch) const
{
	itscPos.Set(0,NULL);
	itscPlHd.Set(0,NULL);

	double l;
	const YsVec3 &p1=org;
	YsVec3 p2;
	YsArray <unsigned,64> blkList;
	YsHashTable <YsShellPolygonHandle> visited(255);
	YsShellPolygonHandle tst;
	YsArray <YsVec3> plg;
	YSBOOL hitEdge;

	l=(max-min).GetLength();
	p2=vec;
	hitEdge=YSFALSE;
	if(p2.Normalize()==YSOK)
	{
		p2=org+p2*(l*2.0);

		if(MakeIntersectingBlockList(blkList,p1,p2)==YSOK)
		{
			int b,p;
			int itscCount;
			itscCount=0;
			for(b=0; b<=blkList.GetN()-3; b+=3)
			{
				const YsShellLatticeElem *elm;
				elm=GetBlock(blkList[b],blkList[b+1],blkList[b+2]);
				for(p=0; p<elm->plgList.GetN(); p++)
				{
					if(visited.FindElement(tst,shl->GetSearchKey(elm->plgList[p]))!=YSOK)
					{
						if(watch==YSTRUE)
						{
							printf("PlId %d\n",shl->GetPolygonIdFromHandle(elm->plgList[p]));
						}


						YSBOOL hit;
						YsVec3 itsc;
						visited.AddElement(shl->GetSearchKey(elm->plgList[p]),elm->plgList[p]);
						shl->GetVertexListOfPolygon(plg,elm->plgList[p]);
						if(CalculatePolygonRayIntersection(hit,itsc,plg.GetN(),plg,p1,p2,watch)!=YSOK)
						{
							if(watch==YSTRUE)
							{
								printf("Hit Edge.\n");
							}
							hitEdge=YSTRUE;
						}

						if(watch==YSTRUE)
						{
							printf("Hit=%d\n",hit);
						}

						if(hit==YSTRUE)
						{
							itscPos.Append(itsc);
							itscPlHd.Append(elm->plgList[p]);
							itscCount++;
						}
					}
				}
			}

			if(hitEdge!=YSTRUE)
			{
				if(itscCount%2==0)
				{
					return YSINSIDE;
				}
				else
				{
					return YSOUTSIDE;
				}
			}
			
		}
	}
	return YSUNKNOWNSIDE;
}

YSRESULT YsShellLattice::CalculatePolygonRayIntersection
	(YSBOOL &hit,YsVec3 &itsc,int np,const YsVec3 p[],const YsVec3 &ray1,const YsVec3 &ray2,YSBOOL watch) const
{
	hit=YSFALSE;

	if(np==0)
	{
		return YSERR;
	}

	int j;
	int nForward;
	nForward=0;
	for(j=0; j<np; j++)
	{
		if((p[j]-ray1)*(ray2-ray1)>-YsTolerance)
		{
			nForward++;
			break;
		}
	}

	if(watch==YSTRUE)
	{
		printf("nForward=%d\n",nForward);
	}

	if(nForward>0)
	{
		YsPlane pln;
		if(pln.MakeBestFitPlane(np,p)==YSOK)
		{
			if(pln.GetPenetration(itsc,ray1,ray2)==YSOK)
			{
				YSSIDE side;
			 	side=YsCheckInsidePolygon3(itsc,np,p);

				if(watch==YSTRUE)
				{
					printf("itsc %s side %d\n",itsc.Txt(),side);
				}

			 	if(side==YSBOUNDARY)
			 	{
					hit=YSTRUE;
					return YSERR;
				}
				else if(side==YSINSIDE)
				{
					hit=YSTRUE;
					return YSOK;
				}
				else
				{
					hit=YSFALSE;
					return YSOK;
				}
			}
		}
		else
		{
			// It's a bad polygon, but it is ok as long as it is far from the ray.
			int m;
			YsVec3 p1,p2,neaP,neaQ;
			YSRESULT err;
			err=YSOK;
			p2=p[np-1];
			for(m=0; m<np; m++)
			{
				p1=p2;
				p2=p[m];
				if(p1==ray1 || p1==ray2 || p2==ray1 || p2==ray2)
				{
					err=YSERR;
					break;
				}
				if(YsGetNearestPointOfTwoLine(neaP,neaQ,p1,p2,ray1,ray2)==YSOK && neaP==neaQ)
				{
					err=YSERR;
					break;
				}
			}
			if(err!=YSOK)
			{
				YsErrOut("YsShellLattice::CalculatePolygonRayIntersection()\n");
				YsErrOut("  The ray hits a bad polygon\n");
				return YSERR;
			}
		}
	}
	return YSOK;
}

YsShellPolygonHandle YsShellLattice::FindNearestPolygon(
    double &nearDist,YsVec3 &nearPos,const YsVec3 &from,YSBOOL att4Test,int att4Value) const
{
	int x0,y0,z0;
	YsHashTable <int> visited(16);
	visited.EnableAutoResizing(16,256);

	YSBOOL first;
	YsShellPolygonHandle nearPlHd;
	YsArray <YsVec3,16> plVtPos;
	double lBlkMin;

	YSBOOL watch;

	watch=YSFALSE;

	lBlkMin=YsSmallestOf(lxBlk,lyBlk,lzBlk);

	nearPlHd=NULL;
	nearDist=YsInfinity*YsInfinity;
	first=YSTRUE;

	int range,range0,searchRange;

	if(GetBlockIndex(x0,y0,z0,from)==YSOK)
	{
		range0=0;
		searchRange=YsGreatestOf(nBlkX,nBlkY,nBlkZ);
	}
	else
	{
		int dx,dy,dz,lx,ly,lz;

		GetBlockIndexUnconstrained(x0,y0,z0,from);

		dx=(x0<0 ? -x0 : x0-GetNumBlockX()+1);
		dy=(y0<0 ? -y0 : y0-GetNumBlockY()+1);
		dz=(z0<0 ? -z0 : z0-GetNumBlockZ()+1);
		range0=YsGreatestOf(dx,dy,dz);

		lx=dx+nBlkX;
		ly=dy+nBlkY;
		lz=dz+nBlkZ;
		searchRange=YsGreatestOf(lx,ly,lz);
	}

	int xMin,xMax,yMin,yMax,zMin,zMax;
	xMin=0;
	yMin=0;
	zMin=0;
	xMax=nBlkX-1;
	yMax=nBlkY-1;
	zMax=nBlkZ-1;

	for(range=range0; range<=searchRange; range++)  // This must be <=   See memo/20061208.doc
	{
		int i,j,pln,du,dv,bx,by,bz;

		if(watch==YSTRUE)
		{
			YsPrintf("Range %d/%d\n",range,searchRange);
		}

		for(du=-range; du<=range; du++)
		{
			for(dv=-range; dv<=range; dv++)
			{
				for(pln=0; pln<6; pln++)
				{
					switch(pln)
					{
					case 0:
						bx=x0-range;
						by=y0+du;
						bz=z0+dv;
						break;
					case 1:
						bx=x0+range;
						by=y0+du;
						bz=z0+dv;
						break;
					case 2:
						bx=x0+du;
						by=y0+range;
						bz=z0+dv;
						break;
					case 3:
						bx=x0+du;
						by=y0-range;
						bz=z0+dv;
						break;
					case 4:
						bx=x0+du;
						by=y0+dv;
						bz=z0+range;
						break;
					case 5:
						bx=x0+du;
						by=y0+dv;
						bz=z0-range;
						break;
					}

					// dx=YsAbs(bx-x0)+1;   <- Apparently, it's wrong
					// dy=YsAbs(by-y0)+1;
					// dz=YsAbs(bz-z0)+1;
					// lx=(double)dx*lxBlk;
					// ly=(double)dy*lyBlk;
					// lz=(double)dz*lzBlk;
					// if(nearDist<lx*lx+ly*ly+lz*lz)
					// {
					// 	continue;
					// }

					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL && elm->plgList.GetN()>0)
					{
						int nPlVt;
						const YsShellVertexHandle *plVtHd;
						YsVec3 plNom,vtPos,plCen;
						double dist;
						YsPlane pln;

						for(i=0; i<elm->plgList.GetN(); i++)
						{
							if(att4Test==YSTRUE && shl->GetPolygon(elm->plgList[i])->freeAttribute4!=att4Value)
							{
								continue;
							}

							if(visited.CheckKeyExist(shl->GetSearchKey(elm->plgList[i]))!=YSTRUE)
							{
								YSBOOL hit;
								hit=YSFALSE;

								visited.AddElement(shl->GetSearchKey(elm->plgList[i]),0);

								shl->GetVertexListOfPolygon(nPlVt,plVtHd,elm->plgList[i]);
								shl->GetNormalOfPolygon(plNom,elm->plgList[i]);

								plVtPos.Set(0,NULL);
								plCen=YsOrigin();
								for(j=0; j<nPlVt; j++)
								{
									shl->GetVertexPosition(vtPos,plVtHd[j]);
									dist=(vtPos-from).GetSquareLength();
									plVtPos.Append(vtPos);
									plCen+=vtPos;
									if(first==YSTRUE || dist<nearDist)
									{
										first=YSFALSE;
										hit=YSTRUE;

										nearDist=dist;
										nearPlHd=elm->plgList[i];
										nearPos=vtPos;
									}
								}
								plCen/=(double)nPlVt;

								if(plNom!=YsOrigin())
								{
									pln.Set(plCen,plNom);
								}
								else
								{
									pln.MakeBestFitPlane(nPlVt,plVtPos);
								}

								if(pln.GetNearestPoint(vtPos,from)==YSOK &&
								   ((dist=(vtPos-from).GetSquareLength())<nearDist) || first==YSTRUE)
								{
									YSSIDE side;
									if(nPlVt==3)
									{
										side=YsCheckInsideTriangle3(vtPos,plVtPos);
									}
									else if(plNom!=YsOrigin())
									{
										side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos,plNom);
									}
									else
									{
										side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos);
									}

									if(side==YSINSIDE || side==YSBOUNDARY)
									{
										first=YSFALSE;
										hit=YSTRUE;

										nearPlHd=elm->plgList[i];
										nearPos=vtPos;
										nearDist=dist;
									}
								}

								int prevJ;
								prevJ=nPlVt-1;
								for(j=0; j<nPlVt; j++)
								{
									if(YsGetNearestPointOnLine3(vtPos,plVtPos[prevJ],plVtPos[j],from)==YSOK &&
									   YsCheckInBetween3(vtPos,plVtPos[prevJ],plVtPos[j])==YSTRUE &&
									   ((dist=(vtPos-from).GetSquareLength())<nearDist) || first==YSTRUE)
									{
										first=YSFALSE;
										hit=YSTRUE;

										nearPlHd=elm->plgList[i];
										nearPos=vtPos;
										nearDist=dist;
									}
									prevJ=j;
								}

								if(hit==YSTRUE)
								{
									YsVec3 dif;
									dif=nearPos-from;
									dist=dif.GetLength();

									int rangeX,rangeY,rangeZ,newRange;
									rangeX=YsSmaller(1+(int)(dist/lxBlk),nBlkX);
									rangeY=YsSmaller(1+(int)(dist/lyBlk),nBlkY);
									rangeZ=YsSmaller(1+(int)(dist/lzBlk),nBlkZ);
									if(xyPlane==YSTRUE)
									{
										rangeZ=0;
									}
									if(xzPlane==YSTRUE)
									{
										rangeY=0;
									}
									if(yzPlane==YSTRUE)
									{
										rangeX=0;
									}
									newRange=YsGreatestOf(rangeX,rangeY,rangeZ);

									searchRange=YsSmaller(searchRange,newRange);

									if(watch==YSTRUE)
									{
										YsPrintf("Search Range Updated to %d\n",newRange);
									}

									// dist/=lBlkMin;
									// searchRange=YsSmaller(searchRange,1+(int)dist);
								}
							}
						}
					}

					if(range==0)
					{
						break;
					}
				}
			}
		}
	}

	if(nearPlHd!=NULL)  // 2007/07/05
	{
		nearDist=sqrt(nearDist);
	}

	return nearPlHd;
}

YSRESULT YsShellLattice::FindNearestEdgePiece
    (YsShellVertexHandle nearEdVtHd[2],unsigned int &nearCeKey,double &nearDist,YsVec3 &nearPos,const YsVec3 &from,
     const YsVec3 &dissimilarConst) const
{
	int x0,y0,z0;
	YsFixedLengthToMultiHashTable <int,2,1> visited(16);
	visited.EnableAutoResizing(16,256);

	nearEdVtHd[0]=NULL;
	nearEdVtHd[1]=NULL;
	nearCeKey=~0;

	if(GetBlockIndex(x0,y0,z0,from)!=YSOK)
	{
		YsErrOut("YsShellLattice::FindNearestEdgePiece()\n");
		YsErrOut("  Only support a point inside the bounding box.\n");
		nearDist=YsInfinity;
		return YSERR;
	}

	YSBOOL testDissimilarity;
	testDissimilarity=(dissimilarConst!=YsOrigin() ? YSTRUE : YSFALSE);

	YsVec3 edVtPos[2];
	double lBlkMin;
	lBlkMin=YsSmallestOf(lxBlk,lyBlk,lzBlk);

	nearDist=YsInfinity*YsInfinity;

	int range,searchRange;
	searchRange=YsGreatestOf(nBlkX,nBlkY,nBlkZ);

	for(range=0; range<=searchRange; range++)  // This must be <=   See memo/20061208.doc
	{
		int i,j,pln,du,dv,du0,du1,dv0,dv1,bx,by,bz;
		int *uPtr,*vPtr;
		// double dx,dy,dz,lx,ly,lz;
		for(pln=0; pln<6; pln++)
		{
			du0=-range;
			du1= range;
			dv0=-range;
			dv1= range;
			switch(pln)
			{
			case 0:  // bx=x0-range  U->y  V->z
				if(x0-range<0)
				{
					continue;
				}
				du0=YsGreater(0    ,y0-range);
				du1=YsSmaller(nBlkY,y0+range);
				dv0=YsGreater(0    ,z0-range);
				dv1=YsSmaller(nBlkZ,z0+range);
				bx=x0-range;
				uPtr=&by;
				vPtr=&bz;
				break;
			case 1:  // bx=x0+range
				if(nBlkX<x0+range)
				{
					continue;
				}
				du0=YsGreater(0    ,y0-range);
				du1=YsSmaller(nBlkY,y0+range);
				dv0=YsGreater(0    ,z0-range);
				dv1=YsSmaller(nBlkZ,z0+range);
				bx=x0+range;
				uPtr=&by;
				vPtr=&bz;
				break;
			case 2:  // by=y0+range  U->X  V->Z
				if(nBlkY<y0+range)
				{
					continue;
				}
				du0=YsGreater(0    ,x0-range);
				du1=YsSmaller(nBlkX,x0+range);
				dv0=YsGreater(0    ,z0-range);
				dv1=YsSmaller(nBlkZ,z0+range);
				by=y0+range;
				uPtr=&bx;
				vPtr=&bz;
				break;
			case 3:  // by=y0-range
				if(y0-range<0)
				{
					continue;
				}
				du0=YsGreater(0    ,x0-range);
				du1=YsSmaller(nBlkX,x0+range);
				dv0=YsGreater(0    ,z0-range);
				dv1=YsSmaller(nBlkZ,z0+range);
				by=y0-range;
				uPtr=&bx;
				vPtr=&bz;
				break;
			case 4:  // bz=z0+range  U->X  V->Y
				if(nBlkZ<z0+range)
				{
					continue;
				}
				du0=YsGreater(0    ,x0-range);
				du1=YsSmaller(nBlkX,x0+range);
				dv0=YsGreater(0    ,y0-range);
				dv1=YsSmaller(nBlkY,y0+range);
				bz=z0+range;
				uPtr=&bx;
				vPtr=&by;
				break;
			case 5:  // bz=z0-range
				if(z0-range<0)
				{
					continue;
				}
				du0=YsGreater(0    ,x0-range);
				du1=YsSmaller(nBlkX,x0+range);
				dv0=YsGreater(0    ,y0-range);
				dv1=YsSmaller(nBlkY,y0+range);
				bz=z0-range;
				uPtr=&bx;
				vPtr=&by;
				break;
			}

			for(du=du0; du<=du1; du++)
			{
				for(dv=dv0; dv<=dv1; dv++)
				{
//					switch(pln)
//					{
//					case 0:
//						bx=x0-range;
//						by=du;
//						bz=dv;
//						break;
//					case 1:
//						bx=x0+range;
//						by=du;
//						bz=dv;
//						break;
//					case 2:
//						bx=du;
//						by=y0+range;
//						bz=dv;
//						break;
//					case 3:
//						bx=du;
//						by=y0-range;
//						bz=dv;
//						break;
//					case 4:
//						bx=du;
//						by=dv;
//						bz=z0+range;
//						break;
//					case 5:
//						bx=du;
//						by=dv;
//						bz=z0-range;
//						break;
//					}

					*uPtr=du;
					*vPtr=dv;


					//dx=YsAbs(bx-x0);
					//dy=YsAbs(by-y0);
					//dz=YsAbs(bz-z0);
					//lx=(double)dx*lxBlk;
					//ly=(double)dy*lyBlk;
					//lz=(double)dz*lzBlk;
					//if(nearDist<lx*lx+ly*ly+lz*lz)
					//{
					//	continue;
					//}



					const YsShellLatticeElem *elm;
					elm=GetBlock(bx,by,bz);
					if(elm!=NULL && elm->plgList.GetN()>0)
					{
						double dist;

						for(i=0; i<elm->edgList.GetN(); i++)
						{
							unsigned edVtKey[2];
							edVtKey[0]=shl->GetSearchKey(elm->edgList[i].edVtHd[0]);
							edVtKey[1]=shl->GetSearchKey(elm->edgList[i].edVtHd[1]);

							if(visited.CheckKeyExist(2,edVtKey)!=YSTRUE)
							{
								YSBOOL hit;
								hit=YSFALSE;

								visited.AddElement(2,edVtKey,0);

								shl->GetVertexPosition(edVtPos[0],elm->edgList[i].edVtHd[0]);
								shl->GetVertexPosition(edVtPos[1],elm->edgList[i].edVtHd[1]);

								if(testDissimilarity==YSTRUE)
								{
									YsVec3 vec;
									vec=edVtPos[1]-edVtPos[0];
									if(vec.Normalize()!=YSOK || fabs(vec*dissimilarConst)>0.707107)
									{
										continue;  // next i
									}
								}

								for(j=0; j<2; j++)
								{
									dist=(edVtPos[j]-from).GetSquareLength();
									if(nearEdVtHd[0]==NULL || dist<nearDist)
									{
										hit=YSTRUE;

										nearDist=dist;
										nearEdVtHd[0]=elm->edgList[i].edVtHd[0];
										nearEdVtHd[1]=elm->edgList[i].edVtHd[1];
										nearCeKey=elm->edgList[i].ceKey;
										nearPos=edVtPos[j];
									}
								}

								YsVec3 tst;
								if(YsGetNearestPointOnLine3(tst,edVtPos[0],edVtPos[1],from)==YSOK &&
								   YsCheckInBetween3(tst,edVtPos[0],edVtPos[1])==YSTRUE &&
								   ((dist=(tst-from).GetSquareLength())<nearDist) || nearEdVtHd[0]==NULL)
								{
									hit=YSTRUE;
									nearDist=dist;
									nearEdVtHd[0]=elm->edgList[i].edVtHd[0];
									nearEdVtHd[1]=elm->edgList[i].edVtHd[1];

									nearCeKey=elm->edgList[i].ceKey;
									nearPos=tst;
								}

								if(hit==YSTRUE)
								{
									YsVec3 dif;
									dif=nearPos-from;
									dist=dif.GetLength();

									int rangeX,rangeY,rangeZ,newRange;
									rangeX=YsSmaller(1+(int)(dist/lxBlk),nBlkX);
									rangeY=YsSmaller(1+(int)(dist/lyBlk),nBlkY);
									rangeZ=YsSmaller(1+(int)(dist/lzBlk),nBlkZ);
									if(xyPlane==YSTRUE)
									{
										rangeZ=0;
									}
									if(xzPlane==YSTRUE)
									{
										rangeY=0;
									}
									if(yzPlane==YSTRUE)
									{
										rangeX=0;
									}
									newRange=YsGreatestOf(rangeX,rangeY,rangeZ);

									searchRange=YsSmaller(searchRange,newRange);

									// dist/=lBlkMin;
									// searchRange=YsSmaller(searchRange,1+(int)dist);
								}
							}
						}
					}

					if(range==0)
					{
						break;
					}
				}
			}
		}
	}

	if(nearEdVtHd[0]!=NULL)
	{
		nearDist=sqrt(nearDist);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellLattice::PreparePolygon
    (YsArray <YsVec3> &plg,int &bx1,int &by1,int &bz1,int &bx2,int &by2,int &bz2,YsShellPolygonHandle plHd)
{
	int i;
	YsVec3 bbxMin,bbxMax;

	// Get Vertex List
	if(shl->GetVertexListOfPolygon(plg,plHd)==YSOK)
	{
		// Get Bounding Box
		YsBoundingBoxMaker3 bbx;
		bbx.Begin(plg.GetItem(0));
		for(i=1; i<plg.GetNumItem(); i++)
		{
			bbx.Add(plg[i]);
		}

		bbx.Get(bbxMin,bbxMax);

		// Get Block Index Range of Bounding Box
		if(GetBlockIndexAutoBound(bx1,by1,bz1,bbxMin)==YSOK &&
		   GetBlockIndexAutoBound(bx2,by2,bz2,bbxMax)==YSOK)
		{
			return YSOK;
		}
	}
	return YSERR;
}

