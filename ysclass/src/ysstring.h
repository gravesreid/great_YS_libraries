#ifndef YSSTRING_IS_INCLUDED
#define YSSTRING_IS_INCLUDED
/* { */

#include <ctype.h>

void YsStringHead(char **line);
void YsStringTail(char *l);
void YsCapitalize(char l[]);
void YsUncapitalize(char l[]);
const char *YsOptimizeDoubleString(char buf[],const double &value);

// For parsing CSV, blank="", comma=","
void YsArguments2(YsArray <class YsString,16> &args,const char str[],const char *blank=" \t",const char *comma=",");
void YsArguments2(YsArray <class YsString,16> &args,const char str[],int l,const char *blank=" \t",const char *comma=",");

////////////////////////////////////////////////////////////

class YsString : protected YsArray <char,16>
{
public:
	inline YsString();
	inline YsString(const char str[]);

	inline int Strlen(void) const;
	inline const char &operator[](int idx) const;
	inline const char &operator[](int idx);
	inline operator const char *() const;
	inline operator const char *();
	inline const char *Ptr(void ) const;
	inline const char *Txt(void ) const; // 2009/03/27 To make it look like YsVec3/YsVec2
	inline const char *GetArray(void) const;
	inline YSRESULT Arguments(YsArray <YsString,16> &args,const char *blank=" \t",const char *comma=",") const;
	inline char LastChar(void) const;

	inline YSRESULT SeparatePathFile(YsString &pth,YsString &fil) const;

	// inline char &operator[](int idx);
	// inline operator char *();
	// inline char *GetEditableArray(void);
	inline YSRESULT Set(const char str[]);
	inline YSRESULT Set(int n,const char str[]);
	inline YSRESULT Set(int n,char c);
	inline YSRESULT Append(const char str[]);
	inline YSRESULT Append(char c);
	inline YSRESULT BackSpace(void);
	inline void DeleteHeadSpace(void);
	inline void DeleteTailSpace(void);
	inline void Capitalize(void);
	inline void Uncapitalize(void);
	inline void SetLength(int n);
	inline const char *Fgets(FILE *fp);

	inline void ReplaceExtension(const char ext[]);
	inline void RemoveExtension(void);
	void MakeFullPathName(const char path[],const char name[]);

	int Printf(const char *fom,...);
};

inline YsString::YsString()
{
	YsArray <char,16>::Set(1,"");
}

inline YsString::YsString(const char str[])
{
	Set(str);
}

inline int YsString::Strlen(void) const
{
	return GetN()-1;
}

inline const char &YsString::operator[](int idx) const
{
	return vv[idx];
}

inline const char &YsString::operator[](int idx)
{
	return vv[idx];
}

inline YsString::operator const char *() const
{
	return vv;
}

inline YsString::operator const char *()
{
	return vv;
}

inline const char *YsString::Ptr(void) const
{
	return vv;
}

inline const char *YsString::Txt(void) const
{
	return vv;
}

inline const char *YsString::GetArray(void) const
{
	return vv;
}

inline YSRESULT YsString::Arguments(YsArray <YsString,16> &args,const char *blank,const char *comma) const
{
	YsArguments2(args,vv,Strlen(),blank,comma);
	return YSOK;
}

inline char YsString::LastChar(void) const
{
	if(Strlen()>0)
	{
		return vv[Strlen()-1];
	}
	return 0;
}

inline YSRESULT YsString::SeparatePathFile(YsString &pth,YsString &fil) const
{
	int cutPtr,seekPtr;

	cutPtr=0;

	/* Skip Drive Name */
	if(vv[0]!=0 && vv[1]==':')
	{
		cutPtr+=2;
	}

	/* scan until nul Stopper */
	for(seekPtr=cutPtr; vv[seekPtr]!=0; seekPtr++)
	{
		if((vv[seekPtr]&0x80)!=0 && vv[seekPtr+1]!=0)  // 2 byte char
		{
			seekPtr++;
		}
		else if(vv[seekPtr]=='\\' || vv[seekPtr]=='/' || vv[seekPtr]==':')
		{
			cutPtr=seekPtr+1;
		}
	}

	/* cutPtr points *tmp  or after ':' or after last '\\' */
	fil.Set(vv+cutPtr);

	int i;
	pth.Resize(cutPtr+1);
	for(i=0; i<cutPtr; i++)
	{
		pth.vv[i]=vv[i];
	}
	pth.vv[cutPtr]=0;

	return YSOK;
}

