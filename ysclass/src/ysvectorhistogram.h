#include "ysclass.h"


// See Research Note 09/06/2002

template <class T>
class YsVectorHistogramCell
{
public:
	T dat;
	double weight;
	int nVec;
	int face,u,v;
};



template <class T>
class YsVectorHistogram
{
public:
	YsVectorHistogram();
	~YsVectorHistogram();

	int nCellPerAxis;
	YsVectorHistogramCell <T> *cellArray[6],*buffer;

	YSRESULT Prepare(int nCellPerAxis);
	int GetNumCellPerAxis(void) const;
	YsVectorHistogramCell <T> *GetEditableCell(int face,int u,int v);
	const YsVectorHistogramCell <T> *GetCell(int face,int u,int v) const;
	const YSRESULT GetCellIndexByVector(int &face,int &u,int &v,const YsVec3 &vec) const;

	YSRESULT GetCellQuadCoordinate(YsVec3 coord[4],int face,int u,int v) const;
	YSRESULT GetCellCenterCoordinate(YsVec3 &cen,int face,int u,int v) const;
};


template <class T>
YsVectorHistogram <T>::YsVectorHistogram()
{
	nCellPerAxis=0;
	cellArray[0]=NULL;
	cellArray[1]=NULL;
	cellArray[2]=NULL;
	cellArray[3]=NULL;
	cellArray[4]=NULL;
	cellArray[5]=NULL;
	buffer=NULL;
}


template <class T>
YsVectorHistogram <T>::~YsVectorHistogram()
{
	if(buffer!=NULL)
	{
		delete [] buffer;
	}
}


template <class T>
YSRESULT YsVectorHistogram <T>::Prepare(int ncpa)
{
	int i,u,v;
	nCellPerAxis=ncpa;
	buffer=new YsVectorHistogramCell <T> [ncpa*ncpa*6];

	for(i=0; i<6; i++)
	{
		cellArray[i]=&buffer[ncpa*ncpa*i];
	}

	for(i=0; i<6; i++)
	{
		for(u=0; u<ncpa; u++)
		{
			for(v=0; v<ncpa; v++)
			{
				YsVectorHistogramCell <T> *cell;
				cell=GetEditableCell(i,u,v);
				cell->weight=0.0;
				cell->nVec=0;
				cell->face=i;
				cell->u=u;
				cell->v=v;
			}
		}
	}

	return YSOK;
}

template <class T>
int YsVectorHistogram <T>::GetNumCellPerAxis(void) const
{
	return nCellPerAxis;
}

template <class T>
YsVectorHistogramCell <T> *YsVectorHistogram <T>::GetEditableCell(int face,int u,int v)
{
	const YsVectorHistogramCell <T> *cell;
	cell=GetCell(face,u,v);
	return (YsVectorHistogramCell <T> *)cell;
}

template <class T>
const YsVectorHistogramCell <T> *YsVectorHistogram <T>::GetCell(int face,int u,int v) const
{
	if(0<=u && u<nCellPerAxis && 0<=v && v<nCellPerAxis)
	{
		return &cellArray[face][v*nCellPerAxis+u];
	}
	else if((u<0 || nCellPerAxis<=u) && (v<0 || nCellPerAxis<=v))
	{
		return NULL;
	}
	else if(u==-1)
	{
		switch(face)
		{
		case 0:
			return GetCell(5,v,0);
		case 1:
			return GetCell(5,v,nCellPerAxis-1);
		case 2:
			return GetCell(5,nCellPerAxis-1,v);
		case 3:
			return GetCell(4,nCellPerAxis-1,v);
		case 4:
			return GetCell(5,0,v);
		case 5:
			return GetCell(4,0,v);
		}
	}
	else if(u==nCellPerAxis)
	{
		switch(face)
		{
		case 0:
			return GetCell(3,v,0);
		case 1:
			return GetCell(3,v,nCellPerAxis-1);
		case 2:
			return GetCell(3,nCellPerAxis-1,v);
		case 3:
			return GetCell(2,nCellPerAxis-1,v);
		case 4:
			return GetCell(3,0,v);
		case 5:
			return GetCell(2,0,v);
		}
	}
	else if(v==-1)
	{
		switch(face)
		{
		case 0:
			return GetCell(4,u,0);
		case 1:
			return GetCell(4,u,nCellPerAxis-1);
		case 2:
			return GetCell(0,u,nCellPerAxis-1);
		case 3:
			return GetCell(0,nCellPerAxis-1,u);
		case 4:
			return GetCell(0,u,0);
		case 5:
			return GetCell(0,0,u);
		}
	}
	else if(v==nCellPerAxis)
	{
		switch(face)
		{
		case 0:
			return GetCell(2,u,0);
		case 1:
			return GetCell(2,u,nCellPerAxis-1);
		case 2:
			return GetCell(1,u,nCellPerAxis-1);
		case 3:
			return GetCell(1,nCellPerAxis-1,u);
		case 4:
			return GetCell(1,u,0);
		case 5:
			return GetCell(1,0,u);
		}
	}

	return NULL;
}

