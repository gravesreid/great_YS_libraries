#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


#include "ysexception.h"
#include "ysdef.h"
#include "ysbase.h"
#include "ysmath.h"
#include "yslist.h"
#include "yslist2.h"
#include "ysarray.h"
#include "ysstring.h"


void YsStringHead(char **line)
{
	char *ptr=(*line);

	while(*ptr != 0)
	{
		if(isprint(*ptr) && *ptr!=' ')
		{
			break;
		}
		ptr++;
	}

	(*line)=ptr;
}


void YsStringTail(char *l)
{
	unsigned int chr;
	int i,cut,len;

	len=strlen(l);
	cut=0;
	for(i=0; i<len; i++)
	{
		chr=((unsigned char *)l)[i];

		if((0x81<=chr && chr<=0x9f) || 0xe0<=chr)
		{
			cut=i+2;
			i++;
			continue;
		}
		else if(isprint(chr) && chr!=' ' && chr!='\t')
		{
			cut=i+1;
			continue;
		}
	}

	for(i=cut; i<len; i++)
	{
		l[i]=0;
	}
}

void YsCapitalize(char l[])
{
	int i;
	for(i=0; l[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)l)[i] && ((unsigned char*)l)[i]<=0x9f) || 0xe0<=((unsigned char*)l)[i])
		{
			i++;
			if(l[i]==0)
			{
				break;
			}
		}
		else if('a'<=l[i] && l[i]<='z')
		{
			l[i]=l[i]+'A'-'a';
		}
	}
}

void YsUncapitalize(char l[])
{
	int i;
	for(i=0; l[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)l)[i] && ((unsigned char*)l)[i]<=0x9f) || 0xe0<=((unsigned char*)l)[i])
		{
			i++;
			if(l[i]==0)
			{
				break;
			}
		}
		else if('A'<=l[i] && l[i]<='Z')
		{
			l[i]=l[i]+'a'-'A';
		}
	}
}

const char *YsOptimizeDoubleString(char buf[],const double &value)
{
	int i,j;
	sprintf(buf,"%lf",value);

	for(i=0; buf[i]!=0; i++)
	{
		if(buf[i]=='.')
		{
			for(j=strlen(buf)-1; j>=i && (buf[j]=='0' || buf[j]=='.') ; j--)
			{
				buf[j]=0;
			}
			break;
		}
	}
	return buf;
}

void YsArguments2(YsArray <YsString,16> &args,const char str[],const char *blank,const char *comma)
{
	int l;
	l=strlen(str);
	YsArguments2(args,str,l,blank,comma);
}

static YSBOOL CharIsOneOf(char c,const char str[])
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		if(str[i]==c)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

void YsArguments2(YsArray <YsString,16> &args,const char vv[],int l,const char *blank,const char *comma)
{
	YsString empty;
	args.Set(0,NULL);

	int i;
	i=0;
	while(vv[i]==' ' || vv[i]=='\t')
	{
		i++;
	}

	int head;
	head=i;
	while(i<l)
	{
		if(vv[head]=='\"')
		{
			head++;
			i++;
			while(i<l && vv[i]!='\"')
			{
				if(vv[i]&0x80)  // 1st letter of Kanji character?
				{
					i++;
				}
				else if(isprint(vv[i])==0)
				{
					break;
				}
				i++;
			}

			args.Append(empty);
			args[args.GetN()-1].Set(i-head,vv+head);

			if(vv[i]=='\"')
			{
				i++;
			}
		}
		else
		{
			while(i<l && (CharIsOneOf(vv[i],blank)!=YSTRUE && CharIsOneOf(vv[i],comma)!=YSTRUE))
			{
				if(vv[i]&0x80)  // 1st letter of Kanji character?
				{
					i++;
				}
				else if(isprint(vv[i])==0)
				{
					break;
				}
				i++;
			}

			if(head<i)  // <- This condition is added on 2005/03/03
			{
				args.Append(empty);
				args[args.GetN()-1].Set(i-head,vv+head);
			}
		}

		while(i<l && isprint(vv[i])==0)
		{
			i++;
		}
		while(i<l && CharIsOneOf(vv[i],blank)==YSTRUE)  // Skip blank separator
		{
			i++;
		}
		if(CharIsOneOf(vv[i],comma)==YSTRUE) // Skip one comma separator
		{
			i++;

			while(i<l && CharIsOneOf(vv[i],blank)==YSTRUE)  // Skip blank separator after a comma separator
			{
				i++;
			}

			if(i==l)
			{
				args.Append(empty);
			}
		}
		head=i;
	}
}

