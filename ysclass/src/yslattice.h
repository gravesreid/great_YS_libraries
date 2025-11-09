#ifndef YSLATTICE_IS_INCLUDED
#define YSLATTICE_IS_INCLUDED
/* { */

#include "yspredefined.h"

template <class T>
class YsLattice2d
{
public:
	YsLattice2d();
	YsLattice2d(const YsLattice2d <T> &from);
	YsLattice2d(int nx,int ny,const YsVec2 &min,const YsVec2 &max);
	~YsLattice2d();

	const YsLattice2d <T> &operator=(const YsLattice2d <T> &from);

	YSRESULT Create(int nBlk,const YsVec2 &min,const YsVec2 &max);
	YSRESULT Create(int nBlkX,int nBlkY,const YsVec2 &min,const YsVec2 &max);

	YSRESULT GetBlockIndex(int &x,int &y,const YsVec2 &pos) const;
	YSRESULT GetBlockIndexAutoBound(int &x,int &y,const YsVec2 &pos) const;
	T *GetEditableBlock(int x,int y);
	const T *GetBlock(int x,int y) const;
	void GetBlockRange(YsVec2 &mn,YsVec2 &mx,int bx,int by) const;

	YSRESULT SetBlock(int x,int y,const T &blk);

	int GetNumBlockX(void) const;
	int GetNumBlockY(void) const;

	void GetLatticeRange(YsVec2 &min,YsVec2 &max) const;

protected:
	YSRESULT Release(void);

	YsVec2 min,max;
	int nBlkX,nBlkY;
	double lxBlk,lyBlk;
	T *blk;
};

template <class T>
YsLattice2d <T>::YsLattice2d()
{
	nBlkX=0;
	nBlkY=0;
	lxBlk=0.0;
	lyBlk=0.0;
	min.Set(0.0,0.0);
	max.Set(0.0,0.0);
	blk=NULL;
}

template <class T>
YsLattice2d <T>::YsLattice2d(const YsLattice2d <T> &from)
{
	*this=from;
}

template <class T>
YsLattice2d <T>::YsLattice2d(int nx,int ny,const YsVec2 &mn,const YsVec2 &mx)
{
	nBlkX=0;
	nBlkY=0;
	lxBlk=0.0;
	lyBlk=0.0;
	min.Set(0.0,0.0);
	max.Set(0.0,0.0);
	blk=NULL;  // <- Necessary because Create function will call Release function
	Create(nx,ny,mn,mx);
}

template <class T>
YsLattice2d <T>::~YsLattice2d()
{
	Release();
}


template <class T>
const YsLattice2d <T> &YsLattice2d <T>::operator=(const YsLattice2d <T> &from)
{
	Release();

	nBlkX=from.nBlkX;
	nBlkY=from.nBlkY;

	lxBlk=from.lxBlk;
	lyBlk=from.lyBlk;

	min=from.min;
	max=from.max;

	blk=new T [(nBlkX+1)*(nBlkY+1)];

	int i;
	for(i=0; i<(nBlkX+1)*(nBlkY+1); i++)
	{
		blk[i]=from.blk[i];
	}

	return *this;
}


template <class T>
YSRESULT YsLattice2d <T>::Create(int nBlk,const YsVec2 &min,const YsVec2 &max)
{
	int nx,ny;
	double area,lBlk,lx,ly;

	lx=fabs(max.x()-min.x());
	ly=fabs(max.y()-min.y());

	area=fabs(lx*ly)/(double)nBlk;

	lBlk=sqrt(area);

	nx=YsGreater(1,(int)(lx/lBlk));
	ny=YsGreater(1,(int)(ly/lBlk));

	return Create(nx,ny,min,max);
}


