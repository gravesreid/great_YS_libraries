#ifndef YSLIST__IS_INCLUDED
#define YSLIST__IS_INCLUDED
/* { */


/*
Caution:
  The function YsListContainer::Create does not necessarily calls the
  constructor of class T. */


#include <limits.h>


extern int YsNPreAllocListItem;


template <class T>
class YsListContainer;

template <class T>
class YsListPreallocItem;

template <class T>
class YsListAllocator;

template <class T>
class YsListItem
{
friend class YsListContainer <T>;
friend class YsListAllocator <T>;
friend class YsListPreallocItem <T>;

protected:
	YsListPreallocItem <T> *prealloc;
	YsListContainer <T> *belongTo;
	YsListItem <T> *next,*prev;
	int serial;
public:
	inline YsListContainer <T> *GetContainer(void) const;
	T dat;
};

template <class T>
inline YsListContainer <T> *YsListItem <T>::GetContainer(void) const
{
	return belongTo;
}

////////////////////////////////////////////////////////////

template <class T>
class YsListPreallocItem
{
friend class YsListContainer <T>;
friend class YsListAllocator <T>;

protected:
	inline YsListPreallocItem(int nAllocUnit,YsListAllocator <T> *owner);
	inline ~YsListPreallocItem();

	YsListAllocator <T> *belongTo;
	YsListPreallocItem <T> *next,*prev;

public:
	int nUsed,nBuffer;
	YsListItem <T> *buffer;
};

template <class T>
inline YsListPreallocItem <T>::YsListPreallocItem(int nAllocUnit,YsListAllocator <T> *owner)
{
	int i;

	buffer=new YsListItem <T> [nAllocUnit];
	if(buffer!=NULL)
	{
		buffer[0].prealloc=this;
		buffer[0].prev=NULL;
		buffer[0].next=&buffer[1];
		for(i=1; i<nAllocUnit-1; i++)
		{
			buffer[i].prealloc=this;
			buffer[i].prev=&buffer[i-1];
			buffer[i].next=&buffer[i+1];
		}
		buffer[nAllocUnit-1].prealloc=this;
		buffer[nAllocUnit-1].prev=&buffer[nAllocUnit-2];
		buffer[nAllocUnit-1].next=NULL;

		nUsed=0;

		belongTo=owner;
		next=NULL;
		prev=NULL;

		YsNPreAllocListItem++;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsListPreallocItem()");
	}
}

template <class T>
inline YsListPreallocItem <T>::~YsListPreallocItem()
{
	YsNPreAllocListItem--;
	delete [] buffer;
}

////////////////////////////////////////////////////////////

template <class T>
class YsListAllocator
{
public:
	inline YsListAllocator(int nAllocUnit=128);
	inline ~YsListAllocator();

	inline void SelfDiagnostic(void);
	inline YsListItem <T> *Create(void);
	inline YSRESULT SetAllocUnit(int unit);
	inline int GetAllocUnit(void) const;
	inline YSRESULT CollectGarbage(void);
	inline YSRESULT SaveUsageMap(const char fn[]) const;

	int nUnit;
	YsListPreallocItem <T> *preallocHead,*preallocTail;
	YsListItem <T> *availableHead,*availableTail;
};

template <class T>
inline YsListAllocator <T>::YsListAllocator(int nAllocUnit)
{
	availableHead=NULL;
	availableTail=NULL;
	preallocHead=NULL;
	preallocTail=NULL;
	nUnit=nAllocUnit;
}

template <class T>
inline YsListAllocator <T>::~YsListAllocator()
{
	YsListPreallocItem <T> *seek,*next;
	next=preallocHead;
	while((seek=next)!=NULL)
	{
		next=seek->next;
		if(seek->nUsed==0)
		{
			delete seek;
		}
		else
		{
			seek->next=NULL;
			seek->prev=NULL;
			seek->belongTo=NULL;  // This will be deleted when nUsed becomes zero.
		}
	}
}

