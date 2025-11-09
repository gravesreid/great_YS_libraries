#include <stdio.h>

#include "ysdef.h"
#include "yslist.h"
#include "ysmath.h"
#include "ysarray.h"
#include "yshash.h"

int YsHashSameKeyGroupCreateDeleteCounter=0;


YsKeyStore::YsKeyStore(int nTable)
{
	int i;
	keyArray=new YsArray <unsigned int> *[nTable];
	for(i=0; i<nTable; i++)
	{
		keyArray[i]=NULL;
	}
	autoResizing=YSFALSE;
	nKey=0;
	nTab=nTable;
}

YsKeyStore::~YsKeyStore()
{
	CleanUp();
	delete [] keyArray;
}

void YsKeyStore::CleanUp(void)
{
	int i;
	for(i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}
	nKey=0;
}

void YsKeyStore::PrepareTable(void)
{
	CleanUp();
}

void YsKeyStore::EnableAutoResizing(int minSize,int maxSize)
{
	autoResizing=YSTRUE;
	autoResizeMin=minSize;
	autoResizeMax=maxSize;
}

void YsKeyStore::EnableAutoResizing(void)
{
	autoResizing=YSTRUE;
	autoResizeMin=0;
	autoResizeMax=0;
}

void YsKeyStore::Resize(int newNTable)
{
	int i,j,k;
	YsArray <unsigned int> allKey;
	allKey.Set(nKey,NULL);
	k=0;

	for(i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			for(j=0; j<keyArray[i]->GetN(); j++)
			{
				allKey[k++]=(*keyArray[i])[j];
			}
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}

	delete [] keyArray;
	keyArray=new YsArray <unsigned int> * [newNTable];
	nTab=newNTable;
	for(i=0; i<nTab; i++)
	{
		keyArray[i]=NULL;
	}

	forYsArray(i,allKey)
	{
		int tabId;
		tabId=allKey[i]%nTab;
		if(keyArray[tabId]==NULL)
		{
			keyArray[tabId]=new YsArray <unsigned int>;
		}
		keyArray[tabId]->Append(allKey[i]);
	}
}

YSRESULT YsKeyStore::AddKey(unsigned int key)
{
	int i;
	unsigned int tabId;

	tabId=key%nTab;
	if(keyArray[tabId]==NULL)
	{
		keyArray[tabId]=new YsArray <unsigned int>;
	}
	for(i=0; i<keyArray[tabId]->GetN(); i++)
	{
		if((*keyArray[tabId])[i]==key)
		{
			return YSERR;  // Already in.
		}
	}

	keyArray[tabId]->Append(key);
	nKey++;

	CheckResizeGrow();

	return YSOK;
}

YSRESULT YsKeyStore::DeleteKey(unsigned int key)
{
	int i;
	unsigned int tabId;

	tabId=key%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSERR;  // Key doesn't exist.
	}

	for(i=0; i<keyArray[tabId]->GetN(); i++)
	{
		if((*keyArray[tabId])[i]==key)
		{
			keyArray[tabId]->DeleteBySwapping(i);
			nKey--;
			if(keyArray[tabId]->GetN()==0)
			{
				delete keyArray[tabId];
				keyArray[tabId]=NULL;
			}

			CheckResizeShrink();

			return YSOK;
		}
	}

	return YSERR; // Key doesn't exist.
}

YSBOOL YsKeyStore::IsIncluded(unsigned int key) const
{
	int i;
	unsigned int tabId;
	tabId=key%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSFALSE;
	}

	for(i=0; i<keyArray[tabId]->GetN(); i++)
	{
		if((*keyArray[tabId])[i]==key)
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

void YsKeyStore::CheckResizeGrow(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMax==0)
		{
			if(nTab*4<nKey)
			{
				Resize(nKey);
			}
		}
		else if(nTab<autoResizeMax && nTab*4<nKey)
		{
			Resize(YsSmaller(nTab*2,autoResizeMax));
		}
	}
}

void YsKeyStore::CheckResizeShrink(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMin==0)
		{
			if(nKey<nTab/4)
			{
				Resize(YsGreater(nKey,1));
			}
		}
		else if(autoResizeMin<nTab && nKey<nTab)
		{
			Resize(YsGreater(nTab/2,autoResizeMin));
		}
	}
}

int YsKeyStore::GetNumKey(void) const
{
	return nKey;
}

int YsKeyStore::GetNumTable(void) const
{
	return nTab;
}

////////////////////////////////////////////////////////////

YsTwoKeyStore::YsTwoKeyStore(int nTable)
{
	int i;
	keyArray=new YsArray <unsigned int> *[nTable];
	for(i=0; i<nTable; i++)
	{
		keyArray[i]=NULL;
	}
	autoResizing=YSFALSE;
	nKey=0;
	nTab=nTable;
}

YsTwoKeyStore::~YsTwoKeyStore()
{
	CleanUp();
	delete [] keyArray;
}

