#ifndef YSBASE_IS_INCLUDED
#define YSBASE_IS_INCLUDED
/* { */

#include "ysclassmgr.h"

// Declaration /////////////////////////////////////////////
class YsBaseClass
{
public:
	static YsClassHierarchy *ClassInfo;
	static char *ClassName;
	virtual char *WhatItIs(void){return YsBaseClass::ClassName;}
	virtual YsClassHierarchy *Hierarchy(void){return YsBaseClass::ClassInfo;}

	int IsKindOf(char *classname){return Hierarchy()->IsKindOf(classname);}
	static unsigned GetLeak(void){return ClassInfo->GetLeak();}

	YsBaseClass();
	YsBaseClass(const YsBaseClass &from);
	virtual ~YsBaseClass();
};

/* } */
#endif