template <class T>
YSRESULT YsLattice2d <T>::Create(int nx,int ny,const YsVec2 &mn,const YsVec2 &mx)
{
	Release();

	T *newBlk;
	newBlk=new T [(nx+1)*(ny+1)];

	if(newBlk!=NULL)
	{
		nBlkX=nx;
		nBlkY=ny;

		min.Set(YsSmaller(mn.x(),mx.x()),YsSmaller(mn.y(),mx.y()));
		max.Set(YsGreater(mn.x(),mx.x()),YsGreater(mn.y(),mx.y()));

		lxBlk=(max.x()-min.x())/double(nx);
		lyBlk=(max.y()-min.y())/double(ny);

		blk=newBlk;

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


template <class T>
YSRESULT YsLattice2d <T>::GetBlockIndex(int &x,int &y,const YsVec2 &pos) const
{
	if(min.x()<=pos.x() && pos.x()<max.x() && min.y()<=pos.y() && pos.y()<max.y())
	{
		YsVec2 rel;
		rel=pos-min;

		int bx,by;
		bx=int(rel.x()/lxBlk);
		by=int(rel.y()/lyBlk);

		if(0<=bx && bx<nBlkX && 0<=by && by<nBlkY) // Redundant check
		{
			x=bx;
			y=by;
			return YSOK;
		}
	}
	x=0;
	y=0;
	return YSERR;
}

template <class T>
YSRESULT YsLattice2d <T>::GetBlockIndexAutoBound(int &x,int &y,const YsVec2 &pos) const
{
	YsVec2 bnd;
	bnd.Set(YsBound(pos.x(),min.x()+YsTolerance,max.x()-YsTolerance),
	        YsBound(pos.y(),min.y()+YsTolerance,max.y()-YsTolerance));
	return GetBlockIndex(x,y,bnd);
}

template <class T>
T *YsLattice2d <T>::GetEditableBlock(int x,int y)
{
	return (T *)GetBlock(x,y);
}

template <class T>
const T *YsLattice2d <T>::GetBlock(int x,int y) const
{
	if(0<=x && x<nBlkX && 0<=y && y<nBlkY)
	{
		return &blk[y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
void YsLattice2d <T>::GetBlockRange(YsVec2 &mn,YsVec2 &mx,int bx,int by) const
{
	YsVec2 u,v;
	u.Set(lxBlk,0.0);
	v.Set(0.0,lyBlk);

	mn=min+u*double(bx)+v*double(by);
	mx=mn+u+v;
}


template <class T>
YSRESULT YsLattice2d <T>::SetBlock(int x,int y,const T &blk)
{
	T *desBlk;
	desBlk=GetEditableBlock(x,y);
	if(desBlk!=NULL)
	{
		*desBlk=blk;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


template <class T>
int YsLattice2d <T>::GetNumBlockX(void) const
{
	return nBlkX;
}

template <class T>
int YsLattice2d <T>::GetNumBlockY(void) const
{
	return nBlkY;
}

template <class T>
void YsLattice2d <T>::GetLatticeRange(YsVec2 &n,YsVec2 &x) const
{
	n=min;
	x=max;
}

template <class T>
YSRESULT YsLattice2d <T>::Release(void)
{
	if(blk!=NULL)
	{
		delete [] blk;
		blk=NULL;
	}

	nBlkX=0;
	nBlkY=0;
	lxBlk=0.0;
	lyBlk=0.0;
	min.Set(0.0,0.0);
	max.Set(0.0,0.0);
	blk=NULL;

	return YSOK;
}

////////////////////////////////////////////////////////////

template <class T>
class YsLattice3d
{
public:
	YsLattice3d();
	YsLattice3d(const YsLattice3d <T> &from);
	YsLattice3d(int nx,int ny,int nz,const YsVec3 &min,const YsVec3 &max);
	~YsLattice3d();

	const YsLattice3d <T> &operator=(const YsLattice3d <T> &from);

	YSRESULT Create(int nCell,const YsVec3 &min,const YsVec3 &max);
	YSRESULT Create(int nBlkX,int nBlkY,int nBlkZ,const YsVec3 &min,const YsVec3 &max);

	void CalculateNxNyNzFromNCell(int &nx,int &ny,int &nz,const YsVec3 &min,const YsVec3 &max,int nCell);


	YSRESULT GetBlockIndex(int &x,int &y,int &z,const YsVec3 &pos) const;
	YSRESULT GetBlockIndexUnconstrained(int &x,int &y,int &z,const YsVec3 &pos) const;
	YSRESULT GetBlockIndexRange(int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,int np,const YsVec3 p[]) const;
	YSRESULT GetBlockIndexRange(int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,const YsVec3 &p1,const YsVec3 &p2) const;
	YSRESULT GetBlockIndexAutoBound(int &x,int &y,int &z,const YsVec3 &pos) const;
	T *GetEditableBlock(int x,int y,int z);
	const T *GetBlock(int x,int y,int z) const;
	void GetBlockRange(YsVec3 &mn,YsVec3 &mx,int bx,int by,int bz) const;
	void GetGridPosition(YsVec3 &pos,int bx,int by,int bz) const;

	YSRESULT SetBlock(int x,int y,int z,const T &blk);

	int GetNumBlockX(void) const;
	int GetNumBlockY(void) const;
	int GetNumBlockZ(void) const;
	void GetLatticeRange(YsVec3 &min,YsVec3 &max) const;

	YSRESULT GetIntersectingCellList
	    (YsArray <YsFixedLengthArray <int,3> > &cellLst,const YsVec3 &p1,const YsVec3 &p2) const;

	YSRESULT MakeIntersectingBlockList
	    (YsArray <unsigned,64> &blkList,const YsVec3 &p1,const YsVec3 &p2) const;
	YSRESULT MakeIntersectingBlockListAfterClipping
	    (YsArray <unsigned,64> &blkList,const YsVec3 &p1,const YsVec3 &p2) const;

	YSRESULT MakePotentiallyIntersectingBlockListPolygon
	    (YsArray <unsigned,64> &blkList,int np,const YsVec3 p[]) const;
	YSRESULT MakeIntersectingBlockListPolygon
	    (YsArray <unsigned,64> &blkList,int np,const YsVec3 p[]) const;
	YSRESULT MakeIntersectingBlockListPlane
    	(YsArray <unsigned,64> &blkList,int bx1,int by1,int bz1,int bx2,int by2,int bz2,const YsPlane &pln) const;


protected:
	YSRESULT Release(void);


	YsVec3 min,max;
	int nBlkX,nBlkY,nBlkZ;
	double lxBlk,lyBlk,lzBlk;
	T *blk;
};

template <class T>
YsLattice3d <T>::YsLattice3d()
{
	min.Set(0.0,0.0,0.0);
	max.Set(0.0,0.0,0.0);
	nBlkX=0;
	nBlkY=0;
	nBlkZ=0;
	lxBlk=0.0;
	lyBlk=0.0;
	lzBlk=0.0;
	blk=NULL;
}

template <class T>
YsLattice3d <T>::YsLattice3d(const YsLattice3d <T> &from)
{
	*this=from;
}

template <class T>
YsLattice3d <T>::YsLattice3d(int nx,int ny,int nz,const YsVec3 &mn,const YsVec3 &mx)
{
	YsLattice3d();
	Create(nx,ny,nz,mn,mx);
}

template <class T>
YsLattice3d <T>::~YsLattice3d()
{
	Release();
}

template <class T>
const YsLattice3d <T> &YsLattice3d <T>::operator=(const YsLattice3d <T> &from)
{
	int i,nBlk;
	T *buf;
	nBlk=(from.GetNumBlockX()+1)*(from.GetNumBlockY()+1)*(from.GetNumBlockZ()+1);
	buf=new T [nBlk];
	if(buf!=NULL)
	{
		Release();
		blk=buf;

		for(i=0; i<nBlk; i++)
		{
			blk[i]=from.blk[i];
		}

		min=from.min;
		max=from.max;
		nBlkX=from.nBlkX;
		nBlkY=from.nBlkY;
		nBlkZ=from.nBlkZ;
		lxBlk=from.lxBlk;
		lyBlk=from.lyBlk;
		lzBlk=from.lzBlk;
	}
	else
	{
		YsErrOut("YsLattice3d::operator=()\n");
		YsErrOut("  Low memory warning!\n");
	}

	return *this;
}

template <class T>
YSRESULT YsLattice3d <T>::Create(int nCell,const YsVec3 &mn,const YsVec3 &mx)
{
	YsVec3 dg;
	double vol,cellVol,l;
	int nx,ny,nz;

	dg=mx-mn;
	vol=fabs(dg.x()*dg.y()*dg.z());
	cellVol=vol/(double)nCell;
	l=pow(cellVol,1.0/3.0);

	nx=YsGreater(1,(int)(fabs(dg.x())/l));
	ny=YsGreater(1,(int)(fabs(dg.y())/l));
	nz=YsGreater(1,(int)(fabs(dg.z())/l));

	return Create(nx,ny,nz,mn,mx);
}

template <class T>
void YsLattice3d <T>::CalculateNxNyNzFromNCell(int &nx,int &ny,int &nz,const YsVec3 &min,const YsVec3 &max,int nCell)
{
	double lx,ly,lz,volCell,lCell;
	lx=fabs(max.x()-min.x());
	ly=fabs(max.y()-min.y());
	lz=fabs(max.z()-min.z());

	volCell=(lx*ly*lz)/(double)nCell;
	lCell=pow(volCell,1.0/3.0);

	nx=1+(int)(lx/lCell);
	ny=1+(int)(ly/lCell);
	nz=1+(int)(lz/lCell);
}

template <class T>
YSRESULT YsLattice3d <T>::Create(int nx,int ny,int nz,const YsVec3 &mn,const YsVec3 &mx)
{
	T *buf;
	buf=new T [(nx+1)*(ny+1)*(nz+1)];
	if(buf!=NULL)
	{
		Release();
		blk=buf;
		nBlkX=nx;
		nBlkY=ny;
		nBlkZ=nz;

		YsBoundingBoxMaker3 bbx;
		bbx.Begin(mn);
		bbx.Add(mx);

		YsVec3 trueMin,trueMax,dif;
		bbx.Get(trueMin,trueMax);
		min=trueMin;
		max=trueMax;

		dif=trueMax-trueMin;
		lxBlk=dif.x()/double(nx);
		lyBlk=dif.y()/double(ny);
		lzBlk=dif.z()/double(nz);

		blk=buf;

		return YSOK;
	}
	else
	{
		YsErrOut("YsLattice3d::Create()\n");
		YsErrOut("  Low memory warning!!\n");
		return YSERR;
	}
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndex(int &x,int &y,int &z,const YsVec3 &pos) const
{

	if(min.x()<=pos.x() && pos.x()<max.x() &&
	   min.y()<=pos.y() && pos.y()<max.y() &&
	   min.z()<=pos.z() && pos.z()<max.z())
	{
		YsVec3 rel;
		rel=pos-min;

		int bx,by,bz;
		bx=int(rel.x()/lxBlk);
		by=int(rel.y()/lyBlk);
		bz=int(rel.z()/lzBlk);

		if(0<=bx && bx<nBlkX &&
		   0<=by && by<nBlkY &&
		   0<=bz && bz<nBlkZ) // Redundant check
		{
			x=bx;
			y=by;
			z=bz;
			return YSOK;
		}
		else
		{
			YsErrOut("YsLattice3d::GetBlockIndex()\n");
			YsErrOut("  Failed the secondary screening.\n");
			return YSERR;
		}
	}
	x=0;
	y=0;
	z=0;
	return YSERR;
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexUnconstrained(int &x,int &y,int &z,const YsVec3 &pos) const
{
	YsVec3 rel;
	rel=pos-min;
	x=int(rel.x()/lxBlk);
	y=int(rel.y()/lyBlk);
	z=int(rel.z()/lzBlk);
	return YSOK;
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexRange
    (int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,int np,const YsVec3 p[]) const
{
	int i;
	YsVec3 p1,p2;
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(p[0]);
	for(i=1; i<np; i++)
	{
		bbx.Add(p[i]);
	}
	bbx.Get(p1,p2);

	if(GetBlockIndexAutoBound(x1,y1,z1,p1)==YSOK &&
	   GetBlockIndexAutoBound(x2,y2,z2,p2)==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexRange
    (int &x1,int &y1,int &z1,int &x2,int &y2,int &z2,const YsVec3 &p1,const YsVec3 &p2) const
{
	YsVec3 q1,q2;
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(p1);
	bbx.Add(p2);

	bbx.Get(q1,q2);
	if(GetBlockIndexAutoBound(x1,y1,z1,q1)==YSOK &&
	   GetBlockIndexAutoBound(x2,y2,z2,q2)==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class T>
YSRESULT YsLattice3d <T>::GetBlockIndexAutoBound(int &x,int &y,int &z,const YsVec3 &pos) const
{
	YsVec3 bnd;
	bnd.Set(YsBound(pos.x(),min.x()+YsTolerance,max.x()-YsTolerance),
	        YsBound(pos.y(),min.y()+YsTolerance,max.y()-YsTolerance),
	        YsBound(pos.z(),min.z()+YsTolerance,max.z()-YsTolerance));
	return GetBlockIndex(x,y,z,bnd);
}


template <class T>
T *YsLattice3d <T>::GetEditableBlock(int x,int y,int z)
{
	if(0<=x && x<=nBlkX &&
	   0<=y && y<=nBlkY &&
	   0<=z && z<=nBlkZ)
	{
		return &blk[z*(nBlkX+1)*(nBlkY+1)+y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
const T *YsLattice3d <T>::GetBlock(int x,int y,int z) const
{
	if(0<=x && x<=nBlkX &&
	   0<=y && y<=nBlkY &&
	   0<=z && z<=nBlkZ)
	{
		return &blk[z*(nBlkX+1)*(nBlkY+1)+y*(nBlkX+1)+x];
	}
	else
	{
		return NULL;
	}
}

template <class T>
void YsLattice3d <T>::GetBlockRange(YsVec3 &mn,YsVec3 &mx,int bx,int by,int bz) const
{
	YsVec3 u,v,w;
	u.Set(lxBlk,0.0,0.0);
	v.Set(0.0,lyBlk,0.0);
	w.Set(0.0,0.0,lzBlk);
	mn=min+u*double(bx)+v*double(by)+w*double(bz);
	mx=mn+u+v+w;
}

template <class T>
void YsLattice3d <T>::GetGridPosition(YsVec3 &pos,int bx,int by,int bz) const
{
	YsVec3 u,v,w;
	u.Set(lxBlk,0.0,0.0);
	v.Set(0.0,lyBlk,0.0);
	w.Set(0.0,0.0,lzBlk);
	pos=min+u*double(bx)+v*double(by)+w*double(bz);
}

template <class T>
YSRESULT YsLattice3d <T>::SetBlock(int x,int y,int z,const T &blk)
{
	T *b;
	b=GetEditableBlock(x,y,z);
	if(b!=NULL)
	{
		*b=blk;
		return YSOK;
	}
	else
	{
		YsErrOut("YsLattice3d::SetBlock()\n");
		YsErrOut("  Out of range\n");
		return YSERR;
	}
}

template <class T>
int YsLattice3d <T>::GetNumBlockX(void) const
{
	return nBlkX;
}

template <class T>
int YsLattice3d <T>::GetNumBlockY(void) const
{
	return nBlkY;
}

template <class T>
int YsLattice3d <T>::GetNumBlockZ(void) const
{
	return nBlkZ;
}

template <class T>
void YsLattice3d <T> ::GetLatticeRange(YsVec3 &m1,YsVec3 &m2) const
{
	m1=min;
	m2=max;
}

template <class T>
YSRESULT YsLattice3d <T>::GetIntersectingCellList
	    (YsArray <YsFixedLengthArray <int,3> > &cellLst,const YsVec3 &p1,const YsVec3 &p2) const
{
	// Remark : p1,p2 must be already clipped by external function.
	int bx1,by1,bz1,bx2,by2,bz2,vx,vy,vz;
	int x,y,z,i,j,k;
	YsVec3 blk[2],clip[2];
	YsArray <YsFixedLengthArray <int,3>,32> toLookAt;

	YsFixedLengthArray <int,3> cellCoord;
	GetBlockIndexAutoBound(bx1,by1,bz1,p1);
	GetBlockIndexAutoBound(bx2,by2,bz2,p2);

	vx=bx2-bx1;
	vy=by2-by1;
	vz=bz2-bz1;
	if(vx!=0)
	{
		vx=(vx>0 ? 1 : -1);
	}
	if(vy!=0)
	{
		vy=(vy>0 ? 1 : -1);
	}
	if(vz!=0)
	{
		vz=(vz>0 ? 1 : -1);
	}

	cellLst.Set(0,NULL);

	x=bx1;
	y=by1;
	z=bz1;
	for(i=-1; i<=1; i++)
	{
		for(j=-1; j<=1; j++)
		{
			for(k=-1; k<=1; k++)
			{
				cellCoord.dat[0]=x+i;
				cellCoord.dat[1]=y+j;
				cellCoord.dat[2]=z+k;
				toLookAt.AppendItem(cellCoord);
			}
		}
	}
	while(toLookAt.GetN()>0)
	{
		int i;

		x=toLookAt[0].dat[0];
		y=toLookAt[0].dat[1];
		z=toLookAt[0].dat[2];

		if(0<=x && x<nBlkX && 0<=y && y<nBlkY && 0<=z && z<nBlkZ)
		{
			for(i=cellLst.GetNumItem()-1; i>=0; i--)
			{
				if(cellLst[i].dat[0]==x &&
				   cellLst[i].dat[1]==y &&
				   cellLst[i].dat[2]==z)
				{
					goto NEXTCELL;
				}
			}

			GetBlockRange(blk[0],blk[1],x,y,z);
			if(YsClipInfiniteLine3(clip[0],clip[1],p1,p2-p1,blk[0],blk[1])==YSOK)
			{
				cellLst.AppendItem(toLookAt.GetItem(0));

				cellCoord.dat[0]=x+vx;
				cellCoord.dat[1]=y;
				cellCoord.dat[2]=z;
				toLookAt.Append(cellCoord);
				cellCoord.dat[0]=x;
				cellCoord.dat[1]=y+vy;
				cellCoord.dat[2]=z;
				toLookAt.Append(cellCoord);
				cellCoord.dat[0]=x;
				cellCoord.dat[1]=y;
				cellCoord.dat[2]=z+vz;
				toLookAt.Append(cellCoord);
			}
		}
	NEXTCELL:
		toLookAt.Delete(0);
	}

	return YSOK;
}

template <class T>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockList
	    (YsArray <unsigned,64> &blkList,const YsVec3 &p1,const YsVec3 &p2) const
{
	YsVec3 lat[2],c1,c2;
	GetLatticeRange(lat[0],lat[1]);

	blkList.Set(0,NULL);

	if(YsCheckInsideBoundingBox3(p1,lat[0],lat[1])==YSTRUE &&
	   YsCheckInsideBoundingBox3(p2,lat[0],lat[1])==YSTRUE)
	{
		return MakeIntersectingBlockListAfterClipping(blkList,p1,p2);
	}
	else
	{
		if(YsClipLineSeg3(c1,c2,p1,p2,lat[0],lat[1])==YSOK)
		{
			return MakeIntersectingBlockListAfterClipping(blkList,c1,c2);
		}
	}
	return YSOK;
}

// p1,p2 must be clipped before this function.
template <class T>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockListAfterClipping
    (YsArray <unsigned,64> &blkList,const YsVec3 &p1,const YsVec3 &p2) const
{
	int goAlong,toCalc[2];
	YsVec3 v,o;
	YSBOOL watch;

	watch=YSFALSE;

	blkList.Set(0,NULL);

	// if(p1==YsVec3(26.702956,22.297454,-20.583457))
	// {
	// 	watch=YSTRUE;
	// 	printf("MakeIntersectingBlockListAfterClipping Watch!\n");
	// }

	v=p2-p1;
	if(fabs(v.x())>fabs(v.y()) && fabs(v.x())>fabs(v.z()))  // Proceed along X direction
	{
		goAlong=0;    // Go along X direction.
		toCalc[0]=1;  // Calculate Y range and
		toCalc[1]=2;  // Calculate Z range for each step.
	}
	else if(fabs(v.y())>fabs(v.z()))  // Proceed along Y direction
	{
		goAlong=1;    // Go along Y direction.
		toCalc[0]=0;  // Calculate X range and
		toCalc[1]=2;  // Calculate Z range for each step.
	}
	else // Proceed along Z direction
	{
		goAlong=2;    // Go along Z direction.
		toCalc[0]=0;  // Calculate X range and
		toCalc[1]=1;  // Calculate Y range for each step.
	}

	v[toCalc[0]]/=v[goAlong];
	v[toCalc[1]]/=v[goAlong];
	v[goAlong]=1.0;

	YsBoundingBoxMaker3 makeBbx;
	YsVec3 bbx[2];
	int bIdx[2][3];
	makeBbx.Begin(p1);
	makeBbx.Add(p2);
	makeBbx.Get(bbx[0],bbx[1]);
	GetBlockIndexAutoBound(bIdx[0][0],bIdx[0][1],bIdx[0][2],bbx[0]);
	GetBlockIndexAutoBound(bIdx[1][0],bIdx[1][1],bIdx[1][2],bbx[1]);

	YsVec3 blkStep;
	int nBlk[3];
	nBlk[0]=nBlkX;
	nBlk[1]=nBlkY;
	nBlk[2]=nBlkZ;
	blkStep[goAlong]=(max[goAlong]-min[goAlong])/(double)nBlk[goAlong];
	blkStep[toCalc[0]]=0.0;
	blkStep[toCalc[1]]=0.0;

	// YsVec3 blkMargine;
	// blkMargine=max-min;
	// blkMargine[toCalc[0]]*=0.02/(double)nBlk[toCalc[0]];
	// blkMargine[toCalc[1]]*=0.02/(double)nBlk[toCalc[1]];
	// blkMargine[goAlong]=0.0;

	if(p1[goAlong]<p2[goAlong])
	{
		o=p1;
	}
	else
	{
		o=p2;
	}

	int p,q,r,n,k;
	int lastRange[2][3];
	n=0;
	lastRange[0][0]=-1;
	lastRange[0][1]=-1;
	lastRange[0][2]=-1;
	lastRange[1][0]=-1;
	lastRange[1][1]=-1;
	lastRange[1][2]=-1;
	for(k=bIdx[0][goAlong]; k<=bIdx[1][goAlong]; k++)
	{
		YsVec3 itsc[2];

		itsc[0]=o+(blkStep[goAlong]*(double)n)*v;
		itsc[1]=o+(blkStep[goAlong]*(double)(n+1))*v;

		makeBbx.Begin(itsc[0]);
		makeBbx.Add(itsc[1]);
		makeBbx.Get(bbx[0],bbx[1]);

		// bbx[0]-=blkMargine;
		// bbx[1]+=blkMargine;

		int idx[2][3];
		GetBlockIndexAutoBound(idx[0][0],idx[0][1],idx[0][2],bbx[0]);
		GetBlockIndexAutoBound(idx[1][0],idx[1][1],idx[1][2],bbx[1]);

		for(p=idx[0][0]; p<=idx[1][0]; p++)
		{
			for(q=idx[0][1]; q<=idx[1][1]; q++)
			{
				for(r=idx[0][2]; r<=idx[1][2]; r++)
				{
					if(p<lastRange[0][0] || lastRange[1][0]<p ||
					   q<lastRange[0][1] || lastRange[1][1]<q ||
					   r<lastRange[0][2] || lastRange[1][2]<r)
					{
						blkList.Append(p);
						blkList.Append(q);
						blkList.Append(r);
						if(watch==YSTRUE)
						{
							printf("%d %d %d\n",p,q,r);
						}
					}
				}
			}
		}
		lastRange[0][0]=idx[0][0];
		lastRange[0][1]=idx[0][1];
		lastRange[0][2]=idx[0][2];
		lastRange[1][0]=idx[1][0];
		lastRange[1][1]=idx[1][1];
		lastRange[1][2]=idx[1][2];

		// for(q=idx[0][toCalc[0]]; q<=idx[1][toCalc[0]]; q++)
		// {
		// 	for(r=idx[0][toCalc[1]]; r<=idx[1][toCalc[1]]; r++)
		// 	{
		// 		unsigned c[3];
		// 		c[goAlong]=p;
		// 		c[toCalc[0]]=q;
		// 		c[toCalc[1]]=r;
		// 		blkList.Append(c[0]);
		// 		blkList.Append(c[1]);
		// 		blkList.Append(c[2]);
 		// 		if(watch==YSTRUE)
 		// 		{
 		// 			printf("%d %d %d\n",c[0],c[1],c[2]);
 		// 		}
 		// 	}
 		// }

		n++;
	}
	return YSOK;
}

template <class T>
YSRESULT YsLattice3d <T>::MakePotentiallyIntersectingBlockListPolygon
    (YsArray <unsigned,64> &blkList,int np,const YsVec3 p[]) const
{
	YsPlane pln;
	YsBoundingBoxMaker3 makeBbx;
	YsVec3 bbx[2];
	int bx1,by1,bz1,bx2,by2,bz2;

	makeBbx.Make(np,p);
	makeBbx.Get(bbx[0],bbx[1]);
	bbx[0]-=YsXYZ()*YsTolerance;
	bbx[1]+=YsXYZ()*YsTolerance;
	if(GetBlockIndexRange(bx1,by1,bz1,bx2,by2,bz2,bbx[0],bbx[1])==YSOK)
	{
		double a,b,c,d;
		if(YsFindLeastSquareFittingPlane(a,b,c,d,np,p)==YSOK)
		{
			int i;
			YsVec3 cen,nom;

			cen=p[0];
			for(i=1; i<np; i++)
			{
				cen+=p[i];
			}
			cen/=double(np);
			nom.Set(a,b,c);
			pln.Set(cen,nom);
			return MakeIntersectingBlockListPlane(blkList,bx1,by1,bz1,bx2,by2,bz2,pln);
		}
	}
	return YSERR;
}

template <class T>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockListPolygon
    (YsArray <unsigned,64> &blkList,int np,const YsVec3 p[]) const
{
	YsArray <unsigned,64> potential;
	blkList.Set(0,NULL);
	if(MakePotentiallyIntersectingBlockListPolygon(potential,np,p)==YSOK)
	{
		int i;
		YsCollisionOfPolygon coll;
		coll.SetPolygon1(np,p);
		for(i=0; i<potential.GetN(); i+=3)
		{
			YsVec3 range[2];
			GetBlockRange(range[0],range[1],potential[i],potential[i+1],potential[i+2]);
			if(coll.CheckIntersectionBetweenPolygon1AndBlock(range[0],range[1])==YSTRUE)
			{
				blkList.Append(potential[i]);
				blkList.Append(potential[i+1]);
				blkList.Append(potential[i+2]);
			}
		}

		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT YsLattice3d <T>::MakeIntersectingBlockListPlane
    (YsArray <unsigned,64> &blkList,int bx1,int by1,int bz1,int bx2,int by2,int bz2,const YsPlane &pln) const
{
	int scan1,scan2;
	int ray,b0[3],b1[3];
	YsVec3 rayDir;
	YsArray <double,64> itscLattice;

	blkList.Set(0,NULL);

	if(fabs(pln.GetNormal().x())>fabs(pln.GetNormal().y()) && fabs(pln.GetNormal().x())>fabs(pln.GetNormal().z()))
	{
		ray  =0;
		scan1=1;
		scan2=2;
		rayDir=YsXVec();
	}
	else if(fabs(pln.GetNormal().y())>fabs(pln.GetNormal().z()))
	{
		scan1=0;
		ray  =1;
		scan2=2;
		rayDir=YsYVec();
	}
	else
	{
		scan1=0;
		scan2=1;
		ray  =2;
		rayDir=YsZVec();
	}

	b0[0]=YsSmaller(bx1,bx2);
	b0[1]=YsSmaller(by1,by2);
	b0[2]=YsSmaller(bz1,bz2);

	b1[0]=YsGreater(bx1,bx2);
	b1[1]=YsGreater(by1,by2);
	b1[2]=YsGreater(bz1,bz2);

	int p,q,r,nNode1,nNode2;
	nNode1=(b1[scan1]-b0[scan1]+2);
	nNode2=(b1[scan2]-b0[scan2]+2);
	itscLattice.Set(nNode1*nNode2,NULL);
	for(p=b0[scan1]; p<=b1[scan1]+1; p++)
	{
		for(q=b0[scan2]; q<=b1[scan2]+1; q++)
		{
			int idx[3];
			YsVec3 range[2],itsc;
			idx[scan1]=p;
			idx[scan2]=q;
			idx[ray]=0;

			GetBlockRange(range[0],range[1],idx[0],idx[1],idx[2]);
			if(pln.GetIntersection(itsc,range[0],rayDir)==YSOK)
			{
				itscLattice[(p-b0[scan1])+(q-b0[scan2])*(nNode1)]=itsc[ray];
			}
		}
	}

	double lUnit,lMin,lMax,lBlk[3];
	lBlk[0]=lxBlk;
	lBlk[1]=lyBlk;
	lBlk[2]=lzBlk;
	lUnit=lBlk[ray];
	lMin=min[ray];
	lMax=max[ray];
	for(p=b0[scan1]; p<=b1[scan1]; p++)
	{
		for(q=b0[scan2]; q<=b1[scan2]; q++)
		{
			int r0,r1,idx[3];
			double rangeMin,rangeMax;
			rangeMin=itscLattice[(p-b0[scan1])+(q-b0[scan2])*(nNode1)];
			rangeMax=itscLattice[(p-b0[scan1])+(q-b0[scan2])*(nNode1)];

			rangeMin=YsSmaller(rangeMin,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]  )*(nNode1)]);
			rangeMax=YsGreater(rangeMax,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]  )*(nNode1)]);

			rangeMin=YsSmaller(rangeMin,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]+1)*(nNode1)]);
			rangeMax=YsGreater(rangeMax,itscLattice[(p-b0[scan1]+1)+(q-b0[scan2]+1)*(nNode1)]);

			rangeMin=YsSmaller(rangeMin,itscLattice[(p-b0[scan1]  )+(q-b0[scan2]+1)*(nNode1)]);
			rangeMax=YsGreater(rangeMax,itscLattice[(p-b0[scan1]  )+(q-b0[scan2]+1)*(nNode1)]);

			r0=(int)((rangeMin-YsTolerance-lMin)/lUnit);
			r1=(int)((rangeMax+YsTolerance-lMin)/lUnit);

			for(r=YsGreater(r0,b0[ray]); r<=YsSmaller(r1,b1[ray]); r++)
			{
				idx[scan1]=p;
				idx[scan2]=q;
				idx[ray]=r;
				blkList.Append(idx[0]);
				blkList.Append(idx[1]);
				blkList.Append(idx[2]);
			}
		}
	}

	return YSOK;
}

template <class T>
YSRESULT YsLattice3d <T>::Release(void)
{
	if(blk!=NULL)
	{
		delete [] blk;
		blk=NULL;
	}
	min.Set(0.0,0.0,0.0);
	max.Set(0.0,0.0,0.0);
	nBlkX=0;
	nBlkY=0;
	nBlkZ=0;
	lxBlk=0.0;
	lyBlk=0.0;
	lzBlk=0.0;
	return YSOK;
}

/* } */
#endif