template <class T>
const YSRESULT YsVectorHistogram <T>::GetCellIndexByVector(int &face,int &u,int &v,const YsVec3 &vec) const
{
	double ax,ay,az,x,y,z,cellWidth;
	ax=YsAbs(vec.x());
	ay=YsAbs(vec.y());
	az=YsAbs(vec.z());
	x=vec.x();
	y=vec.y();
	z=vec.z();
	cellWidth=2.0/double(nCellPerAxis);
	if(ax>ay && ax>az)  // F5 or F3
	{
		y/=ax;
		z/=ax;
		u=int((z+1.0)/cellWidth);
		v=int((y+1.0)/cellWidth);
		u=YsBound(u,0,nCellPerAxis-1);
		v=YsBound(v,0,nCellPerAxis-1);
		face=(x<0.0 ? 5 : 3);
		return YSOK;
	}
	else if(ay>az) // F1 or F0
	{
		x/=ay;
		z/=ay;
		u=int((x+1.0)/cellWidth);
		v=int((z+1.0)/cellWidth);
		u=YsBound(u,0,nCellPerAxis-1);
		v=YsBound(v,0,nCellPerAxis-1);
		face=(y<0.0 ? 0 : 1);
		return YSOK;
	}
	else // F2 or F4
	{
		x/=az;
		y/=az;
		u=int((x+1.0)/cellWidth);
		v=int((y+1.0)/cellWidth);
		u=YsBound(u,0,nCellPerAxis-1);
		v=YsBound(v,0,nCellPerAxis-1);
		face=(z<0.0 ? 4 : 2);
		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT YsVectorHistogram <T>::GetCellQuadCoordinate(YsVec3 coord[4],int face,int u,int v) const
{
	const YsVectorHistogramCell <T> *cell;
	double cellWidth;

	cellWidth=2.0/double(nCellPerAxis);
	cell=GetCell(face,u,v);
	if(cell!=NULL)
	{
		double du,dv;
		face=cell->face;
		du=double(cell->u);
		dv=double(cell->v);
		switch(face)
		{
		case 0:
			coord[0].Set(-1.0+cellWidth*(du    ),-1.0,-1.0+cellWidth*(dv    ));
			coord[1].Set(-1.0+cellWidth*(du+1.0),-1.0,-1.0+cellWidth*(dv    ));
			coord[2].Set(-1.0+cellWidth*(du+1.0),-1.0,-1.0+cellWidth*(dv+1.0));
			coord[3].Set(-1.0+cellWidth*(du    ),-1.0,-1.0+cellWidth*(dv+1.0));
			break;
		case 1:
			coord[0].Set(-1.0+cellWidth*(du    ), 1.0,-1.0+cellWidth*(dv    ));
			coord[1].Set(-1.0+cellWidth*(du+1.0), 1.0,-1.0+cellWidth*(dv    ));
			coord[2].Set(-1.0+cellWidth*(du+1.0), 1.0,-1.0+cellWidth*(dv+1.0));
			coord[3].Set(-1.0+cellWidth*(du    ), 1.0,-1.0+cellWidth*(dv+1.0));
			break;
		case 2:
			coord[0].Set(-1.0+cellWidth*(du    ),-1.0+cellWidth*(dv    ), 1.0);
			coord[1].Set(-1.0+cellWidth*(du+1.0),-1.0+cellWidth*(dv    ), 1.0);
			coord[2].Set(-1.0+cellWidth*(du+1.0),-1.0+cellWidth*(dv+1.0), 1.0);
			coord[3].Set(-1.0+cellWidth*(du    ),-1.0+cellWidth*(dv+1.0), 1.0);
			break;
		case 3:
			coord[0].Set( 1.0,-1.0+cellWidth*(dv    ),-1.0+cellWidth*(du    ));
			coord[1].Set( 1.0,-1.0+cellWidth*(dv+1.0),-1.0+cellWidth*(du    ));
			coord[2].Set( 1.0,-1.0+cellWidth*(dv+1.0),-1.0+cellWidth*(du+1.0));
			coord[3].Set( 1.0,-1.0+cellWidth*(dv    ),-1.0+cellWidth*(du+1.0));
			break;
		case 4:
			coord[0].Set(-1.0+cellWidth*(du    ),-1.0+cellWidth*(dv    ),-1.0);
			coord[1].Set(-1.0+cellWidth*(du+1.0),-1.0+cellWidth*(dv    ),-1.0);
			coord[2].Set(-1.0+cellWidth*(du+1.0),-1.0+cellWidth*(dv+1.0),-1.0);
			coord[3].Set(-1.0+cellWidth*(du    ),-1.0+cellWidth*(dv+1.0),-1.0);
			break;
		case 5:
			coord[0].Set(-1.0,-1.0+cellWidth*(dv    ),-1.0+cellWidth*(du    ));
			coord[1].Set(-1.0,-1.0+cellWidth*(dv+1.0),-1.0+cellWidth*(du    ));
			coord[2].Set(-1.0,-1.0+cellWidth*(dv+1.0),-1.0+cellWidth*(du+1.0));
			coord[3].Set(-1.0,-1.0+cellWidth*(dv    ),-1.0+cellWidth*(du+1.0));
			break;
		}
		coord[0].Normalize();
		coord[1].Normalize();
		coord[2].Normalize();
		coord[3].Normalize();
		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT YsVectorHistogram <T>::GetCellCenterCoordinate(YsVec3 &cen,int face,int u,int v) const
{
	YsVec3 quad[4];
	if(GetCellQuadCoordinate(quad,face,u,v)==YSOK)
	{
		cen=(quad[0]+quad[1]+quad[2]+quad[3])/4.0;
		cen.Normalize();
		return YSOK;
	}
	return YSERR;
}

