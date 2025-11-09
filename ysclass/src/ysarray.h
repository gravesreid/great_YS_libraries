#ifndef YSARRAY_IS_INCLUDED
#define YSARRAY_IS_INCLUDED
/* { */

#include "ysexception.h"
#include "yssort.h"


template <class T1,class T2>
class YsPair
{
public:
	T1 a;
	T2 b;
};

template <class T1,class T2,class T3>
class YsTrio
{
public:
	T1 a;
	T2 b;
	T3 c;
};

template <class T1,class T2,class T3,class T4>
class YsQuartet
{
public:
	T1 a;
	T2 b;
	T3 c;
	T4 d;
};


////////////////////////////////////////////////////////////



template <class T,const int size>
class YsFixedLengthArray
{
public:
	T dat[size];

	inline const T &operator[](int idx) const
	{
		return dat[idx];
	}
	inline T &operator[](int idx)
	{
		return dat[idx];
	}
	operator const T *() const
	{
		return dat;
	}
	operator T *()
	{
		return dat;
	}
};

template <class T,const int size>
inline int operator==(const YsFixedLengthArray <T,size> &a,const YsFixedLengthArray <T,size> &b)
{
	int i;
	for(i=0; i<size; i++)
	{
		if(a[i]!=b[i])
		{
			return 0;
		}
	}
	return 1;
}



////////////////////////////////////////////////////////////



template <class T,const int MinimumLength=1,class SizeType=int>
class YsArray
{
   // CAUTION!
// Whenever add an item, do not forget to modify operator=

public:
	YsArray();
	YsArray(int nv,const T v[]);
	YsArray(const YsArray <T,MinimumLength,SizeType> &from);
	~YsArray();

	const YsArray <T,MinimumLength,SizeType> &operator=(const YsArray <T,MinimumLength,SizeType> &from);
	const YsArray <T,MinimumLength,SizeType> &operator=(const YsList <T> *from);


	// Added 2001/04/26 >>
	inline const T &operator[](int idx) const;
	inline T &operator[](int idx);
	inline operator const T *() const;
	inline operator T *();
	inline int GetN(void) const;
	inline YSRESULT Append(const T &itm);
	inline const T &GetTop(void) const;
	inline T &GetEnd(void);
	inline const T &GetEnd(void) const;
	YSRESULT Append(int nItm,const T itm[]);
	// << Added 2001/04/26

	YSRESULT DeleteBySwapping(int id);  // Added 2001/04/27
	YSRESULT DeleteLast(void);  // Added 2006/03/22
	YSRESULT Increment(void);   // Added 2009/04/08
	YSRESULT Clear(void);       // Added 2009/04/09


	YSRESULT Set(const T &v); // 2004/08/23
	YSRESULT Set(int nv,const T v[]);
	inline YSRESULT SetItem(int idx,const T &v);
	inline int GetNumItem(void) const;
	inline const T &v(int i) const;
	inline T &GetEditableItem(int i);
	inline const T &GetItem(int i) const;
	inline const T *GetArray(void) const;
	inline T *GetEditableArray(void);
	inline T &GetEditableTopItem(void);
	inline const T &GetTopItem(void) const;
	inline T &GetEditableEndItem(void);
	inline const T &GetEndItem(void) const;

	inline int GetNAvailable(void) const;
	inline int GetNUnused(void) const;

	inline YSBOOL IsInRange(int id) const;  // 2008/07/06

	YSRESULT Resize(int neoSize);
	YSRESULT AppendItem(const T &dat);
	YSRESULT Insert(int id,const T &dat);
	YSRESULT Delete(int id);
	YSRESULT Delete(int idFrom,int numItem);

