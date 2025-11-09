#ifndef YSHASH_IS_INCLUDED
#define YSHASH_IS_INCLUDED
/* { */

#include "yssort.h"

template <class toFind>
class YsHashElement
{
public:
	toFind dat;
	unsigned key;
};

class YsHashElementEnumHandle
{
public:
	unsigned hashIdx;
	unsigned arrayIdx;
};

// Each element must have an unique key
template <class toFind>
class YsHashTable
{
public:
	YsHashTable(int hashSize=16);
	~YsHashTable();

	YSRESULT PrepareTable(void);
	YSRESULT PrepareTable(int hashSize);
	YSRESULT Resize(int hashSize);
	YSRESULT CollectGarbage(void);
	YSRESULT AddElement(unsigned searchKey,toFind element);
	YSRESULT DeleteElement(unsigned searchKey,toFind element);
	YSRESULT FindElement(toFind &elem,unsigned searchKey) const;
	YSRESULT GetElement(toFind &elem,const YsHashElementEnumHandle &handle) const;
	YSBOOL CheckKeyExist(unsigned searchKey) const;

	YSRESULT RewindElementEnumHandle(YsHashElementEnumHandle &handle) const;
	YSRESULT FindNextElement(YsHashElementEnumHandle &handle) const;

	YSRESULT SelfDiagnostic(void) const;

	void EnableAutoResizing(int minSize,int maxSize);
	void EnableAutoResizing(void);

	int GetNumUnused(void) const;

	const char *tableName;

protected:
	YSRESULT CheckAutoResizingGrow(void);
	YSRESULT CheckAutoResizingShrink(void);

	YSBOOL enableAutoResizing;
	int autoResizingMin,autoResizingMax;

	int HashSize;
	int nElem;
	mutable YsArray <YsHashElement <toFind> > *table;
	YsArray <YsHashElement <toFind> > *FindTableIndex(int &idx,unsigned searchKey) const;
};

template <class toFind>
YsHashTable <toFind> ::YsHashTable(int hashSize)
{
	HashSize=hashSize;
	table=new YsArray <YsHashElement <toFind> >[hashSize];
	nElem=0;

	tableName="";

	enableAutoResizing=YSFALSE;
	autoResizingMin=0;
	autoResizingMax=0;
}

template <class toFind>
YsHashTable <toFind> ::~YsHashTable()
{
	if(table!=NULL)
	{
		delete [] table;
	}
}