/***********************************************************************/

void YsString::MakeFullPathName(const char path[],const char name[])
{
	Set(path);
	if(LastChar()=='/' || LastChar()=='\\')
	{
		Append(name);
	}
	else
	{
		Append("/");
		Append(name);
	}
}

/***********************************************************************/

int YsString::Printf(const char *fom,...)
{
	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fom);
	a=vsprintf(buf,fom,arg_ptr);
	va_end(arg_ptr);

	Set(buf);

	return Strlen();
}

/***********************************************************************/

YsKeyWordList::YsKeyWordList() : keyWordList(keyWordAlloc)
{
}

void YsKeyWordList::Initialize(void)
{
	keyWordList.CleanUp();
	keyWordDict.Set(0,NULL);
}

int YsKeyWordList::GetN(void) const
{
	return keyWordList.GetN();
}

YSRESULT YsKeyWordList::MakeList(const char * const str[])
{
	int i,n;
	YsListItem <YsKeyWord> *kw;
	YsArray <const char *> ptr;

	Initialize();

	for(i=0; str[i]!=NULL; i++)
	{
		kw=keyWordList.Create();
		kw->dat.str.Set(str[i]);
		kw->dat.id=i;

		ptr.Append(str[i]);
		keyWordDict.Append(&kw->dat);
	}
	n=i;

	YsQuickSortString <YsKeyWord *> (n,ptr,keyWordDict);

	return YSOK;
}

YSRESULT YsKeyWordList::AddKeyWord(int nStr,const char * const str[])
{
	int i,j,k,id;
	YsListItem <YsKeyWord> *kw;
	YsArray <YsKeyWord *> prvKwd,newKwd;
	YsArray <const char *> newKwdPtr;

	id=keyWordList.GetN();
	newKwd.Set(nStr,NULL);
	newKwdPtr.Set(nStr,NULL);
	for(i=0; i<nStr; i++)
	{
		kw=keyWordList.Create();
		kw->dat.str.Set(str[i]);
		kw->dat.id=id;

		newKwd[i]=&kw->dat;
		newKwdPtr[i]=kw->dat.str;

		id++;
	}

	YsQuickSortString <YsKeyWord *> (newKwdPtr.GetN(),newKwdPtr,newKwd);

	prvKwd=keyWordDict;

	keyWordDict.Set(prvKwd.GetN()+newKwd.GetN(),NULL);

	i=0;
	j=0;
	k=0;
	while(i<prvKwd.GetN() || j<newKwd.GetN())
	{
		if(newKwd.GetN()<=j)
		{
			keyWordDict[k++]=prvKwd[i++];
		}
		else if(prvKwd.GetN()<=i)
		{
			keyWordDict[k++]=newKwd[j++];
		}
		else if(strcmp(prvKwd[i]->str,newKwd[j]->str)<0)
		{
			keyWordDict[k++]=prvKwd[i++];
		}
		else
		{
			keyWordDict[k++]=newKwd[j++];
		}
	}

	return YSOK;
}

YSRESULT YsKeyWordList::AddKeyWord(int nStr,const YsString str[])
{
	int i;
	YsArray <const char *> ptr;
	ptr.Set(nStr,NULL);
	for(i=0; i<nStr; i++)
	{
		ptr[i]=str[i];
	}
	return AddKeyWord(nStr,ptr.GetArray());
}

int YsKeyWordList::GetId(const char str[]) const
{
	if(keyWordDict.GetN()==0)
	{
		return -1;
	}

	int idLow,idHigh;
	idLow=0;
	idHigh=keyWordDict.GetN()-1;

	while(idHigh-idLow>1)
	{
		int idMid,dif;
		idMid=idLow+idHigh;
		idMid>>=1;
		dif=strcmp(str,keyWordDict[idMid]->str);
		if(dif==0)
		{
			return keyWordDict[idMid]->id;
		}
		else if(dif<0)
		{
			idHigh=idMid;
		}
		else
		{
			idLow=idMid;
		}
	}

	if(strcmp(str,keyWordDict[idLow]->str)==0)
	{
		return keyWordDict[idLow]->id;
	}
	else if(strcmp(str,keyWordDict[idHigh]->str)==0)
	{
		return keyWordDict[idHigh]->id;
	}

	return -1;
}

const char *YsKeyWordList::GetString(int id) const
{
	if(keyWordDict.IsInRange(id)==YSTRUE)
	{
		return keyWordDict[id]->str;
	}
	return NULL;
}