template <class T>
inline void YsListAllocator <T>::SelfDiagnostic(void)
{
	int nAvailable,nPrealloc,nUsed;
	YsListItem <T> *seekItem;
	YsListPreallocItem <T> *seekPrealloc;

	YsPrintf("YsListAllocator::SelfDiagnostic()\n");

	YsPrintf("  nUnit=%d\n",nUnit);
	nAvailable=0;
	for(seekItem=availableHead; seekItem!=NULL; seekItem=seekItem->next)
	{
		nAvailable++;
		if(seekItem->next!=NULL && seekItem->next->prev!=seekItem)
		{
			YsPrintf("seekItem->next->prev is not pointing seekItem\n");
			YsPrintf("  seekItem=%08x\n",seekItem);
			YsPrintf("  next=%08x\n",seekItem->next);
			YsPrintf("  prev=%08x\n",seekItem->prev);
		}
		if(seekItem->prev!=NULL && seekItem->prev->next!=seekItem)
		{
			YsPrintf("seekItem->prev->next is not pointing seekItem\n");
			YsPrintf("  next=%08x\n",seekItem->next);
			YsPrintf("  seekItem=%08x\n",seekItem);
			YsPrintf("  prev=%08x\n",seekItem->prev);
		}
	}
	YsPrintf("  %d available items.\n",nAvailable);

	nUsed=0;
	nPrealloc=0;
	for(seekPrealloc=preallocHead; seekPrealloc!=NULL; seekPrealloc=seekPrealloc->next)
	{
		nPrealloc++;
		nUsed+=seekPrealloc->nUsed;
	}
	YsPrintf("  %d preallocations.\n",nPrealloc);
	YsPrintf("  %d used items.\n",nUsed);
	YsPrintf("  %d total number of items.\n",nUsed+nAvailable);
	YsPrintf("  nPrealloc*nUnit=%d\n",nPrealloc*nUnit);
	if(nPrealloc*nUnit!=nUsed+nAvailable)
	{
		YsPrintf("  Total number of items and nPrealloc*nUnit does not match.\n");
	}
}

template <class T>
inline YsListItem <T> *YsListAllocator <T>::Create(void)
{
	if(availableHead==NULL)
	{
		YsListPreallocItem <T> *prealloc;
		prealloc=new YsListPreallocItem <T> (nUnit,this);
		if(prealloc!=NULL)
		{
			if(preallocTail!=NULL)
			{
				preallocTail->next=prealloc;
				prealloc->prev=preallocTail;
				preallocTail=prealloc;
			}
			else
			{
				preallocHead=prealloc;
				preallocTail=prealloc;
			}
			availableHead=&prealloc->buffer[0];
			availableTail=&prealloc->buffer[nUnit-1];
			prealloc->nBuffer=nUnit;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsListAllocator::Create()");
			return NULL;
		}
	}

	YsListItem <T> *neo;
	neo=availableHead;
	availableHead=availableHead->next;
	if(availableHead!=NULL)
	{
		availableHead->prev=NULL;
	}
	else if(availableHead==NULL)
	{
		availableTail=NULL;
	}
	neo->prealloc->nUsed++;
	return neo;
}

template <class T>
inline YSRESULT YsListAllocator <T>::SetAllocUnit(int unit)
{
	if(unit>0)
	{
		nUnit=unit;
		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::SetAllocUnit()\n");
		YsErrOut("  Alloc unit must be greater than 0\n");
		return YSERR;
	}
}

template <class T>
inline int YsListAllocator <T>::GetAllocUnit(void) const
{
	return nUnit;
}

template <class T>
inline YSRESULT YsListAllocator <T>::CollectGarbage(void)
{
	int c;
	YsListPreallocItem <T> *seek,*next;
	next=preallocHead;
	c=0;
	while((seek=next)!=NULL)
	{
		next=seek->next;
		if(seek->nUsed==0)
		{
			if(seek->next!=NULL)
			{
				seek->next->prev=seek->prev;
			}
			if(seek->prev!=NULL)
			{
				seek->prev->next=seek->next;
			}
			if(seek==preallocHead)
			{
				preallocHead=seek->next;
			}
			if(seek==preallocTail)
			{
				preallocTail=seek->prev;
			}

			int i;
			for(i=0; i<seek->nBuffer; i++)
			{
				if(seek->buffer[i].next!=NULL)
				{
					seek->buffer[i].next->prev=seek->buffer[i].prev;
				}
				if(seek->buffer[i].prev!=NULL)
				{
					seek->buffer[i].prev->next=seek->buffer[i].next;
				}
				if(availableHead==&seek->buffer[i])
				{
					availableHead=seek->buffer[i].next;
				}
				if(availableTail==&seek->buffer[i])
				{
					availableTail=seek->buffer[i].prev;
				}
			}

			delete seek;
		}
		c++;
	}
	return YSOK;
}

