#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "ysclass.h"

YsClassHierarchy::YsClassHierarchy()
{
	parent=NULL;
	brother=NULL;
	children=NULL;
	classname=NULL;

	alloc=0;
	release=0;
	accum=0;
	maxAccum=0;
}

void YsClassHierarchy::SetIsChildOf(YsClassHierarchy *parentClass)
{
	if(parentClass==NULL)
	{
		printf("Ys Class Manager :\n");
		printf("    NULL Parent Class Is Detected\n");
		printf("    In intializing class [%s]\n",classname);
	}

	parent=parentClass;

	if(parentClass->children==NULL)
	{
		parentClass->children=this;
	}
	else
	{
		YsClassHierarchy *seeker;
		for(seeker=parentClass->children; seeker!=NULL; seeker=seeker->brother)
		{
			if(seeker->brother==NULL)
			{
				seeker->brother=this;
				break;
			}
		}
	}
}

void YsClassHierarchy::Allocate(void)
{
	alloc++;
	accum++;
	if(accum>maxAccum)
	{
		maxAccum=accum;
	}
}

void YsClassHierarchy::Release(void)
{
	release++;
	accum--;
}

void YsClassHierarchy::ShowHierarchy(int indent)
{
	if(this!=NULL)
	{
		int l;

		if(indent==0)
		{
			printf("Top Down Hierarchy\n");
			printf("[CLASSNAME]                     "
			       "(   Created   Released     Remain        Max)\n");
			indent+=2;
		}

		for(int i=0; i<indent; i++)
		{
			printf(" ");
		}
		l=indent+printf("[%s]",classname);
		while(l<32)
		{
			printf(" ");
			l++;
		}
		printf("(%10d %10d %10d %10d)\n",alloc,release,accum,maxAccum);

		if(children!=NULL)
		{
			children->ShowHierarchy(indent+2);
		}
		if(brother!=NULL)
		{
			brother->ShowHierarchy(indent);
		}
	}
}

void YsClassHierarchy::ShowBackTrack(int indent)
{
	if(indent==0)
	{
		printf("BackTrack[%s]\n",classname);
		indent+=2;
	}

	for(int i=0; i<indent; i++)
	{
		printf(" ");
	}
	printf("%s\n",classname);

	if(parent!=NULL)
	{
		parent->ShowBackTrack(indent+2);
	}
}

int YsClassHierarchy::IsKindOf(char *kind)
{
	int level;
	YsClassHierarchy *seeker;
	level=1;
	for(seeker=this; seeker!=NULL; seeker=seeker->parent)
	{
		if(strcmp(kind,seeker->classname)==0)
		{
			return level;
		}
		level++;
	}
	return 0;
}