inline int operator==(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)==0);
}

inline int operator!=(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)!=0);
}

inline int operator<(const YsString &str1,const YsString &str2)
{
	// str1<str2   <=>  str1-str2<0
	return (strcmp(str1,str2)<0);
}

inline int operator>(const YsString &str1,const YsString &str2)
{
	// str1>str2   <=>  str1-str2>0
	return (strcmp(str1,str2)>0);
}

inline int operator<=(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)<=0);
}

inline int operator>=(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)>=0);
}

// inline char &YsString::operator[](int idx)
// {
// 	return vv[idx];
// }
//
// inline YsString::operator char *()
// {
// 	return vv;
// }
//
// inline char *YsString::GetEditableArray(void)
// {
// 	return vv;
// }

inline YSRESULT YsString::Set(const char str[])
{
	return YsArray <char,16>::Set((int)strlen(str)+1,str);
}

inline YSRESULT YsString::Set(int n,const char str[])
{
	int l;
	l=(int)strlen(str);
	if(l<n)
	{
		n=l;
	}

	if(YsArray <char,16>::Set(n,str)==YSOK)
	{
		YsArray <char,16>::Append(0);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

inline YSRESULT YsString::Set(int n,char c)
{
	vv[n]=c;
	if(c==0 && n<GetN()-1)
	{
		Resize(n+1);
	}
	return YSOK;
}

inline YSRESULT YsString::Append(const char str[])
{
	Resize(GetN()-1);
	return YsArray <char,16>::Append((int)strlen(str)+1,str);
}

inline YSRESULT YsString::Append(char c)
{
	int n;
	n=Strlen();
	Resize(n+2);
	vv[n]=c;
	vv[n+1]=0;
	return YSOK;
}

inline YSRESULT YsString::BackSpace(void)
{
	if(Strlen()>0)
	{
		int l;
		l=Strlen();
		vv[l-1]=0;
		Resize(l);
		return YSOK;
	}
	return YSERR;
}

inline void YsString::DeleteHeadSpace(void)
{
	int i,nDel;
	nDel=0;
	for(i=0; vv[i]!=0; i++)
	{
		nDel=i;
		if(isprint(vv[i]) && vv[i]!=' ' && vv[i]!='\t')
		{
			break;
		}
	}

	if(nDel>0)
	{
		for(i=nDel; vv[i]!=0; i++)
		{
			vv[i-nDel]=vv[i];
		}
		vv[i-nDel]=0;
		Resize(GetN()-nDel);
	}
}

inline void YsString::DeleteTailSpace(void)
{
	unsigned int chr;
	int i,cut,len;

	len=Strlen();
	cut=0;
	for(i=0; i<len; i++)
	{
		chr=((unsigned char *)vv)[i];

		if((0x81<=chr && chr<=0x9f) || 0xe0<=chr)
		{
			cut=i+2;
			i++;
		}
		else if(isprint(chr) && chr!=' ' && chr!='\t')
		{
			cut=i+1;
		}
	}

	if(cut<len)
	{
		vv[cut]=0;
		Resize(cut+1);
	}
}

inline void YsString::Capitalize(void)
{
	int i;
	for(i=0; vv[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)vv)[i] && ((unsigned char*)vv)[i]<=0x9f) || 0xe0<=((unsigned char*)vv)[i])
		{
			i++;
			if(vv[i]==0)
			{
				break;
			}
		}
		else if('a'<=vv[i] && vv[i]<='z')
		{
			vv[i]=vv[i]+'A'-'a';
		}
	}
}

inline void YsString::Uncapitalize(void)
{
	int i;
	for(i=0; vv[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)vv)[i] && ((unsigned char*)vv)[i]<=0x9f) || 0xe0<=((unsigned char*)vv)[i])
		{
			i++;
			if(vv[i]==0)
			{
				break;
			}
		}
		else if('A'<=vv[i] && vv[i]<='Z')
		{
			vv[i]=vv[i]+'a'-'A';
		}
	}
}

inline void YsString::SetLength(int n)
{
	if(0<n)
	{
		int i,k;

		k=Strlen();

		Resize(n+1);
		for(i=k; i<n; i++)
		{
			vv[i]=' ';
		}
		vv[n]=0;
	}
}