void YsTwoKeyStore::CleanUp(void)
{
	int i;
	for(i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}
	nKey=0;
}

void YsTwoKeyStore::PrepareTable(void)
{
	CleanUp();
}

void YsTwoKeyStore::EnableAutoResizing(int minSize,int maxSize)
{
	autoResizing=YSTRUE;
	autoResizeMin=minSize;
	autoResizeMax=maxSize;
}

void YsTwoKeyStore::EnableAutoResizing(void)
{
	autoResizing=YSTRUE;
	autoResizeMin=0;
	autoResizeMax=0;
}

void YsTwoKeyStore::Resize(int newNTable)
{
	int i,j,k;
	YsArray <unsigned int> allKey;
	allKey.Set(nKey*2,NULL);
	k=0;

	for(i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			for(j=0; j<keyArray[i]->GetN(); j++)
			{
				allKey[k++]=(*keyArray[i])[j];
			}
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}

	delete [] keyArray;
	keyArray=new YsArray <unsigned int> * [newNTable];
	nTab=newNTable;
	for(i=0; i<nTab; i++)
	{
		keyArray[i]=NULL;
	}

	for(i=0; i<=allKey.GetN()-2; i+=2)
	{
		int tabId;
		tabId=(allKey[i]+allKey[i+1])%nTab;
		if(keyArray[tabId]==NULL)
		{
			keyArray[tabId]=new YsArray <unsigned int>;
		}
		keyArray[tabId]->Append(2,((unsigned int *)allKey)+i);
	}
}

YSRESULT YsTwoKeyStore::AddKey(unsigned int key1,unsigned int key2)
{
	int i;
	unsigned int tabId;
	unsigned int key[2];

	if(key1<key2)
	{
		key[0]=key1;
		key[1]=key2;
	}
	else
	{
		key[0]=key2;
		key[1]=key1;
	}

	tabId=(key1+key2)%nTab;
	if(keyArray[tabId]==NULL)
	{
		keyArray[tabId]=new YsArray <unsigned int>;
	}

	YsArray <unsigned int> &entry=*keyArray[tabId];
	for(i=0; i<=entry.GetN()-2; i+=2)
	{
		if(entry[i]==key[0] && entry[i+1]==key[1])
		{
			return YSERR;  // Already in.
		}
	}

	entry.Append(2,key);
	nKey++;

	CheckResizeGrow();

	return YSOK;
}

YSRESULT YsTwoKeyStore::DeleteKey(unsigned int key1,unsigned int key2)
{
	int i;
	unsigned int tabId;
	unsigned int key[2];

	if(key1<key2)
	{
		key[0]=key1;
		key[1]=key2;
	}
	else
	{
		key[0]=key2;
		key[1]=key1;
	}

	tabId=(key1+key2)%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSERR;  // Key doesn't exist.
	}

	YsArray <unsigned int> &entry=*keyArray[tabId];
	for(i=0; i<=entry.GetN()-2; i+=2)
	{
		if(entry[i]==key[0] && entry[i+1]==key[1])
		{
			entry[i  ]=entry[entry.GetN()-2];
			entry[i+1]=entry[entry.GetN()-1];
			entry.Resize(entry.GetN()-2);

			nKey--;
			if(entry.GetN()==0)
			{
				delete keyArray[tabId];
				keyArray[tabId]=NULL;
			}

			CheckResizeShrink();

			return YSOK;
		}
	}

	return YSERR; // Key doesn't exist.
}

YSBOOL YsTwoKeyStore::IsIncluded(unsigned int key1,unsigned int key2) const
{
	int i;
	unsigned int tabId;
	unsigned int key[2];

	if(key1<key2)
	{
		key[0]=key1;
		key[1]=key2;
	}
	else
	{
		key[0]=key2;
		key[1]=key1;
	}

	tabId=(key1+key2)%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSFALSE;
	}

	YsArray <unsigned int> &entry=*keyArray[tabId];
	for(i=0; i<=entry.GetN()-2; i+=2)
	{
		if(entry[i]==key[0] && entry[i+1]==key[1])
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

void YsTwoKeyStore::CheckResizeGrow(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMax==0)
		{
			if(nTab*4<nKey)
			{
				Resize(nKey);
			}
		}
		else if(nTab<autoResizeMax && nTab*4<nKey)
		{
			Resize(YsSmaller(nTab*2,autoResizeMax));
		}
	}
}

void YsTwoKeyStore::CheckResizeShrink(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMin==0)
		{
			if(nKey<nTab/4)
			{
				Resize(YsGreater(nKey,1));
			}
		}
		else if(autoResizeMin<nTab && nKey<nTab)
		{
			Resize(YsGreater(nTab/2,autoResizeMin));
		}
	}
}

int YsTwoKeyStore::GetNumKey(void) const
{
	return nKey;
}

int YsTwoKeyStore::GetNumTable(void) const
{
	return nTab;
}