template <class toFind>
YSRESULT YsHashTable <toFind>::PrepareTable(void)
{
	int i;
	for(i=0; i<HashSize; i++)
	{
		table[i].Set(0,NULL);
	}
	nElem=0;
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::PrepareTable(int hashSize)
{
	if(hashSize<1)
	{
		hashSize=1;
	}
	if(table!=NULL)
	{
		delete [] table;
	}
	HashSize=hashSize;
	table=new YsArray <YsHashElement <toFind> >[hashSize];
	nElem=0;
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::Resize(int hashSize)
{
	int i;
	YSBOOL saveAutoResizing;
	YsArray <YsHashElement <toFind> > elemList;
	for(i=0; i<HashSize; i++)
	{
		elemList.Append(table[i].GetN(),table[i]);
	}
	PrepareTable(hashSize);

	saveAutoResizing=enableAutoResizing;
	enableAutoResizing=YSFALSE;
	for(i=0; i<elemList.GetN(); i++)
	{
		AddElement(elemList[i].key,elemList[i].dat);
	}
	enableAutoResizing=saveAutoResizing;

	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::CollectGarbage(void)
{
	Resize(HashSize);
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::AddElement(unsigned searchKey,toFind element)
{
	int hashId,i1,i2,im;
	YsArray <YsHashElement <toFind> > *entry;
	YsHashElement <toFind> neo;

	hashId=searchKey%HashSize;
	entry=&table[hashId];

	neo.dat=element;
	neo.key=searchKey;

	if(entry->GetN()==0)
	{
		entry->Append(neo);
	}
	else
	{
		i1=0;
		i2=entry->GetNumItem()-1;

		if(searchKey<(*entry)[i1].key)
		{
			entry->Insert(i1,neo);
		}
		else if((*entry)[i2].key<searchKey)
		{
			entry->Append(neo);
		}
		else
		{
			i1=0;
			i2=entry->GetN()-1;
			while(i2-i1>1)
			{
				im=(i1+i2)/2;
				if(searchKey<(*entry)[im].key)
				{
					i2=im;
				}
				else
				{
					i1=im;
				}
			}
			entry->Insert(i2,neo);
		}
	}
	nElem++;
	CheckAutoResizingGrow();
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::DeleteElement(unsigned searchKey,toFind element)
{
	YsArray <YsHashElement <toFind> > *entry;
	int idx;

	entry=FindTableIndex(idx,searchKey);
	if(entry!=NULL)
	{
		entry->Delete(idx);
		nElem--;
		CheckAutoResizingShrink();
		return YSOK;
	}

	YsErrOut("YsHashTable::DeleteElement()\n");
	YsErrOut("  Element not found\n");
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::FindElement(toFind &element,unsigned searchKey) const
{
	int idx;
	YsArray <YsHashElement <toFind> > *entry;

	entry=FindTableIndex(idx,searchKey);
	if(entry!=NULL)
	{
		element=(*entry)[idx].dat;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::GetElement(toFind &elem,const YsHashElementEnumHandle &handle) const
{
	int hashIdx,arrayIdx;

	hashIdx=handle.hashIdx;
	arrayIdx=handle.arrayIdx;

	if(0<=hashIdx && hashIdx<HashSize &&
	   0<=arrayIdx && arrayIdx<table[hashIdx].GetN())
	{
		elem=table[hashIdx][arrayIdx].dat;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
YSBOOL YsHashTable <toFind>::CheckKeyExist(unsigned searchKey) const
{
	int idx;
	if(FindTableIndex(idx,searchKey)!=NULL)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}

}

template <class toFind>
YSRESULT YsHashTable <toFind>::RewindElementEnumHandle(YsHashElementEnumHandle &handle) const
{
	// Rewind -> handle points to the first element
	int i;
	for(i=0; i<HashSize; i++)
	{
		if(table[i].GetN()>0)
		{
			handle.hashIdx=i;
			handle.arrayIdx=0;
			return YSOK;
		}
	}
	handle.hashIdx=0;
	handle.arrayIdx=0;
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::FindNextElement(YsHashElementEnumHandle &handle) const
{
	int hashIdx,arrayIdx;
	hashIdx=handle.hashIdx;
	arrayIdx=handle.arrayIdx;

	arrayIdx++;
	if(arrayIdx<table[hashIdx].GetN())
	{
		handle.hashIdx=hashIdx;
		handle.arrayIdx=arrayIdx;
		return YSOK;
	}

	hashIdx++;
	arrayIdx=0;

	while(hashIdx<HashSize)
	{
		if(0<table[hashIdx].GetN())
		{
			handle.hashIdx=hashIdx;
			handle.arrayIdx=0;
			return YSOK;
		}
		hashIdx++;
	}

	handle.hashIdx=0;
	handle.arrayIdx=0;

	return YSERR;
}

template <class toFind>
YsArray <YsHashElement <toFind> > *YsHashTable <toFind>::FindTableIndex(int &idx,unsigned searchKey) const
{
	int hashId,i1,i2,im;
	YsArray <YsHashElement<toFind> > *entry;

	hashId=searchKey%HashSize;
	entry=&table[hashId];

	if(entry->GetNumItem()==0)
	{
		idx=0;
		return NULL;
	}
	else
	{
		i1=0;
		i2=entry->GetN()-1;
		while(i2-i1>1)
		{
			im=(i1+i2)/2;
			if(searchKey<(*entry)[im].key)
			{
				i2=im;
			}
			else
			{
				i1=im;
			}
		}

		if((*entry)[i1].key==searchKey)
		{
			idx=i1;
			return entry;
		}
		else if((*entry)[i2].key==searchKey)
		{
			idx=i2;
			return entry;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}


template <class toFind>
YSRESULT YsHashTable <toFind>::SelfDiagnostic(void) const
{
	// Check all elements in the table are sorted in search key order
	// Search key and hash id is consistent

	int i,j,nElemSum;
	YSBOOL err;
	YSRESULT res;
	res=YSOK;

	YsPrintf("Self Diagnostic YsHashTable\n");
	nElemSum=0;
	for(i=0; i<HashSize; i++)
	{
		if(i%20==0)
		{
			YsPrintf("%d/%d\r",i,HashSize);
		}

		err=YSFALSE;
		for(j=0; j<table[i].GetN(); j++)
		{
			if(j>0)
			{
				if(table[i][j-1].key>=table[i][j].key)
				{
					res=YSERR;
					err=YSTRUE;
				}
			}
			if(table[i].GetItem(j).key%HashSize!=unsigned(i))
			{
				res=YSERR;
				YsPrintf("##Inconsistencies in hash table %d %d (HashSize=%d)\n",i,table[i][j].key,HashSize);
			}
		}
		if(err==YSTRUE)
		{
			YsPrintf("##Elements are not sorted in key order\n");
			for(j=0; j<table[i].GetN(); j++)
			{
				YsPrintf(" %d",table[i][j].key);
			}
			YsPrintf("\n");
		}
		nElemSum+=table[i].GetN();
	}
	YsPrintf("%d/%d\n",HashSize,HashSize);

	if(nElemSum!=nElem)
	{
		printf("Number of elements does not match.\n");
	}

	return res;
}


template <class toFind>
void YsHashTable <toFind>::EnableAutoResizing(int minSize,int maxSize)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=minSize;
	autoResizingMax=maxSize;
}

template <class toFind>
void YsHashTable <toFind>::EnableAutoResizing(void)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=0;  // 0,0 -> Unrestricted auto resizing
	autoResizingMax=0;
}

template <class toFind>
int YsHashTable <toFind>::GetNumUnused(void) const
{
	int i,n;
	n=0;
	for(i=0; i<HashSize; i++)
	{
		n+=table[i].GetNUnused();
	}
	return n;
}


template <class toFind>
YSRESULT YsHashTable <toFind>::CheckAutoResizingGrow(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0 && autoResizingMin==0)  // Unlimited mode
		{
			if(HashSize*4<nElem)
			{
				Resize(nElem);
			}
		}
		else if(HashSize<autoResizingMax && HashSize*4<nElem)
		{
			Resize(YsSmaller(HashSize*2,autoResizingMax));
		}
	}
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::CheckAutoResizingShrink(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0 && autoResizingMin==0)  // Unlimited mode
		{
			if(nElem<HashSize/4)
			{
				Resize(YsGreater(1,nElem));
			}
		}
		else if(autoResizingMin<HashSize && nElem<HashSize)
		{
			Resize(YsGreater(HashSize/2,autoResizingMin));
		}
	}
	return YSOK;
}


////////////////////////////////////////////////////////////

// Sub Sub Group : Group of elements that have same key

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
class YsHashSameKeyGroup
{
public:
	YsArray <unsigned,minKeyBufSize> key;
	YsArray <toFind,minItemBufSize> dat;
	YsList <YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> > *thisInTheList;
};


extern int YsHashSameKeyGroupCreateDeleteCounter;

// Sub Group : Group of sub groups that have same keysum

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
class YsHashSameKeySumGroup
{
public:
	YsHashSameKeySumGroup();
	~YsHashSameKeySumGroup();
	YsList <YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> > *elmList;
};

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize>::YsHashSameKeySumGroup()
{
	YsHashSameKeyGroupCreateDeleteCounter++;
	elmList=NULL;
}

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize>::~YsHashSameKeySumGroup()
{
	YsHashSameKeyGroupCreateDeleteCounter--;
	elmList->DeleteList();
}



//

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
class YsMultiKeyHashElementEnumHandle
{
public:
	YsHashElementEnumHandle outerHandle;
	YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *currentSubSubGroup;
};



// Table main body

template <class toFind,const int minKeyBufSize=1,const int minItemBufSize=1>
class YsMultiKeyHash  : protected YsHashTable <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>
{
protected:
// Complying with a stupid change made in g++ 3.4
using YsHashTable <YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *>::HashSize;
using YsHashTable <YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *>::table;
// Complying with a stupid change made in g++ 3.4

public:
	YsMultiKeyHash(int hashSize=16);
	~YsMultiKeyHash();

	YSRESULT PrepareTable(void);
	YSRESULT PrepareTable(int hashSize);

	YSRESULT AddElement(int nKey,const unsigned key[],toFind element);

	YSRESULT DeleteElement(int nKey,const unsigned key[],toFind element);

	YSRESULT DeleteKey(int nKey,const unsigned key[]);

	const YsArray <toFind,minItemBufSize> *FindElement(int nKey,const unsigned key[]) const;

	YSRESULT RewindElementEnumHandle
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;
	YSRESULT FindNextElement
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;
	const YsArray <toFind,minItemBufSize> *GetElement
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;
	const YsArray <toFind,minItemBufSize> *GetSeachKey
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;

protected:
	unsigned GetKeySum(int nKey,const unsigned key[]) const;
	YSBOOL CheckSameKey(int nKey1,const unsigned key1[],int nKey2,const unsigned key2[]) const;
	YSRESULT FindAddSameKeyGroup
	   (unsigned &keySum,
		YsArray <unsigned,minKeyBufSize> &key,
	    YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	    YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup);
	YSRESULT FindSameKeyGroup
	   (unsigned &keySum,
		YsArray <unsigned,minKeyBufSize> &key,
	    YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	    YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup) const;
};


template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	YsMultiKeyHash(int hashSize) : YsHashTable <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>(hashSize)
{
}

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
	YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::~YsMultiKeyHash()
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table[i].GetN(); j++)
		{
			delete table[i][j].dat;
		}
	}
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::PrepareTable(void)
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table[i].GetN(); j++)
		{
			delete table[i][j].dat;
		}
	}

	return YsHashTable<YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::PrepareTable();
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::PrepareTable(int hashSize)
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table[i].GetN(); j++)
		{
			delete table[i][j].dat;
		}
	}
	return YsHashTable<YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::PrepareTable(hashSize);
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	AddElement(int nKey,const unsigned unsortedKey[],toFind element)
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);

	if(FindAddSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		sameKeyGroup->dat.AppendItem(element);
		return YSOK;
	}
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	DeleteElement(int nKey,const unsigned unsortedKey[],toFind element)
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);  // key is sorted inside FindSameKeyGroup func

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		int i;
		for(i=0; i<sameKeyGroup->dat.GetNumItem(); i++)
		{
			if(sameKeyGroup->dat.GetItem(i)==element)
			{
				sameKeyGroup->dat.Delete(i);

				if(sameKeyGroup->dat.GetNumItem()==0)
				{
					YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *seek;

					//YSBOOL found;
					//found=YSFALSE;
					//for(seek=sameKeySumGroup->elmList; seek!=NULL; seek=seek->Next())
					//{
					//	if(&seek->dat==sameKeyGroup)
					//	{
					//		sameKeySumGroup->elmList=seek->DeleteFromList();
					//		found=YSTRUE;
					//		break;
					//	}
					//}
					//if(found!=YSTRUE)
					//{
					//	YsErrOut("YsMultiKeyHash::DeleteElement()\n");
					//	YsErrOut("  Internal error (1)\n");
					//}

					// Above part is replaced with >>  2000/06/05
					seek=sameKeyGroup->thisInTheList;
					if(seek->SeekTop()==sameKeySumGroup->elmList)
					{
						sameKeySumGroup->elmList=seek->DeleteFromList();
					}
					else
					{
						YsErrOut("YsMultiKeyHash::DeleteElement()\n");
						YsErrOut("  Internal error (1)\n");
					}
					// <<


					if(sameKeySumGroup->elmList==NULL)
					{
						YsHashTable
						    <YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *>::
						        DeleteElement(keySum,sameKeySumGroup);
						delete sameKeySumGroup;
					}
				}

				return YSOK;
			}
		}
	}
	YsErrOut("YsMultiKeyHash::DeleteElement()\n");
	YsErrOut("  Tried to delete an element that is not listed.\n");
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	DeleteKey(int nKey,const unsigned unsortedKey[])
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);  // key is sorted inside FindSameKeyGroup func

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *seek;

		seek=sameKeyGroup->thisInTheList;
		if(seek->SeekTop()==sameKeySumGroup->elmList)
		{
			sameKeySumGroup->elmList=seek->DeleteFromList();
		}
		else
		{
			YsErrOut("YsMultiKeyHash::DeleteKey()\n");
			YsErrOut("  Internal error (1)\n");
		}

		if(sameKeySumGroup->elmList==NULL)
		{
			YsHashTable
			    <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
			        DeleteElement(keySum,sameKeySumGroup);
			delete sameKeySumGroup;
		}

		return YSOK;
	}
	YsErrOut("YsMultiKeyHash::DeleteKey()\n");
	YsErrOut("  Tried To Delete a key that is not listed.\n");
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
const YsArray <toFind,minItemBufSize> *YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	FindElement(int nKey,const unsigned unsortedKey[]) const
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		return &sameKeyGroup->dat;
	}
	else
	{
		return NULL;
	}
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	RewindElementEnumHandle
    	(YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	YsHashTable <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
	    RewindElementEnumHandle(handle.outerHandle);

	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *subElem;

	handle.currentSubSubGroup=NULL;
	while(handle.currentSubSubGroup==NULL)
	{
		if(YsHashTable
		       <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		           GetElement(subElem,handle.outerHandle)==YSOK)
		{
			if(subElem->elmList!=NULL)
			{
				handle.currentSubSubGroup=subElem->elmList;
				return YSOK;
			}
		}
		else if(YsHashTable
		       <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		           FindNextElement(handle.outerHandle)!=YSOK)
		{
			return YSERR;
		}
	}
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	FindNextElement
    	(YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		handle.currentSubSubGroup=handle.currentSubSubGroup->Next();
		if(handle.currentSubSubGroup!=NULL)
		{
			return YSOK;
		}
	}

	// Falling into this point means either one of:
	//    1. Finding first element
	//    2. One SubGroup is over, and have to move to the next SubGroup
	while(handle.currentSubSubGroup==NULL)
	{
		YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *subGroup;

		if(FindNextElement(subGroup,handle.outerHandle)==YSOK)
		{
			handle.currentSubSubGroup=subGroup->elmList;
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
const YsArray <toFind,minItemBufSize> *
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    GetElement(YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		return &handle.currentSubSubGroup->dat;
	}
	return NULL;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
const YsArray <toFind,minItemBufSize> *
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    GetSeachKey
	        (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		return &handle.currentSubSubGroup->key;
	}
	return NULL;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
unsigned
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    GetKeySum(int nKey,const unsigned key[]) const
{
	int i;
	unsigned sum;
	sum=0;
	for(i=0; i<nKey; i++)
	{
		sum+=key[i];
	}
	return sum;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSBOOL
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    CheckSameKey(int nKey1,const unsigned key1[],int nKey2,const unsigned key2[]) const
{
	if(nKey1==nKey2)
	{
		int i;
		for(i=0; i<nKey1; i++)
		{
			if(key1[i]!=key2[i])
			{
				return YSFALSE;
			}
		}
		return YSTRUE;
	}
	return YSFALSE;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
		FindAddSameKeyGroup
	       (unsigned &keySum,
			YsArray <unsigned,minKeyBufSize> &key,
	        YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	        YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup)
{
	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		return YSOK;
	}
	else
	{
		if(sameKeySumGroup==NULL)
		{
			sameKeySumGroup=new YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize>;
			YsHashTable
		       <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		           AddElement(keySum,sameKeySumGroup);  // Pray for no error  :-)
		}

		YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *neo;

		neo=new YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> >;
		neo->dat.key=key;
		neo->dat.thisInTheList=neo;

		sameKeySumGroup->elmList=sameKeySumGroup->elmList->Append(neo);

		sameKeyGroup=&neo->dat;

		return YSOK;
	}
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
		FindSameKeyGroup
	       (unsigned &keySum,
			YsArray <unsigned,minKeyBufSize> &key,
	        YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	        YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup)
	            const
{
	sameKeySumGroup=NULL;
	sameKeyGroup=NULL;

	keySum=GetKeySum(key.GetNumItem(),key.GetArray());

	YsQuickSort <unsigned,void *> (key.GetNumItem(),key.GetEditableArray());

	if(YsHashTable
		  <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		      FindElement(sameKeySumGroup,keySum)==YSOK)
		          // sameKeySumGroup : Group of sub elements that have same keysum
	{
		YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *seek;

		sameKeyGroup=NULL;
		for(seek=sameKeySumGroup->elmList; seek!=NULL; seek=seek->Next())
		{
			int nKey1,nKey2;
			const unsigned *key1,*key2;

			nKey1=seek->dat.key.GetNumItem();
			key1=seek->dat.key.GetArray();
			nKey2=key.GetNumItem();
			key2=key.GetArray();

			if(CheckSameKey(nKey1,key1,nKey2,key2)==YSTRUE)
			{
				sameKeyGroup=&seek->dat;
				return YSOK;
			}
		}
	}
	return YSERR;
}




////////////////////////////////////////////////////////////

class YsFixedLengthToMultiHashElemEnumHandle
{
public:
	int idx1,idx2,objectiveIdx;
};

template <class toFind,int nKeyLng,int nDefObjLng>
class YsFixedLengthToMultiHashElement
{
public:
	unsigned key[nKeyLng];
	YsArray <toFind,nDefObjLng> objective;
};

template <class toFind,int nKeyLng,int nDefObjLng>
class YsFixedLengthToMultiHashTable
{
public:
	YsFixedLengthToMultiHashTable(int hSize=16);
	YSRESULT PrepareTable(void);
	YSRESULT PrepareTable(int hashSize);
	YSRESULT Resize(int hashSize);
	YSRESULT CollectGarbage(void);
	YSRESULT AddElement(int nKey,const unsigned unorderedKey[],const toFind &objective);
	YSRESULT DeleteElement(int nKey,const unsigned unorderedKey[],const toFind &objective);
	YSRESULT DeleteKey(int nKey,const unsigned unorderedKey[]);
	const YsArray <toFind,nDefObjLng> *FindElement(int nKey,const unsigned unorderedKey[]) const;
	YSRESULT FindElement(int &nFind,const toFind *&find,int nKey,const unsigned unorderedKey[]) const;
	YSBOOL CheckKeyExist(int nKey,const unsigned unorderedKey[]) const;
	YSRESULT RewindElemEnumHandle(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	YSRESULT FindNextElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	YSRESULT FindNextKey(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	const toFind &GetElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	const unsigned *GetKey(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	YSRESULT OrderKey(unsigned orderedKey[],int nKey,const unsigned unorderedKey[]) const;
	YSRESULT FindEntryIndex(int &idx1,int &idx2,int nKey,const unsigned orderedKey[]) const;
	const YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *FindEntry(int nKey,const unsigned orderedKey[]) const;
	YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *FindEntry(int nKey,const unsigned orderedKey[]);
	YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *CreateEntry(int nKey,const unsigned orderedKey[]);

	void EnableAutoResizing(int minSize,int maxSize);
	void EnableAutoResizing(void);

	int GetNumUnused(void) const;

	const char *tableName;

protected:
	YSRESULT CheckAutoResizingGrow(void);
	YSRESULT CheckAutoResizingShrink(void);

	YSBOOL enableAutoResizing;
	int autoResizingMin,autoResizingMax;

	int hashSize;
	int nElem;
	YsArray <YsArray <YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> > > keyList;
};


template <class toFind,int nKeyLng,int nDefObjLng>
YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::YsFixedLengthToMultiHashTable(int hSize)
{
	keyList.Set(hSize,NULL);
	hashSize=hSize;
	nElem=0;
	enableAutoResizing=YSFALSE;
	autoResizingMin=0;
	autoResizingMax=0;

	tableName="";
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::PrepareTable(void)
{
	int i;
	for(i=0; i<hashSize; i++)
	{
		keyList[i].Set(0,NULL);
	}
	nElem=0;
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::PrepareTable(int hSize)
{
	int i;
	keyList.Set(0,NULL);  // <- This will invoke Shrink. 2005/05/20
	keyList.Set(hSize,NULL);
	hashSize=hSize;
	for(i=0; i<hashSize; i++)
	{
		keyList[i].Set(0,NULL);
	}
	nElem=0;
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::Resize(int newHashSize)
{
	int i,j,idx1,keySum;
	YsArray <YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> > elemList;
	for(i=0; i<hashSize; i++)
	{
		elemList.Append(keyList[i].GetN(),keyList[i]);
	}
	PrepareTable(newHashSize);  // <- This will reset nElem
	for(i=0; i<elemList.GetN(); i++)
	{
		keySum=0;
		for(j=0; j<nKeyLng; j++)
		{
			keySum+=elemList[i].key[j];;
		}
		idx1=keySum%hashSize;
		keyList[idx1].Append(elemList[i]);
		nElem+=elemList[i].objective.GetN();
	}
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::CollectGarbage(void)
{
	Resize(hashSize);
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::AddElement(
    int nKey,const unsigned unorderedKey[],const toFind &objective)
{
	unsigned orderedKey[nKeyLng];
	YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *entry;

	if(nKey==nKeyLng)
	{
		OrderKey(orderedKey,nKey,unorderedKey);
		entry=FindEntry(nKey,orderedKey);
		if(entry!=NULL)
		{
			entry->objective.Append(objective);
			nElem++;
			CheckAutoResizingGrow();
			return YSOK;
		}
		else
		{
			entry=CreateEntry(nKey,orderedKey);
			if(entry!=NULL)
			{
				entry->objective.Append(objective);
				nElem++;
				CheckAutoResizingGrow();
				return YSOK;
			}
			else
			{
				YsErrOut("YsFixedLengthToMultiHashTable::AddElement()\n");
				YsErrOut("  Out of memory warning.\n");
				return YSERR;
			}
		}
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::AddElement()\n");
		YsErrOut("  Key length does not match.\n");
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::DeleteElement(
    int nKey,const unsigned unorderedKey[],const toFind &objective)
{
	unsigned orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		int idx1,idx2;
		OrderKey(orderedKey,nKey,unorderedKey);
		if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
		{
			int i;
			for(i=0; i<keyList[idx1][idx2].objective.GetN(); i++)
			{
				if(keyList[idx1][idx2].objective[i]==objective)
				{
					keyList[idx1][idx2].objective.DeleteBySwapping(i);
					if(keyList[idx1][idx2].objective.GetN()==0)
					{
						keyList[idx1].DeleteBySwapping(idx2);
					}
					nElem--;
					CheckAutoResizingShrink();
					return YSOK;
				}
			}
		}
		YsErrOut("YsFixedLengthToMultiHashTable::DeleteElement()\n");
		YsErrOut("  Objective not found.\n");
		return YSERR;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::DeleteElement()\n");
		YsErrOut("  Key length does not match.\n");
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::DeleteKey(int nKey,const unsigned unorderedKey[])
{
	unsigned orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		int idx1,idx2;
		OrderKey(orderedKey,nKey,unorderedKey);
		if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
		{
			nElem-=keyList[idx1][idx2].objective.GetN();
			keyList[idx1].DeleteBySwapping(idx2);
			CheckAutoResizingShrink();
			return YSOK;
		}
		return YSERR;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::DeleteKey()\n");
		YsErrOut("  Key length does not match.\n");
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
const YsArray <toFind,nDefObjLng> *YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindElement
   (int nKey,const unsigned unorderedKey[]) const
{
	const YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *entry;
	unsigned orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		OrderKey(orderedKey,nKey,unorderedKey);
		entry=FindEntry(nKey,orderedKey);
		if(entry!=NULL)
		{
			return &entry->objective;
		}
		return NULL;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::FindElement()\n");
		YsErrOut("  Key length does not match.\n");
		return NULL;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindElement
   (int &nFind,const toFind *&find,int nKey,const unsigned unorderedKey[]) const
{
	const YsArray <toFind,nDefObjLng> *foundArray;
	foundArray=FindElement(nKey,unorderedKey);
	if(foundArray!=NULL && foundArray->GetN()>0)
	{
		nFind=foundArray->GetN();
		find=foundArray->GetArray();
		return YSOK;
	}
	else
	{
		nFind=0;
		find=NULL;
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSBOOL YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::CheckKeyExist
   (int nKey,const unsigned unorderedKey[]) const
{
	const YsArray <toFind,nDefObjLng> *entry;
	entry=FindElement(nKey,unorderedKey);
	if(entry!=NULL && entry->GetN()>0)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::RewindElemEnumHandle
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	int i;
	for(i=0; i<hashSize; i++)
	{
		int j;
		for(j=0; j<keyList[i].GetN(); j++)
		{
			if(keyList[i][j].objective.GetN()>0)
			{
				elHd.idx1=i;
				elHd.idx2=j;
				elHd.objectiveIdx=0;
				return YSOK;
			}
		}
	}
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindNextElem
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	elHd.objectiveIdx++;
	if(elHd.objectiveIdx<keyList[elHd.idx1][elHd.idx2].objective.GetN())
	{
		return YSOK;
	}
	elHd.idx2++;
	elHd.objectiveIdx=0;
	if(elHd.idx2<keyList[elHd.idx1].GetN())
	{
		return YSOK;
	}

	int i;
	for(i=elHd.idx1+1; i<hashSize; i++)
	{
		int j;
		for(j=0; j<keyList[i].GetN(); j++)
		{
			if(keyList[i][j].objective.GetN()>0)
			{
				elHd.idx1=i;
				elHd.idx2=j;
				elHd.objectiveIdx=0;
				return YSOK;
			}
		}
	}
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindNextKey
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	elHd.idx2++;
	elHd.objectiveIdx=0;
	if(elHd.idx2<keyList[elHd.idx1].GetN())
	{
		return YSOK;
	}

	int i;
	for(i=elHd.idx1+1; i<hashSize; i++)
	{
		int j;
		for(j=0; j<keyList[i].GetN(); j++)
		{
			if(keyList[i][j].objective.GetN()>0)
			{
				elHd.idx1=i;
				elHd.idx2=j;
				elHd.objectiveIdx=0;
				return YSOK;
			}
		}
	}
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng>
const toFind &YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::GetElem
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return keyList[elHd.idx1][elHd.idx2].objective[elHd.objectiveIdx];
}

template <class toFind,int nKeyLng,int nDefObjLng>
const unsigned *YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::GetKey
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return keyList[elHd.idx1][elHd.idx2].key;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::OrderKey
   (unsigned orderedKey[],int nKey,const unsigned unorderedKey[]) const
{
	// nKeyLng is given as a template parameter (constant).  I hope optimizer is smart enough!
	if(nKeyLng==1)
	{
		orderedKey[0]=unorderedKey[0];
		return YSOK;
	}
	else if(nKeyLng==2)
	{
		if(unorderedKey[0]<unorderedKey[1])
		{
			orderedKey[0]=unorderedKey[0];
			orderedKey[1]=unorderedKey[1];
		}
		else
		{
			orderedKey[0]=unorderedKey[1];
			orderedKey[1]=unorderedKey[0];
		}
		return YSOK;
	}
	else if(nKeyLng==3)
	{
		if(unorderedKey[0]<unorderedKey[1] && unorderedKey[0]<unorderedKey[2])
		{
			orderedKey[0]=unorderedKey[0];
			if(unorderedKey[1]<unorderedKey[2])
			{
				orderedKey[1]=unorderedKey[1];
				orderedKey[2]=unorderedKey[2];
			}
			else
			{
				orderedKey[1]=unorderedKey[2];
				orderedKey[2]=unorderedKey[1];
			}
		}
		else if(unorderedKey[1]<unorderedKey[2])
		{
			orderedKey[0]=unorderedKey[1];
			if(unorderedKey[0]<unorderedKey[2])
			{
				orderedKey[1]=unorderedKey[0];
				orderedKey[2]=unorderedKey[2];
			}
			else
			{
				orderedKey[1]=unorderedKey[2];
				orderedKey[2]=unorderedKey[0];
			}
		}
		else
		{
			orderedKey[0]=unorderedKey[2];
			if(unorderedKey[0]<unorderedKey[1])
			{
				orderedKey[1]=unorderedKey[0];
				orderedKey[2]=unorderedKey[1];
			}
			else
			{
				orderedKey[1]=unorderedKey[1];
				orderedKey[2]=unorderedKey[0];
			}
		}
		return YSOK;
	}
	else
	{
		int i;
		for(i=0; i<nKey; i++)
		{
			orderedKey[i]=unorderedKey[i];
		}
		return YsQuickSort <unsigned,unsigned> (nKey,orderedKey,NULL);
	}
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindEntryIndex
   (int &idx1,int &idx2,int nKey,const unsigned orderedKey[]) const
{
	unsigned keySum;
	int i;
	keySum=0;
	for(i=0; i<nKey; i++)
	{
		keySum+=orderedKey[i];
	}

	idx1=keySum%hashSize;
	for(idx2=0; idx2<keyList[idx1].GetN(); idx2++)
	{
		for(i=0; i<nKey; i++)
		{
			if(keyList[idx1][idx2].key[i]!=orderedKey[i])
			{
				goto NEXTIDX2;
			}
		}
		return YSOK;
	NEXTIDX2:
		;
	}
	idx1=-1;
	idx2=-1;
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng>
const YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindEntry
       (int nKey,const unsigned orderedKey[]) const
{
	int idx1,idx2;
	if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
	{
		return &keyList[idx1][idx2];
	}
	return NULL;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::FindEntry
       (int nKey,const unsigned orderedKey[])
{
	int idx1,idx2;
	if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
	{
		return &keyList[idx1][idx2];
	}
	return NULL;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::CreateEntry
       (int nKey,const unsigned orderedKey[])
{
	unsigned keySum;
	int i;
	keySum=0;
	for(i=0; i<nKey; i++)
	{
		keySum+=orderedKey[i];
	}

	int idx1,idx2;
	idx1=keySum%hashSize;
	idx2=keyList[idx1].GetN();

	YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> newElem;
	for(i=0; i<nKey; i++)
	{
		newElem.key[i]=orderedKey[i];
	}
	keyList[idx1].Append(newElem);
	return &keyList[idx1][idx2];
}

template <class toFind,int nKeyLng,int nDefObjLng>
void YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::EnableAutoResizing(int minSize,int maxSize)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=minSize;
	autoResizingMax=maxSize;
}

template <class toFind,int nKeyLng,int nDefObjLng>
void YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::EnableAutoResizing(void)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=0;
	autoResizingMax=0;
}

template <class toFind,int nKeyLng,int nDefObjLng>
int YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::GetNumUnused(void) const
{
	int i,n;
	YsArray <YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> > elemList;
	n=0;
	for(i=0; i<hashSize; i++)
	{
		n+=keyList[i].GetNUnused();
//		int j;
//		for(j=0; j<keyList[j].GetN(); j++)
//		{
//			n+=keyList[i][j].objective.GetNUnused();
//		}
	}
	return n;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::CheckAutoResizingGrow(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0)
		{
			if(hashSize*4<nElem)
			{
				Resize(nElem);
			}
		}
		else if(hashSize<autoResizingMax && (hashSize<<2)<nElem)
		{
			Resize(YsSmaller(hashSize*2,autoResizingMax));
		}
	}
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng>::CheckAutoResizingShrink(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMin==0)
		{
			if(nElem<hashSize/4)
			{
				Resize(YsGreater(nElem,1));
			}
		}
		else if(autoResizingMin<hashSize && nElem<hashSize)
		{
			Resize(YsGreater(hashSize/2,autoResizingMin));
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

class YsKeyStore
{
public:
	YsKeyStore(int nTable=16);
	~YsKeyStore();
	void CleanUp(void);

	void PrepareTable(void);
	void EnableAutoResizing(int minSize,int maxSize);
	void EnableAutoResizing(void);
	void Resize(int nTable);
	int GetNumKey(void) const;
	int GetNumTable(void) const;

	YSRESULT AddKey(unsigned int key);
	YSRESULT DeleteKey(unsigned int key);
	YSBOOL IsIncluded(unsigned int key) const;

protected:
	void CheckResizeGrow(void);
	void CheckResizeShrink(void);

	YSBOOL autoResizing;
	int autoResizeMin,autoResizeMax;
	int nTab,nKey;
	YsArray <unsigned int> **keyArray;
};

////////////////////////////////////////////////////////////

class YsTwoKeyStore
{
public:
	YsTwoKeyStore(int nTable=16);
	~YsTwoKeyStore();
	void CleanUp(void);

	void PrepareTable(void);
	void EnableAutoResizing(int minSize,int maxSize);
	void EnableAutoResizing(void);
	void Resize(int nTable);
	int GetNumKey(void) const;
	int GetNumTable(void) const;

	YSRESULT AddKey(unsigned int key1,unsigned int key2);
	YSRESULT DeleteKey(unsigned int key1,unsigned int key2);
	YSBOOL IsIncluded(unsigned int key1,unsigned int key2) const;

protected:
	void CheckResizeGrow(void);
	void CheckResizeShrink(void);

	YSBOOL autoResizing;
	int autoResizeMin,autoResizeMax;
	int nTab,nKey;
	YsArray <unsigned int> **keyArray;
};



/* } */
#endif