template <class T>
inline YSRESULT YsListAllocator <T>::SaveUsageMap(const char fn[]) const
{
	FILE *fp;
	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		YsListPreallocItem <T> *seeker;
		for(seeker=preallocHead; seeker!=NULL; seeker=seeker->next)
		{
			int nUsed;
			int blk,prevBlk;  // 0 to 99
			int i;
			blk=0;
			prevBlk=0;
			nUsed=0;
			for(i=0; i<seeker->nBuffer; i++)
			{
				blk=i*80/seeker->nBuffer;
				if(blk!=prevBlk)
				{
					if(nUsed<10)
					{
						fprintf(fp,"%d",nUsed);
					}
					else if(nUsed-10<='Z'-'A')
					{
						fprintf(fp,"%c",'A'+(nUsed-10));
					}
					else
					{
						fprintf(fp,"*");
					}
					nUsed=0;
				}

				if(seeker->buffer[i].belongTo!=NULL)
				{
					nUsed++;
				}

				prevBlk=blk;
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
	}
}

////////////////////////////////////////////////////////////

template <class T>
class YsListContainer
{
public:
	inline YsListContainer(YsListAllocator <T> &alloc);
	inline ~YsListContainer();
	inline void SelfDiagnostic(void) const;
	inline int GetN(void) const;
	inline int GetNumCached(void) const;
	inline int GetIdFromItem(const YsListItem <T> *ptr) const;
	inline YsListItem <T> *GetItemFromId(int id) const;
	inline YsListItem <T> *Create(void);
	inline YSRESULT Delete(YsListItem <T> *toDelete);
	inline YSRESULT CleanUp(void);
	inline YSRESULT Transfer(YsListItem <T> *itm,YsListContainer <T> &dst);
	inline YSRESULT MoveItemToTop(YsListItem <T> *itm);
	inline YSRESULT MoveItemToEnd(YsListItem <T> *itm);
	inline YSRESULT MoveItemAfter(YsListItem <T> *itm,YsListItem <T> *afterThis);
	inline YSRESULT MoveItemBefore(YsListItem <T> *itm,YsListItem <T> *beforeThis);
	inline YSRESULT Reorder(int n,YsListItem <T> *order[]);
	inline YSRESULT Encache(void) const;
	inline YSRESULT Decache(void) const;
	inline YsListItem <T> *FindNext(const YsListItem <T> *ptr) const;
	inline YsListItem <T> *FindPrev(const YsListItem <T> *ptr) const;
	inline YsListItem <T> *SeekTop(void) const;
	inline YsListItem <T> *SeekEnd(void) const;
	inline void RewindPointer(void) const;
	inline YsListItem <T> *StepPointer(void) const;
	inline YsListItem <T> *StepBackPointer(void) const;

protected:
	int nItem;

	mutable int nCached;
	mutable YsListItem <T> **cache;

	YsListItem <T> *head,*tail;
	YsListAllocator <T> *allocator;

	mutable YsListItem <T> *fwdSeeker,*bckSeeker;
};

template <class T>
inline YsListContainer <T>::YsListContainer(YsListAllocator <T> &alloc)
{
	head=NULL;
	tail=NULL;
	fwdSeeker=NULL;
	bckSeeker=NULL;

	nItem=0;
	nCached=0;
	cache=NULL;

	allocator=&alloc;
}

template <class T>
inline YsListContainer <T>::~YsListContainer()
{
	YsListItem <T> *seeker,*next;
	seeker=head;
	while(seeker!=NULL)
	{
		next=seeker->next;
		Delete(seeker);
		seeker=next;
	}
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}
}

template <class T>
inline void YsListContainer <T>::SelfDiagnostic(void) const
{
	int n;
	YsListItem <T> *seeker;

	YsPrintf("YsListContainer::SelfDiagnostic()\n");
	YsPrintf("  nItem=%d\n",nItem);
	YsPrintf("  nCached=%d\n",nCached);

	n=0;
	for(seeker=head; seeker!=NULL; seeker=seeker->next)
	{
		if(seeker->serial<nCached && cache[n]!=seeker)
		{
			YsPrintf("Cache Mismatch! [%d] %08x %08x\n",n,seeker,cache[n]);
		}
		if(seeker->belongTo!=this)
		{
			YsPrintf("Item [%d] %08x does not belong to this list!\n",n,seeker);
		}
		n++;
		if(seeker->next==NULL && seeker!=tail)
		{
			YsPrintf("Tail item is incorrectly recorded!\n");
		}
	}

	YsPrintf("  Number of item counted=%d\n",n);
	if(n!=nItem)
	{
		YsPrintf("  nItem and the number of item do not match!\n");
	}
}

