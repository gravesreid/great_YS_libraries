#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "ysdef.h"
#include "ysargs.h"

/***********************************************************************/
static char *YsStrtok(char **src);

YSRESULT YsArguments(int *ac,char *av[],int mxac,char *src)
{
	char *arg;
	int len;

	while(*src==' ' || *src=='\t')
	{
		src++;
	}

	len=strlen(src)-1;
	while(len>=0 && isprint(src[len])==0)
	{
		src[len]=0;
		len--;
	}

	*ac=0;
	while((arg=YsStrtok(&src))!=NULL && *ac<mxac)
	{
		av[*ac]=arg;
		(*ac)++;
	}

	if(*ac<mxac)  // 2007/06/22 Added this check.
	{
		av[*ac]=NULL;
	}
	return YSOK;
}

static char *YsStrtok(char **src)
{
	char *r;
	switch(**src)
	{
	case 0:
		return NULL;
	case '\"':
		(*src)++;
		r=(*src);
		while((**src)!='\"' && (**src)!=0)
		{
			(*src)++;
		}
		break;
	default:
		r=(*src);
		while((**src)!=' ' && (**src)!='\t' && (**src)!=0)
		{
			(*src)++;
		}
		break;
	}
	if((**src)!=0)
	{
		(**src)=0;
		(*src)++;
	}
	while((**src)!=0 && ((**src)==' ' || (**src)=='\t'))
	{
		(*src)++;
	}
	return r;
}


/***********************************************************************/
YSRESULT YsCommandNumber(int *cmd,const char in[],char *const lst[])
{
	int i,l;
	for(i=0; lst[i]!=NULL; i++)
	{
		l=strlen(lst[i]);
		if(strncmp(in,lst[i],l)==0)
		{
			*cmd=i;
			return YSOK;
		}
	}
	*cmd=0;
	return YSERR;
}


