#include <stdio.h>
#include <string.h>

#include "ysdef.h"
#include "ysreldir.h"

/***********************************************************************/
static void YsAppendSlash(char *dir);
static void YsAnalyze(char *dst,char *str,char *dir);
static void YsRemoveSamePart(char **str,char **dir);
static void YsMoveUpward(char *dst,char *dir);
static void YsAntiMacintosh(char rel[]);

#define YsIsSprtr(a) ((a)=='/' || (a)=='\\' || (a)==':')

YSRESULT YsRelativePath(char *dst,char *src,char *org)
{
	char str[256],dir[256];

	strcpy(str,src);
	strcpy(dir,org);
	YsAppendSlash(dir);
	YsAnalyze(dst,str,dir);
	YsAntiMacintosh(dst);
	return YSOK;
}

static void YsAntiMacintosh(char rel[])
{
	int i;
	/* Change ':' into '/' because Macintosh doesn't know what the relative path is. */
	for(i=0; rel[i]!=0; i++)
	{
		if(rel[i]==':')
		{
			rel[i]='/';
		}
	}
}

static void YsAppendSlash(char *dir)
{
	int ln;
	ln=strlen(dir);
	if(!YsIsSprtr(dir[ln-1]))
	{
		dir[ln  ]='/';
		dir[ln+1]=0;
	}
}

static void YsAnalyze(char *dst,char *str,char *dir)
{
	char relPath[256];
	YsRemoveSamePart(&str,&dir);
	YsMoveUpward(relPath,dir);
	sprintf(dst,"%s%s",relPath,str);
}

static void YsRemoveSamePart(char **str,char **dir)
{
	int n;
	char *a,*b;

	a=*str;
	b=*dir;
	while(*a!=0 && *b!=0)
	{
		while(YsIsSprtr(*a) && YsIsSprtr(*b))
		{
			a++;
			b++;
		}

		n=0;
		while(a[n]!=0 && b[n]!=0 && !YsIsSprtr(a[n]) && !YsIsSprtr(b[n]))
		{
			n++;
		}
		if(n==0 || strncmp(a,b,n)!=0)
		{
			break;
		}
		a+=n;
		b+=n;
	}
	*str=a;
	*dir=b;
}

static void YsMoveUpward(char *dst,char *dir)
{
	dst[0]=0;
	while(*dir!=0)
	{
		if(YsIsSprtr(*dir))
		{
			strcpy(dst,"../");
			dst+=3;
		}
		dir++;
	}
}