template <class T>
inline int YsListContainer <T>::GetN(void) const
{
	return nItem;
}

template <class T>
inline int YsListContainer <T>::GetNumCached(void) const
{
	return nCached;
}

template <class T>
inline int YsListContainer <T>::GetIdFromItem(const YsListItem <T> *ptr) const
{
	if(ptr->belongTo==this)
	{
		if(ptr->serial<nCached)
		{
			return ptr->serial;
		}
		else
		{
			int id;
			YsListItem <T> *seek;
			id=0;
			for(seek=head; seek!=ptr && seek!=NULL; seek=seek->next)
			{
				id++;
			}
			return id;
		}
	}
	else
	{
		YsErrOut("YsListContainer::GetId()\n");
		YsErrOut("  Tried to access a list item of different list.\n");
		return -1;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::GetItemFromId(int id) const
{
	if(0<=id && id<nItem)
	{
		if(id<nCached)
		{
			return cache[id];
		}
		else
		{
			int n;
			YsListItem <T> *seek;
			if(nCached>0)
			{
				n=nCached;
				seek=cache[nCached-1]->next;
			}
			else
			{
				n=0;
				seek=head;
			}
			while(n<id && seek!=NULL)
			{
				n++;
				seek=seek->next;
			}
			return seek;
		}
	}
	else
	{
		YsErrOut("YsListContainer::GetItemFromId()\n");
		YsErrOut("  Index out of range.\n");
		return NULL;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::Create(void)
{
	YsListItem <T> *neo;
	neo=allocator->Create();
	if(head!=NULL)
	{
		tail->next=neo;
		neo->prev=tail;
		neo->next=NULL;
		tail=neo;
	}
	else
	{
		head=neo;
		tail=neo;
		neo->prev=NULL;
		neo->next=NULL;
	}
	neo->belongTo=this;
	neo->serial=INT_MAX;
	nItem++;
	return neo;
};

template <class T>
inline YSRESULT YsListContainer <T>::Delete(YsListItem <T> *toDelete)
{
	if(toDelete==NULL)
	{
		YsErrOut("YsListContainer::Delete\n");
		YsErrOut("  NULL pointer is given.\n");
		return YSERR;
	}
	else if(toDelete->belongTo==this)
	{
		if(toDelete==fwdSeeker)
		{
			fwdSeeker=FindNext(fwdSeeker);
		}
		if(toDelete==bckSeeker)
		{
			bckSeeker=FindPrev(bckSeeker);
		}


		if(toDelete->prev!=NULL)
		{
			toDelete->prev->next=toDelete->next;
		}
		if(toDelete->next!=NULL)
		{
			toDelete->next->prev=toDelete->prev;
		}

		if(toDelete->next==NULL)
		{
			tail=toDelete->prev;
		}
		if(toDelete->prev==NULL)
		{
			head=toDelete->next;
		}

		if(toDelete->serial<nCached)  // 2002/01/04 moved from below
		{
			nCached=toDelete->serial;
		}

		toDelete->next=NULL;
		if(toDelete->prealloc->belongTo!=NULL)
		{
			toDelete->prev=toDelete->prealloc->belongTo->availableTail;
			if(toDelete->prealloc->belongTo->availableHead==NULL)
			{
				toDelete->prealloc->belongTo->availableHead=toDelete;
			}
			else
			{
				toDelete->prealloc->belongTo->availableTail->next=toDelete;
			}
			toDelete->prealloc->belongTo->availableTail=toDelete;
			toDelete->prealloc->nUsed--;
			toDelete->belongTo=NULL;
		}
		else
		{
			toDelete->prealloc->nUsed--;
			if(toDelete->prealloc->nUsed==0)
			{
				delete toDelete->prealloc;
			}
		}

		nItem--;

		// 2003/01/04  if(toDelete->serial<nCached) used to be here.
		// It's not a good location, because toDelete may be already deleted at this point.

		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::Delete\n");
		YsErrOut("  Attempted to delete an item not belonging to the container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::CleanUp(void)
{
	YsListItem <T> *itm,*nxt;
	nxt=head;
	while((itm=nxt)!=NULL)
	{
		nxt=itm->next;

		if(itm->prealloc->belongTo!=NULL)
		{
			itm->prev=itm->prealloc->belongTo->availableTail;
			itm->next=NULL;
			if(itm->prealloc->belongTo->availableHead==NULL)
			{
				itm->prealloc->belongTo->availableHead=itm;
			}
			else
			{
				itm->prealloc->belongTo->availableTail->next=itm;
			}
			itm->prealloc->belongTo->availableTail=itm;
			itm->prealloc->nUsed--;
			itm->belongTo=NULL;
		}
		else
		{
			itm->prealloc->nUsed--;
			if(itm->prealloc->nUsed==0)
			{
				delete itm->prealloc;
			}
		}
	}
	nItem=0;
	nCached=0;
	head=NULL;
	tail=NULL;
	fwdSeeker=NULL;
	bckSeeker=NULL;
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}
	return YSOK;
}

template <class T>
inline YSRESULT YsListContainer <T>::Transfer(YsListItem <T> *itm,YsListContainer <T> &dst)
{
	if(itm==NULL)
	{
		YsErrOut("YsListContainer::Transfer\n");
		YsErrOut("  NULL pointer is given.\n");
		return YSERR;
	}
	else if(itm->belongTo==this)
	{
		if(itm==fwdSeeker)
		{
			fwdSeeker=FindNext(fwdSeeker);
		}
		if(itm==bckSeeker)
		{
			bckSeeker=FindPrev(bckSeeker);
		}


		if(itm->prev!=NULL)
		{
			itm->prev->next=itm->next;
		}
		if(itm->next!=NULL)
		{
			itm->next->prev=itm->prev;
		}
		if(tail==itm)
		{
			tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
		}
		if(head==itm)
		{
			head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
		}

		nItem--;
		if(itm->serial<nCached)
		{
			nCached=itm->serial;
		}

		if(dst.tail!=NULL)
		{
			dst.tail->next=itm;
			itm->prev=dst.tail;
			dst.tail=itm;
			// dst.nItem++; Why did I write this? Isn't it a bug? 2002/06/27
		}
		else
		{
			dst.head=itm;
			dst.tail=itm;
			itm->prev=NULL;
		}
		itm->next=NULL;
		itm->belongTo=&dst;
		dst.nItem++;

		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::Transfer\n");
		YsErrOut("  Attempted to transfer an item not belonging to this container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemToTop(YsListItem <T> *itm)
{
	if(itm->belongTo==this)
	{
		if(itm==head)
		{
			return YSOK;  // Already at the top
		}
		else
		{
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm->next;
			}
			if(itm->next!=NULL)
			{
				itm->next->prev=itm->prev;
			}
			if(tail==itm)
			{
				tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
			}

			nCached=0;

			itm->prev=NULL;
			itm->next=head;
			if(itm->next!=NULL)
			{
				itm->next->prev=itm;
			}
			head=itm;

			return YSOK;
		}
	}
	else
	{
		YsErrOut("YsListContainer::MoveItemToTop\n");
		YsErrOut("  Attempted to access an item that does not belong to this container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemToEnd(YsListItem <T> *itm)
{
	if(itm->belongTo==this)
	{
		if(itm==tail)
		{
			return YSOK;  // Already at the end
		}

		if(itm->prev!=NULL)
		{
			itm->prev->next=itm->next;
		}
		if(itm->next!=NULL)
		{
			itm->next->prev=itm->prev;
		}
		if(head==itm)
		{
			head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
		}

		if(itm->serial<nCached)
		{
			nCached=itm->serial;
		}

		itm->prev=tail;
		itm->next=NULL;
		if(itm->prev!=NULL)
		{
			itm->prev->next=itm;
		}
		tail=itm;
		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::MoveItemToEnd\n");
		YsErrOut("  Attempted to access an item that does not belong to this container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemAfter(YsListItem <T> *itm,YsListItem <T> *afterThis)
{
	if(afterThis==NULL)
	{
		return MoveItemToTop(itm);
	}
	else
	{
		if(itm!=afterThis && itm->belongTo==this && afterThis->belongTo==this)
		{
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm->next;
			}
			if(itm->next!=NULL)
			{
				itm->next->prev=itm->prev;
			}
			if(tail==itm)
			{
				tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
			}
			if(head==itm)
			{
				head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
			}

			if(itm->serial<nCached)
			{
				nCached=itm->serial;
			}
			if(afterThis->serial<nCached)
			{
				nCached=afterThis->serial;
			}

			itm->prev=afterThis;
			itm->next=afterThis->next;
			itm->prev->next=itm;
			if(itm->next!=NULL)
			{
				itm->next->prev=itm;
			}
			else
			{
				tail=itm;
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsListContainer::MoveItemAfter\n");
			YsErrOut("  Invalid pointer.\n");
			return YSERR;
		}
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemBefore(YsListItem <T> *itm,YsListItem <T> *beforeThis)
{
	if(beforeThis==NULL)
	{
		return MoveItemToEnd(itm);
	}
	else
	{
		if(itm!=beforeThis && itm->belongTo==this && beforeThis->belongTo==this)
		{
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm->next;
			}
			if(itm->next!=NULL)
			{
				itm->next->prev=itm->prev;
			}
			if(tail==itm)
			{
				tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
			}
			if(head==itm)
			{
				head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
			}

			if(itm->serial<nCached)
			{
				nCached=itm->serial;
			}
			if(beforeThis->serial<nCached)
			{
				nCached=beforeThis->serial-1;
				if(nCached<0)
				{
					nCached=0;
				}
			}

			itm->prev=beforeThis->prev;
			itm->next=beforeThis;
			itm->next->prev=itm;
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm;
			}
			else
			{
				head=itm;
			}

			return YSOK;
		}
		else
		{
			YsErrOut("YsListContainer::MoveItemBefore\n");
			YsErrOut("  Invalid pointer.\n");
			return YSERR;
		}
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::Reorder(int n,YsListItem <T> *order[])
{
	int i;
	for(i=0; i<n; i++)
	{
		if(order[i]->belongTo!=this)
		{
			YsErrOut("YsListContainer::Reorder()\n");
			YsErrOut("  Attempted to access an item not belonging to this container.\n");
			return YSERR;
		}
	}

	for(i=n-1; i>=0; i--)
	{
		if(order[i]!=head)
		{
			if(order[i]->prev!=NULL)
			{
				order[i]->prev->next=order[i]->next;
			}
			if(order[i]->next!=NULL)
			{
				order[i]->next->prev=order[i]->prev;
			}
			// if(head==order[i])
			// {
			// 	head=order[i]->next;
			// }
			if(tail==order[i])
			{
				tail=order[i]->prev;
			}

			order[i]->prev=NULL;
			order[i]->next=head;
			head->prev=order[i];
			head=order[i];
		}
	}

	nCached=0;
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}

	return YSOK;
}

template <class T>
inline YSRESULT YsListContainer <T>::Encache(void) const
{
	if(nCached<nItem)
	{
		int i;
		YsListItem <T> *seeker;

		if(cache!=NULL)
		{
			delete [] cache;
			cache=NULL;
		}

		cache=new YsListItem <T> * [nItem];
		if(cache!=NULL)
		{
			i=0;
			for(seeker=head; seeker!=NULL; seeker=seeker->next)
			{
				seeker->serial=i;
				cache[i]=seeker;
				i++;
			}

			nCached=nItem;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsListAllocator::Encache()");
		}
	}
	return YSOK;
}

template <class T>
inline YSRESULT YsListContainer <T>::Decache(void) const
{
	nCached=0;
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}
	return YSOK;
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::FindNext(const YsListItem <T> *ptr) const
{
	if(ptr!=NULL)
	{
		return ptr->next;
	}
	else
	{
		return head;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::FindPrev(const YsListItem <T> *ptr) const
{
	if(ptr!=NULL)
	{
		return ptr->prev;
	}
	else
	{
		return tail;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::SeekTop(void) const
{
	return head;
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::SeekEnd(void) const
{
	return tail;
}

template <class T>
inline void YsListContainer <T>::RewindPointer(void) const
{
	fwdSeeker=FindNext(NULL);
	bckSeeker=FindPrev(NULL);
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::StepPointer(void) const
{
	YsListItem <T> *ptr;
	ptr=fwdSeeker;
	fwdSeeker=FindNext(fwdSeeker);
	return ptr;
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::StepBackPointer(void) const
{
	YsListItem <T> *ptr;
	ptr=bckSeeker;
	bckSeeker=FindPrev(bckSeeker);
	return ptr;
}

/* } */
#endif