inline const char *YsString::Fgets(FILE *fp)
{
	char buf[256];
	YsArray <char,16>::Set(0,NULL);
	while(fgets(buf,255,fp)!=NULL)
	{
		int l;
		l=(int)strlen(buf);
		YsArray <char,16>::Append(l,buf);
		if(buf[l-1]=='\n')
		{
			vv[GetN()-1]=0;
			break;
		}
	}

	if(GetN()==0)
	{
		Set("");
		return NULL;
	}

	if(vv[GetN()-1]!=0)  // In case /n is not at the end of the file 2006/10/26
	{
		YsArray <char,16>::Append(0);
	}

	return vv;
}

inline void YsString::ReplaceExtension(const char ext[])
{
	int i,l,insPoint;
	int extLen;
	const char *extWithoutDot;

	if(ext==NULL || ext[0]==0)
	{
		RemoveExtension();
		return;
	}

	if(ext[0]=='.')
	{
		extLen=(int)strlen(ext)-1;
		extWithoutDot=ext+1;
	}
	else
	{
		extLen=(int)strlen(ext);
		extWithoutDot=ext;
	}

	l=Strlen();
	for(i=l-1; i>=0; i--)
	{
		if(vv[i]=='.' || vv[i]=='/' || vv[i]=='\\')
		{
			break;
		}
	}

	if(i<0 || vv[i]!='.') // Means there's no dot in the filename.
	{
		insPoint=Strlen();
		YsArray <char,16>::Resize(Strlen()+extLen+2);
	}
	else
	{
		insPoint=i;
		YsArray <char,16>::Resize(insPoint+extLen+2);
	}

	vv[insPoint]='.';
	strcpy(vv+insPoint+1,extWithoutDot);
}

inline void YsString::RemoveExtension(void)
{
	int i;
	for(i=Strlen()-1; i>=0; i--)
	{
		if(vv[i]=='.')
		{
			vv[i]=0;
			YsArray <char,16>::Resize(i+1);
			break;
		}
		else if(vv[i]=='/' || vv[i]=='\\' || vv[i]==':')
		{
			break;
		}
	}
}


template <class assocType>
YSRESULT YsQuickSortString(int n,YsString const *keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortString (YsString *) %d\n",n);
	}

	int i,dividePoint,dif;
	const YsString *divider,*nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];  // Can use random number.
	nonDivider=NULL;

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		dif=strcmp(*keyValue[i],*divider);
		if(dif<0)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <const YsString *> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
		if(dif!=0)
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}
	}

	if(allEqual==YSTRUE)
	{
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		if(watch==YSTRUE)
		{
			YsPrintf("YsQuickSortString: Trying again %08x %08x\n",divider,nonDivider);
		}
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(assocValue!=NULL)
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,assocValue);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint);
	}
	else
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,NULL);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,NULL);
	}

	return YSOK;
}


template <class assocType>
YSRESULT YsQuickSortString(int n,char const *keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	int i,dividePoint,dif;
	const char *divider,*nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];  // Can use random number.
	nonDivider=NULL;

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortString (const char *) %d\n",n);
	}

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		dif=strcmp(keyValue[i],divider);
		if(dif<0)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <const char *> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
		if(dif!=0)
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}
	}

	if(allEqual==YSTRUE)
	{
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		if(watch==YSTRUE)
		{
			YsPrintf("YsQuickSortString: Trying again %08x %08x\n",divider,nonDivider);
		}
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(assocValue!=NULL)
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,assocValue,watch);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint,watch);
	}
	else
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,NULL,watch);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,NULL,watch);
	}

	return YSOK;
}



class YsKeyWord
{
public:
	int id;
	class YsString str;
};

class YsKeyWordList
{
protected:
	YsListAllocator <YsKeyWord> keyWordAlloc;
	YsListContainer <YsKeyWord> keyWordList;
	YsArray <YsKeyWord *> keyWordDict;
public:
	YsKeyWordList();
	void Initialize(void);

	int GetN(void) const;
	YSRESULT MakeList(const char * const str[]);
	YSRESULT AddKeyWord(int nStr,const char * const str[]);
	YSRESULT AddKeyWord(int nStr,const YsString str[]);
	int GetId(const char str[]) const;
	const char *GetString(int id) const;
};


/* } */
#endif
