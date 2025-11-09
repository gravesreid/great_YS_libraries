#ifndef YSMALLOC_IS_INCLUDED
#define YSMALLOC_IS_INCLUDED
/* { */

#include "ysexception.h"

template <class T,const int minimum> class YsOneMemoryBlock
{
public:
	YsOneMemoryBlock();
	virtual ~YsOneMemoryBlock();

	T *GetMemoryBlock(int requiredSize);

protected:
	T minBuf[minimum];
	T *ptr;
};

template <class T,const int minimum>
YsOneMemoryBlock <T,minimum>::YsOneMemoryBlock()
{
	ptr=NULL;
}

template <class T,const int minimum>
YsOneMemoryBlock <T,minimum>::~YsOneMemoryBlock()
{
	if(ptr!=NULL && ptr!=minBuf)
	{
		delete [] ptr;
	}
}

template <class T,const int minimum>
T *YsOneMemoryBlock <T,minimum>::GetMemoryBlock(int requiredSize)
{
	if(ptr==NULL)
	{
		if(requiredSize<=minimum)
		{
			ptr=minBuf;
		}
		else
		{
			ptr=new T[requiredSize];
			if(ptr==NULL)
			{
				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsOneMemoryBlock::GetMemoryBlock()");
			}
		}
		return ptr;
	}
	YsErrOut(
	    "YsOneMemoryBlock::GetMemoryBlock\n"
	    "  Attempted doubely get memory block from one YsOneMemoryBlock\n");
	return NULL;
}

/* } */
#endif
