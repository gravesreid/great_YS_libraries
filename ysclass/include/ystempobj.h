#ifndef YSTEMPOBJ_IS_INCLUDED
#define YSTEMPOBJ_IS_INCLUDED
/* { */


template <class T>
class YsTemporaryObject
{
public:
	YsTemporaryObject();
	~YsTemporaryObject();

	T *GetSingle(void);
	T *GetArray(int n);

	YSRESULT Release(void);

private:
	YSBOOL usedAsAnArray;
	T *ptr;
};

template <class T>
YsTemporaryObject <T>::YsTemporaryObject()
{
	usedAsAnArray=YSFALSE;
	ptr=NULL;
}

template <class T>
YsTemporaryObject <T>::~YsTemporaryObject()
{
	Release();
}

template <class T>
T *YsTemporaryObject <T>::GetSingle(void)
{
	if(ptr==NULL)
	{
		ptr=new T;
		if(ptr!=NULL)
		{
			usedAsAnArray=YSFALSE;
			return ptr;
		}
		else
		{
			printf("YsTemporaryObject::GetSingle()\n");
			printf("  Cannot allocate memory block.\n");
			return NULL;
		}
	}
	else
	{
		printf("YsTemporaryObject::GetSingle()\n");
		printf("  This instance is already used.\n");
		return NULL;
	}
}

template <class T>
T *YsTemporaryObject <T>::GetArray(int n)
{
	if(ptr==NULL)
	{
		ptr=new T [n];
		if(ptr!=NULL)
		{
			usedAsAnArray=YSTRUE;
			return ptr;
		}
		else
		{
			printf("YsTemporaryObject::GetArray()\n");
			printf("  Cannot allocate memory block.\n");
			return NULL;
		}
	}
	else
	{
		printf("YsTemporaryObject::GetArray()\n");
		printf("  This instance is already used.\n");
		return NULL;
	}
}

template <class T>
YSRESULT YsTemporaryObject <T>::Release(void)
{

	if(ptr!=NULL)
	{
		if(usedAsAnArray==YSTRUE)
		{
			delete [] ptr;
		}
		else
		{
			delete ptr;
		}
		ptr=NULL;
	}
	return YSOK;
}

/* } */
#endif