	YSRESULT Invert(void);
	YSRESULT Swap(int a,int b);

protected:
	SizeType nv,nAvailable;
	T *vv,prealloc[MinimumLength];
	inline YSRESULT Alloc(int n,YSBOOL cpy);
	inline YSRESULT Shrink(void);
};

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::operator[](int idx) const
{
	// return GetItem(idx);  2003/01/29 Don't check bound.
	return vv[idx];
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::operator[](int idx)
{
	return vv[idx];
	// return GetEditableItem(idx);  2003/01/29 Don't check bound.
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::operator const T *() const
{
	return GetArray();
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::operator T *()
{
	return GetEditableArray();
}

template <class T,const int MinimumLength,class SizeType>
inline int YsArray<T,MinimumLength,SizeType>::GetN(void) const
{
	return GetNumItem();
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::Append(const T &itm)
{
	return AppendItem(itm);
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetTop(void) const
{
	return GetTopItem();
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEnd(void)
{
	if(0<nv)
	{
		return vv[nv-1];
	}
	else
	{
		YsErrOut("YsArray::GetEnd()\n  The array is empty.\n");
		return *((T *)NULL);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetEnd(void) const
{
	return GetEndItem();
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::SetItem(int idx,const T &v)
{
	if(0<=idx && idx<nv)
	{
		vv[idx]=v;
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline int YsArray<T,MinimumLength,SizeType>::GetNumItem(void) const
{
	return nv;
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::v(int i) const
{
	if(0<=i && i<nv)
	{
		return vv[i];
	}
	else
	{
		YsErrOut("YsArray::v()\n  Array Subscript Exceeded\n");
		return *((T *)NULL);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEditableItem(int i)
{
	if(0<=i && i<nv)
	{
		return vv[i];
	}
	YsErrOut("YsArray::GetEditableItem()\n  Array Subscript Exceeded\n");
	return *((T *)NULL);
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetItem(int i) const
{
	return v(i);
}

template <class T,const int MinimumLength,class SizeType>
inline const T *YsArray<T,MinimumLength,SizeType>::GetArray(void) const
{
	return vv;
}

template <class T,const int MinimumLength,class SizeType>
inline T *YsArray<T,MinimumLength,SizeType>::GetEditableArray(void)
{
	return vv;
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEditableTopItem(void)
{
	if(0<nv)
	{
		return vv[0];
	}
	else
	{
		YsErrOut("YsArray::GetEditableTopItem()\n  The array is empty.\n");
		return *((T *)NULL);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetTopItem(void) const
{
	if(0<nv)
	{
		return vv[0];
	}
	else
	{
		YsErrOut("YsArray::GetTopItem()\n  The array is empty.\n");
		return *((T *)NULL);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEditableEndItem(void)
{
	if(0<nv)
	{
		return vv[nv-1];
	}
	else
	{
		YsErrOut("YsArray::GetEditableEndItem()\n  The array is empty.\n");
		return *((T *)NULL);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetEndItem(void) const
{
	if(0<nv)
	{
		return vv[nv-1];
	}
	else
	{
		YsErrOut("YsArray::GetEndItem()\n  The array is empty.\n");
		return *((T *)NULL);
	}
}


template <class T,const int MinimumLength,class SizeType>
inline int YsArray <T,MinimumLength,SizeType>::GetNAvailable(void) const
{
	return nAvailable;
}


template <class T,const int MinimumLength,class SizeType>
inline int YsArray <T,MinimumLength,SizeType>::GetNUnused(void) const
{
	if(nAvailable==0)
	{
		return MinimumLength-nv;
	}
	else
	{
		return nAvailable-nv;
	}
}

template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsArray<T,MinimumLength,SizeType>::IsInRange(int id) const
{
	if(0<=id && id<GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}


template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::Alloc(int n,YSBOOL cpy)
{
	if(n<=nAvailable)
	{
		nv=n;
		return YSOK;
	}
	else if(/* nAvailable<n && */ n<=MinimumLength)
	{
		if(vv==prealloc)
		{
			// Nothing to do
		}
		else /* if(vv!=prealloc) */
		{
			if(cpy==YSTRUE && nv>0)
			{
				int i;
				for(i=0; i<nv; i++)
				{
					prealloc[i]=vv[i];
				}
			}
			if(vv!=NULL)
			{
				delete [] vv;
			}
		}
		nv=n;
		vv=prealloc;
		nAvailable=0;
		return YSOK;
	}
	else
	{
		T *neo;
		int toAlloc;

		toAlloc=YsGreater((MinimumLength>0 ? MinimumLength*2 : 1),nAvailable*2);
		while(toAlloc<n)
		{
			toAlloc=toAlloc*2;
			if(toAlloc<0) // Overflow
			{
				goto INTOVERFLOW;
			}
		}
		neo=new T [toAlloc];
		if(neo!=NULL)
		{
			if(cpy==YSTRUE && nv>0)    // nv==0 -> don't have to copy
			{
				int i;
				for(i=0; i<nv; i++)
				{
					neo[i]=vv[i];
				}
			}
			if(vv!=NULL && vv!=prealloc)
			{
				delete [] vv;
			}
			nv=n;
			vv=neo;
			nAvailable=toAlloc;
			return YSOK;
		}
		else
		{
			goto OUTOFMEM;
		}
		return YSOK;
	}

// Never fall into this place

OUTOFMEM:
	YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsArray::Alloc()");
	YsErrOut("YsArray::Alloc()\n");
	YsErrOut("  Out of memory!!!\n");
	return YSERR;

INTOVERFLOW:
	YsErrOut("YsArray::Alloc()\n");
	YsErrOut("  Integer Overflow!!!\n");
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray <T,MinimumLength,SizeType>::Shrink(void)
{
	int i;
	if(vv!=NULL && vv!=prealloc && (nv<nAvailable/2 || nv<=MinimumLength))
	// Condition: nv<=MinimumLength is added on 02/11/2006
	{
		if(nv<=MinimumLength)
		{
			for(i=0; i<nv; i++)
			{
				prealloc[i]=vv[i];
			}
			delete [] vv;
			vv=prealloc;
			nAvailable=0;
		}
		else
		{
			int newSize;
			T *newArray;
			newSize=nAvailable/2;
			while(newSize>MinimumLength*2 && newSize>nv*2)
			{
				newSize/=2;
			}
			newArray=new T [newSize];
			if(newArray!=NULL)
			{
				for(i=0; i<nv; i++)
				{
					newArray[i]=vv[i];
				}
				delete [] vv;
				vv=newArray;
				nAvailable=newSize;
			}
			else
			{
				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsArray::Shrink()");
				return YSERR;
			}
		}
	}
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray()
{
	nv=0;
	nAvailable=0;
	vv=NULL;
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(int nv,const T v[])
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	Set(nv,v);
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(const YsArray <T,MinimumLength,SizeType> &from)
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	*this=from;
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::~YsArray()
{
	if(vv!=NULL && vv!=prealloc)
	{
		delete [] vv;
	}
}

template <class T,const int MinimumLength,class SizeType>
const YsArray<T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(const YsArray <T,MinimumLength,SizeType> &from)
{
	if(this!=&from)
	{
		Set(from.nv,from.vv);
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Set(const T &v)
{
	return Set(1,&v);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Set(int NV,const T V[])
{
	if(NV==0)
	{
		if(nAvailable>MinimumLength*2)
		{
			if(vv!=NULL && vv!=prealloc)
			{
				delete [] vv;
			}
			nv=0;
			vv=NULL;
			nAvailable=0;
		}
		else
		{
			nv=0;
		}
		return YSOK;
	}
	else
	{
		if(Alloc(NV,YSFALSE)==YSOK)
		{
			if(V!=NULL)
			{
				for(int i=0; i<NV; i++)
				{
					vv[i]=V[i];
				}
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsArray::Set()\n  Low Memory Warning!!\n");
			return YSERR;
		}
	}
}

template <class T,const int MinimumLength,class SizeType>
const YsArray<T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(const YsList <T> *from)
{
	int nItem;
	const YsList <T> *seeker;

	nItem=from->GetNumObject();
	if(Alloc(nItem,YSFALSE)==YSOK)
	{
		int i;
		for(i=0; i<nItem; i++)
		{
			seeker=from->Seek(i);
			vv[i]=seeker->dat;
		}
	}
	else
	{
		YsErrOut("YsArray::Set()\n  Low Memory Warning!!\n");
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Resize(int neoSize)
{
	return Alloc(neoSize,YSTRUE);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::AppendItem(const T &dat)
{
	if(&dat<vv || (vv+nv)<=&dat)
	{
		if(Alloc(nv+1,YSTRUE)==YSOK)
		{
			vv[nv-1]=dat;
			return YSOK;
		}
	}
	else
	{
		T tmp;
		tmp=dat;
		if(Alloc(nv+1,YSTRUE)==YSOK)
		{
			vv[nv-1]=tmp;
			return YSOK;
		}
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Append(int nItm,const T itm[])
{
	int i,cpyTop;
	cpyTop=nv;
	if(Alloc(nv+nItm,YSTRUE)==YSOK)
	{
		for(i=0; i<nItm; i++)
		{
			vv[cpyTop+i]=itm[i];
		}
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Delete(int id)
{
	if(0<=id && id<nv)
	{
		int i;
		for(i=id; i<nv-1; i++)
		{
			vv[i]=vv[i+1];
		}
		nv--;
		Shrink();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::Delete(int idFrom,int numItem)
{
	if(GetN()<=idFrom+numItem)
	{
		Resize(idFrom);
	}
	else
	{
		int i;
		for(i=idFrom; i+numItem<GetN(); i++)
		{
			vv[i]=vv[i+numItem];
		}
		Resize(GetN()-numItem);
	}
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::Invert(void)
{
	int i;
	for(i=0; i<nv/2; i++)
	{
		T a;
		a=vv[i];
		vv[i]=vv[nv-1-i];
		vv[nv-1-i]=a;
	}
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::Swap(int a,int b)
{
	T swp;
	swp=vv[a];
	vv[a]=vv[b];
	vv[b]=swp;
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::DeleteBySwapping(int id)
{
	if(0<=id && id<nv)
	{
		vv[id]=vv[nv-1];
		nv--;
		Shrink();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::DeleteLast(void)
{
	if(nv>0)
	{
		nv--;
		Shrink();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Increment(void)
{
	return Alloc(GetN()+1,YSTRUE);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Clear(void)
{
	return Set(0,NULL);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Insert(int id,const T &dat)
{
	if(0<=id && id<=nv && Alloc(nv+1,YSTRUE)==YSOK)
	{
		int i;
		for(i=nv-1; i>id; i--)
		{
			vv[i]=vv[i-1];
		}
		vv[id]=dat;
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline int operator==(const YsArray <T,MinimumLength,SizeType> &a,const YsArray <T,MinimumLength,SizeType> &b)
{
	if(a.GetN()==b.GetN())
	{
		int i;
		for(i=0; i<a.GetN(); i++)
		{
			if(a[i]!=b[i])
			{
				return 0;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////

template <class T,const int MinimumLength=1,class SizeType=int>
class YsOrderedArrayBase : protected YsArray <T,MinimumLength,SizeType>
{
// >> Complying with a stupid change made in g++ 3.4
protected:
using YsArray <T,MinimumLength,SizeType>::Set;
// << Complying with a stupid change made in g++ 3.4

public:
	YSRESULT Clear(void);
	YSRESULT AppendItem(const T &dat);
	YSRESULT DeleteItem(const T &dat);

	using YsArray <T,MinimumLength,SizeType>::operator[];
	using YsArray <T,MinimumLength,SizeType>::operator const T *;
	using YsArray <T,MinimumLength,SizeType>::GetN;
	using YsArray <T,MinimumLength,SizeType>::GetTop;
	using YsArray <T,MinimumLength,SizeType>::GetEnd;
	using YsArray <T,MinimumLength,SizeType>::GetNumItem;
	using YsArray <T,MinimumLength,SizeType>::GetItem;
	using YsArray <T,MinimumLength,SizeType>::GetArray;
	using YsArray <T,MinimumLength,SizeType>::Delete;

protected:
	virtual int Compare(const T &a,const T &b);  // Return -1,0 or +1
};

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::Clear(void)
{
	Set(0,NULL);
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::AppendItem(const T &dat)
{
	int i1,i2,im,n;
	n=GetNumItem();
	if(n==0 || Compare(dat,GetItem(n-1))>=0)
	{
		return YsArray <T,MinimumLength,SizeType>::AppendItem(dat);
	}
	else if(Compare(dat,GetItem(0))<=0)
	{
		return YsArray <T,MinimumLength,SizeType>::Insert(0,dat);
	}
	else
	{
		i1=0;
		i2=n-1;
		while(i2-i1>1)
		{
			im=(i1+i2)/2;
			if(Compare(dat,GetItem(im))<0)
			{
				i2=im;
			}
			else if(Compare(dat,GetItem(im))==0)
			{
				i1=im;
				i2=im+1;
			}
			else if(Compare(dat,GetItem(im))>0)
			{
				i1=im;
			}
		}
		return YsArray <T,MinimumLength,SizeType>::Insert(i2,dat);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::DeleteItem(const T &dat)
{
	int n,i1,i2,im;

	n=GetNumItem();
	if(n>0 && Compare(GetItem(0),dat)<=0 && Compare(dat,GetItem(n-1))<=0)
	{
		i1=0;
		i2=n-1;
		while(i2-i1>1)
		{
			im=(i1+i2)/2;
			if(Compare(dat,GetItem(im))<0)
			{
				i2=im;
			}
			else if(Compare(GetItem(im),dat)==0)
			{
				i1=im;
				i2=im+1;
			}
			else if(Compare(dat,GetItem(im))>0)
			{
				i1=im;
			}
		}
		if(Compare(GetItem(i1),dat)==0)
		{
			return YsArray <T,MinimumLength,SizeType>::Delete(i1);
		}
		else if(Compare(GetItem(i2),dat)==0)
		{
			return YsArray <T,MinimumLength,SizeType>::Delete(i2);
		}
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline int YsOrderedArrayBase <T,MinimumLength,SizeType>::Compare(const T &a,const T &b)
{
	YsErrOut("YsOrderedArrayBase <T,MinimumLength,SizeType>::Compate(const T &a,const T &b)\n");
	YsErrOut("  This function must not be called directly!\n");
	return 0;
}


////////////////////////////////////////////////////////////

template <class T,const int MinimumLength=1,class SizeType=int>
class YsOrderedArray : public YsOrderedArrayBase <T,MinimumLength,SizeType>
{
protected:
	virtual int Compare(const T &a,const T &b); // Return -1,0 or +1
};

template <class T,const int MinimumLength,class SizeType>
inline int YsOrderedArray <T,MinimumLength,SizeType>::Compare(const T &a,const T &b)
{
	// Basically, return a-b
	if(a<b)
	{
		return -1;
	}
	else if(a>b)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////


template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsDeleteArrayItem(int &nDel,YsArray <T,MinimumLength,SizeType> &ary, const T &item)
{
	int i;
	nDel=0;
	for(i=ary.GetN()-1; i>=0; i--)
	{
		if(ary[i]==item)
		{
			ary.Delete(i);
			nDel++;
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

template <class T>
inline YSBOOL YsIsIncluded(int &idx,int n,const T v[],const T &x)
{
	int i;
	for(i=0; i<n; i++)
	{
		if(x==v[i])
		{
			idx=i;
			return YSTRUE;
		}
	}
	return YSFALSE;
}

template <class T>
inline YSBOOL YsIsIncluded(int n,const T v[],const T &x)
{
	int idx;
	return YsIsIncluded(idx,n,v,x);
}

template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsIsIncluded(int &idx,YsArray <T,MinimumLength,SizeType> &ary, const T &item)
{
	return YsIsIncluded(idx,ary.GetN(),ary.GetArray(),item);
}

template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsIsIncluded(YsArray <T,MinimumLength,SizeType> &ary, const T &item)
{
	return YsIsIncluded(ary.GetN(),ary.GetArray(),item);
}

template <class T>
inline YSBOOL YsIsIncludedInSortedArray(int n,const T V[],const T &x)
{
	int a,b,m;
	a=0;
	b=n-1;
	while(1<b-a)
	{
		m=(a+b)/2;
		if(V[m]==x)
		{
			return YSTRUE;
		}
		else if(x<V[m])
		{
			b=m;
		}
		else
		{
			a=m;
		}
	}

	if(a==b)
	{
		if(V[a]==x)
		{
			return YSTRUE;
		}
	}
	else if(V[a]==x || V[b]==x)  // Note: Equal operator may cost considerably.  So, take a==b case separately.
	{
		return YSTRUE;
	}

	return YSFALSE;
}

template <class T,const int ML1,const int ML2>
inline YSRESULT YsCopyArray(YsArray <T,ML1> &dst,const YsArray <T,ML2> &src)
{
	return dst.Set(src.GetN(),src.GetArray());
}

template <class T>
inline YSBOOL YsCompareArray(int n,const T a1[],const T a2[])
{
	int i;
	for(i=0; i<n; i++)
	{
		if(a1[i]!=a2[i])
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

////////////////////////////////////////////////////////////

template <class T,const int MinimumLength,class SizeType>
inline int YsRemoveDuplicateInUnorderedArray(YsArray <T,MinimumLength,SizeType> &ary)
{
	int i,n;
	YsQuickSortAny <T,int> (ary.GetN(),ary,NULL);

	n=0;
	for(i=ary.GetN()-1; i>0; i--)
	{
		if(ary[i]==ary[i-1])
		{
			ary.DeleteBySwapping(i);
			n++;
		}
	}
	return n;
}

#define forYsArray(i,ary) for((i)=0; (i)<(ary).GetN(); (i)++)
#define forYsArrayRev(i,ary) for((i)=(ary).GetN()-1; (i)>=0; (i)--)


#define YsAryCpy(dst,src) (dst).Set((src).GetN(),(src))
#define YsAryCat(dst,src) (dst).Append((src).GetN(),(src))

/* } */
#endif
