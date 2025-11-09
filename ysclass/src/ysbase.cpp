#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include "ysbase.h"

// Implementation //////////////////////////////////////////
YsClassHierarchy *YsBaseClass::ClassInfo=NULL;
char *YsBaseClass::ClassName="YsBaseClass";

YsBaseClass::YsBaseClass()
{
	if(ClassInfo==NULL)
	{
		ClassInfo=new YsClassHierarchy;
		ClassInfo->classname=WhatItIs();
	}
	ClassInfo->Allocate();
}

YsBaseClass::YsBaseClass(const YsBaseClass &from)
{
	if(ClassInfo==NULL)
	{
		ClassInfo=new YsClassHierarchy;
		ClassInfo->classname=WhatItIs();
	}
	ClassInfo->Allocate();
	*this=from;
}

YsBaseClass::~YsBaseClass()
{
	ClassInfo->Release();
}


