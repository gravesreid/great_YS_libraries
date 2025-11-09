#ifndef YSCLASSMGR_IS_INCLUDED
#define YSCLASSMGR_IS_INCLUDED
/* { */

class YsClassHierarchy
{
public:
	YsClassHierarchy();
	YsClassHierarchy *parent,*brother,*children;
	char *classname;

	void SetIsChildOf(YsClassHierarchy *parentClass);

	void ShowHierarchy(int indent);
	void ShowBackTrack(int indent);
	int IsKindOf(char *classname);
	unsigned alloc,release,accum,maxAccum;

	void Allocate(void);
	void Release(void);

	inline unsigned GetLeak(void)
	{
		if(this!=NULL)
		{
			return accum;
		}
		else
		{
			return 0;
		}
	};
};

/* } */
#endif





/* Framework for Root Class

// Declaration /////////////////////////////////////////////
class <<ROOTCLASS>>
{
public:
	static YsClassHierarchy *ClassInfo;
	static char *ClassName;
	virtual char *WhatItIs(void){return <<ROOTCLASS>>::ClassName;}
	virtual YsClassHierarchy *Hierarchy(void){return <<ROOTCLASS>>::ClassInfo;}

	int IsKindOf(char *classname){return Hierarchy()->IsKindOf(classname);}
	static unsigned GetLeak(void){return ClassInfo->GetLeak();}

	<<ROOTCLASS>>();
	virtual ~<<ROOTCLASS>>();
};



// Implementation //////////////////////////////////////////
YsClassHierarchy *<<ROOTCLASS>>::ClassInfo=NULL;
char *<<ROOTCLASS>>::ClassName="<<ROOTCLASS>>";

<<ROOTCLASS>>::<<ROOTCLASS>>()
{
	if(ClassInfo==NULL)
	{
		ClassInfo=new YsClassHierarchy;
		ClassInfo->classname=WhatItIs();
	}
	ClassInfo->Allocate();
// any initialization
}

//<<ROOTCLASS>>::<<ROOTCLASS>>(any parameter)
//{
//	if(ClassInfo==NULL)
//	{
//		ClassInfo=new YsClassHierarchy;
//		ClassInfo->classname=WhatItIs();
//	}
//	ClassInfo->Allocate();
//  any initialization
//}

<<ROOTCLASS>>::~<<ROOTCLASS>>()
{
	ClassInfo->Release();
}

*/


/* Framework for Inherited Class

// Declaration /////////////////////////////////////////////
class <<INHERITED>> : public <<BASECLASS>>
{
public:
	static YsClassHierarchy *ClassInfo;
	static char *ClassName;
	virtual char *WhatItIs(void){return <<INHERITED>>::ClassName;}
	virtual YsClassHierarchy *Hierarchy(void){return <<INHERITED>>::ClassInfo;}
	static unsigned GetLeak(void){return ClassInfo->GetLeak();}
	<<INHERITED>>();
	virtual ~<<INHERITED>>();
};



// Implementation //////////////////////////////////////////
YsClassHierarchy *<<INHERITED>>::ClassInfo=NULL;
char *<<INHERITED>>::ClassName="<<INHERITED>>";

<<INHERITED>>::<<INHERITED>>()
{
	if(ClassInfo==NULL)
	{
		ClassInfo=new YsClassHierarchy;
		ClassInfo->classname=WhatItIs();
		ClassInfo->SetIsChildOf(<<BASECLASS>>::ClassInfo);
	}
	ClassInfo->Allocate();
// any initialization
}

//<<INHERITED>>::<<INHERITED>>(any parameter)
//{
//	if(ClassInfo==NULL)
//	{
//		ClassInfo=new YsClassHierarchy;
//		ClassInfo->classname=WhatItIs();
//		ClassInfo->SetIsChildOf(<<BASECLASS>>::ClassInfo);
//	}
//	ClassInfo->Allocate();
//  any initialization
//}

<<INHERITED>>::~<<INHERITED>>()
{
	ClassInfo->Release();
}



*/
