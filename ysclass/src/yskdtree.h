#ifndef YSKDTREE_IS_INCLUDED
#define YSKDTREE_IS_INCLUDED
/* { */


template <class T>
class Ys2dTree;

template <class T>
class Ys2dTreeNode
{
friend class Ys2dTree <T>;

public:
	Ys2dTreeNode();
	Ys2dTreeNode(const YsVec2 &p,const T &a);
	~Ys2dTreeNode();

	YSRESULT SetAttribute(const T &x);
	const T &GetAttribute(void) const;

	YSRESULT SetPosition(const YsVec2 &x);
	const YsVec2 &GetPosition(void) const;

	// YSRESULT AddPoint(int depth,Ys2dTreeNode <T> *newNode);

	const Ys2dTreeNode <T> *GetSmallerNode(void) const;
	const Ys2dTreeNode <T> *GetGreaterNode(void) const;

	// YSRESULT Traverse(const Ys2dTree <T> &callback,const YsVec2 &bbxMin,const YsVec2 &bbxMax,int depth) const;

	YSBOOL preAllocated;
protected:
	Ys2dTreeNode <T> *smaller,*greater;
	T attrib;
	YsVec2 pos;
};

template <class T>
Ys2dTreeNode <T>::Ys2dTreeNode()
{
	smaller=NULL;
	greater=NULL;
	preAllocated=YSTRUE;

}

template <class T>
Ys2dTreeNode <T>::Ys2dTreeNode(const YsVec2 &p,const T &a) : pos(p),attrib(a)
{
	smaller=NULL;
	greater=NULL;
	preAllocated=YSFALSE;
}

template <class T>
Ys2dTreeNode <T>::~Ys2dTreeNode()
{
}

template <class T>
YSRESULT Ys2dTreeNode <T>::SetAttribute(const T &x)
{
	attrib=x;
	return YSOK;
}

template <class T>
const T &Ys2dTreeNode <T>::GetAttribute(void) const
{
	return attrib;
}


template <class T>
YSRESULT Ys2dTreeNode <T>::SetPosition(const YsVec2 &x)
{
	pos=x;
	return YSOK;
}

template <class T>
const YsVec2 &Ys2dTreeNode <T>::GetPosition(void) const
{
	return pos;
}


/* template <class T>
YSRESULT Ys2dTreeNode <T>::AddPoint(int depth,Ys2dTreeNode <T> *newNode)
{
	// 2001/04/23  owner is responsible to set position and attribute of the new node.
	const YsVec2 &newPos=newNode->GetPosition();
	double newValue,currentValue;

	newValue=newPos.GetValue()[depth&1];
	currentValue=pos.GetValue()[depth&1];

	if(newValue<currentValue)
	{
		if(smaller==NULL)
		{
			// 2001/04/23  Now new node must be allocated by the owner
			smaller=newNode;  // new Ys2dTreeNode <T> (newPos,newAttrib);
			return YSOK;
		}
		else
		{
			return smaller->AddPoint(depth+1,newNode);
		}
	}
	else
	{
		if(greater==NULL)
		{
			// 2001/04/23  Now new node must be allocated by the owner
			greater=newNode;  // new Ys2dTreeNode <T> (newPos,newAttrib);
			return YSOK;
		}
		else
		{
			return greater->AddPoint(depth+1,newNode);
		}
	}
} */

template <class T>
const Ys2dTreeNode <T> *Ys2dTreeNode <T>::GetSmallerNode(void) const
{
	return smaller;
}

template <class T>
const Ys2dTreeNode <T> *Ys2dTreeNode <T>::GetGreaterNode(void) const
{
	return greater;
}

////////////////////////////////////////////////////////////


template <class T>
class Ys2dTree
{
friend class Ys2dTreeNode <T>;

public:
	Ys2dTree();
	~Ys2dTree();
	void CleanUp(void);
	YSBOOL watch;

protected:
	void DeleteKdTreeNode(Ys2dTreeNode <T> *root);

public:
	YSRESULT AddPoint(const YsVec2 &pos,const T &attrib);
	const Ys2dTreeNode <T> *GetRootNode(void) const;

	YSRESULT Traverse(const YsVec2 &corner1,const YsVec2 &corner2) const;
	YSRESULT MakePointList(YsArray <YsVec2> &lst,const YsVec2 &corner1,const YsVec2 &corner2) const;
	YSRESULT MakeItemList(YsArray <T> &lst,const YsVec2 &corner1,const YsVec2 &corner2) const;
	YSRESULT MakePointAndItemList
	    (YsArray <YsVec2> &pntList,YsArray <T> &itmList,const YsVec2 &corner1,const YsVec2 &corner2) const;
	YSBOOL PointInRange(const YsVec2 &pos,const double &range) const;

	YSRESULT PreAllocateNode(int numPreAllocation);
protected:
	YSRESULT NodeTraverse(const YsVec2 &bbxMin,const YsVec2 &bbxMax) const;

	Ys2dTreeNode <T> *CreateNode(const YsVec2 &pos,const T &attrib);
	virtual YSRESULT Enumerate(const YsVec2 &pos,const T &attrib) const;
	Ys2dTreeNode <T> *root;

	int nPreAllocatedNode,nPreAllocatedNodeUsed;
	Ys2dTreeNode <T> *preAllocatedNode;

	mutable YsArray <YsVec2> *pointList;
	mutable YsArray <T> *itemList;
};


template <class T>
Ys2dTree <T>::Ys2dTree()
{
	root=NULL;
	nPreAllocatedNode=0;
	nPreAllocatedNodeUsed=0;
	preAllocatedNode=NULL;

	pointList=NULL;
	itemList=NULL;

	watch=YSFALSE;
}

template <class T>
Ys2dTree <T>::~Ys2dTree()
{
	CleanUp();
}

template <class T>
void Ys2dTree <T>::CleanUp(void)
{
	DeleteKdTreeNode(root);
	if(preAllocatedNode!=NULL)
	{
		delete [] preAllocatedNode;
		preAllocatedNode=NULL;
	}

	root=NULL;
	nPreAllocatedNode=0;
	nPreAllocatedNodeUsed=0;
	preAllocatedNode=NULL;

	pointList=NULL;
	itemList=NULL;
}

template <class T>
void Ys2dTree <T>::DeleteKdTreeNode(Ys2dTreeNode <T> *node)
{
	if(node!=NULL)
	{
		Ys2dTree <T>::DeleteKdTreeNode(node->smaller);
		Ys2dTree <T>::DeleteKdTreeNode(node->greater);
		if(node->preAllocated!=YSTRUE)
		{
			delete node;
		}
	}
}

template <class T>
YSRESULT Ys2dTree <T>::AddPoint(const YsVec2 &newPos,const T &attrib)
{
	Ys2dTreeNode <T> *newNode;
	newNode=CreateNode(newPos,attrib);
	if(newNode!=NULL)
	{
		if(root!=NULL)
		{
			int depth;
			Ys2dTreeNode <T> *node;
			node=root;
			depth=0;
			while(1)
			{
				// 2001/04/23  owner is responsible to set position and attribute of the new node.
				double newValue,currentValue;

				newValue=newPos[depth&1];
				currentValue=node->pos[depth&1];

				if(newValue<currentValue)
				{
					if(node->smaller==NULL)
					{
						node->smaller=newNode;
						return YSOK;
					}
					else
					{
						node=node->smaller;
					}
				}
				else
				{
					if(node->greater==NULL)
					{
						node->greater=newNode;
						return YSOK;
					}
					else
					{
						node=node->greater;
					}
				}
				depth++;
			}
		}
		else
		{
			root=newNode;
			return YSOK;
		}
	}
	return YSERR;
}

template <class T>
const Ys2dTreeNode <T> *Ys2dTree <T>::GetRootNode(void) const
{
	return root;
}

template <class T>
YSRESULT Ys2dTree <T>::Traverse(const YsVec2 &corner1,const YsVec2 &corner2) const
{
	YsBoundingBoxMaker2 bbx;
	YsVec2 min,max;

	pointList=NULL;
	itemList=NULL;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	return NodeTraverse(min,max); // root->Traverse(*this,min,max,0);
}

template <class T>
YSRESULT Ys2dTree <T>::MakePointList(YsArray <YsVec2> &lst,const YsVec2 &corner1,const YsVec2 &corner2) const
{
	YsBoundingBoxMaker2 bbx;
	YsVec2 min,max;

	pointList=&lst;
	itemList=NULL;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	lst.Set(0,NULL);
	return NodeTraverse(min,max); // root->Traverse(*this,min,max,0);
}

template <class T>
YSRESULT Ys2dTree <T>::MakeItemList(YsArray <T> &lst,const YsVec2 &corner1,const YsVec2 &corner2) const
{
	YsBoundingBoxMaker2 bbx;
	YsVec2 min,max;

	pointList=NULL;
	itemList=&lst;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	lst.Set(0,NULL);

	if(watch==YSTRUE)
	{
		printf("XX4 %08x\n",root);
	}

	YSRESULT res;
	res=NodeTraverse(min,max); // root->Traverse(*this,min,max,0);

	if(watch==YSTRUE)
	{
		printf("XX5 %d\n",res);
	}
	return res;
}

template <class T>
YSRESULT Ys2dTree <T>::MakePointAndItemList
    (YsArray <YsVec2> &pntList,YsArray <T> &itmList,const YsVec2 &corner1,const YsVec2 &corner2) const
{
	YsBoundingBoxMaker2 bbx;
	YsVec2 min,max;

	pointList=&pntList;
	itemList=&itmList;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	pntList.Set(0,NULL);
	itmList.Set(0,NULL);
	return NodeTraverse(min,max); // root->Traverse(*this,min,max,0);
}

template <class T>
YSBOOL Ys2dTree <T>::PointInRange(const YsVec2 &pos,const double &range) const
{
	YsVec2 bbx[2];
	YsArray <YsVec2> pntList;
	int i;
	double rangeSq;
	extern const YsVec2 &YsXYZ(void);

	bbx[0]=pos-YsXYZ()*range;
	bbx[1]=pos+YsXYZ()*range;

	MakePointList(pntList,bbx[0],bbx[1]);
	rangeSq=range*range;

	forYsArray(i,pntList)
	{
		if((pntList[i]-pos).GetSquareLength()<=rangeSq)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

template <class T>
YSRESULT Ys2dTree <T>::Enumerate(const YsVec2 &pos,const T &attrib) const
{
	YsErrOut("Ys2dTree <T>::Enumerate()\n");
	YsErrOut("  This function does nothing.\n");
	return YSOK;
}

template <class T>
YSRESULT Ys2dTree <T>::PreAllocateNode(int numPreAllocation)
{
	preAllocatedNode=new Ys2dTreeNode <T> [numPreAllocation];
	if(preAllocatedNode!=NULL)
	{
		int i;
		nPreAllocatedNode=numPreAllocation;
		nPreAllocatedNodeUsed=0;

		for(i=0; i<numPreAllocation; i++)
		{
			preAllocatedNode[i].preAllocated=YSTRUE;
		}

		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT Ys2dTree <T>::NodeTraverse(const YsVec2 &bbxMin,const YsVec2 &bbxMax) const
{
	YsArray <Ys2dTreeNode <T> *> nodeStack;
	YsArray <int> depthStack;

	nodeStack.Append(root);
	depthStack.Append(0);

	if(watch==YSTRUE)
	{
	}

	while(nodeStack.GetN()>0)
	{
		int depth;
		Ys2dTreeNode <T> *node;

		node=nodeStack.GetEndItem();
		depth=depthStack.GetEndItem();

		nodeStack.Delete(nodeStack.GetN()-1);
		depthStack.Delete(depthStack.GetN()-1);

		if(node!=NULL)
		{
			double ref,refMin,refMax;

			if(bbxMin.x()<=node->pos.x() && node->pos.x()<bbxMax.x() &&
			   bbxMin.y()<=node->pos.y() && node->pos.y()<bbxMax.y())
			{
				if(itemList==NULL && pointList==NULL)
				{
					Enumerate(node->pos,node->attrib);
				}
				else
				{
					if(itemList!=NULL)
					{
						itemList->Append(node->attrib);
					}
					if(pointList!=NULL)
					{
						pointList->Append(node->pos);
					}
				}
			}

			ref=node->pos.GetValue()[depth%3];
			refMin=bbxMin.GetValue()[depth%3];
			refMax=bbxMax.GetValue()[depth%3];

			if(watch==YSTRUE)
			{
				printf("E %d %08x %08x\n",depth,this,node->smaller);
			}

			if(refMin<=ref && node->smaller!=NULL)
			{
				nodeStack.Append(node->smaller);
				depthStack.Append(depth+1);
			}

			if(watch==YSTRUE)
			{
				printf("F\n");
			}

			if(ref<=refMax && node->greater!=NULL)
			{
				nodeStack.Append(node->greater);
				depthStack.Append(depth+1);
			}
		}
	}

	if(watch==YSTRUE)
	{
		printf("D\n");
	}

	return YSOK;
}

template <class T>
Ys2dTreeNode <T> *Ys2dTree <T>::CreateNode(const YsVec2 &pos,const T &attrib)
{
	Ys2dTreeNode <T> *newNode;
	if(nPreAllocatedNodeUsed<nPreAllocatedNode)
	{
		newNode=&preAllocatedNode[nPreAllocatedNodeUsed];
		newNode->SetPosition(pos);
		newNode->SetAttribute(attrib);
		nPreAllocatedNodeUsed++;
	}
	else
	{
		// if(nPreAllocatedNode>0)
		// {
		// 	YsErrOut("Ys2dTree::CreateNode()\n");
		// 	YsErrOut("  Exceeded preallocation!!\n");
		// }
		newNode=new Ys2dTreeNode <T> (pos,attrib);
		newNode->preAllocated=YSFALSE;
	}
	return newNode;
}


////////////////////////////////////////////////////////////


extern int Ys3dTreeNumTraverse,Ys3dTreeNumEnumerate /* ,Ys3dTreeNodeMemLeakTrack */;  
// 2009/07/17 Stop memory-leak track.  This doesn't work in multi-thread.

template <class T>
class Ys3dTree;

template <class T>
class Ys3dTreeNode
{
friend class Ys3dTree <T>;

public:
	Ys3dTreeNode();
	Ys3dTreeNode(const YsVec3 &p,const T &a);
	~Ys3dTreeNode();

	YSRESULT SetAttribute(const T &x);
	const T &GetAttribute(void) const;

	YSRESULT SetPosition(const YsVec3 &x);
	const YsVec3 &GetPosition(void) const;

	// YSRESULT AddPoint(int depth,Ys3dTreeNode <T> *newNode);

	const Ys3dTreeNode <T> *GetSmallerNode(void) const;
	const Ys3dTreeNode <T> *GetGreaterNode(void) const;

	// YSRESULT Traverse(const Ys3dTree <T> &callback,const YsVec3 &bbxMin,const YsVec3 &bbxMax,int depth) const;

	YSBOOL preAllocated;
protected:
	Ys3dTreeNode <T> *smaller,*greater;
	T attrib;
	YsVec3 pos;
};

template <class T>
Ys3dTreeNode <T>::Ys3dTreeNode()
{
	smaller=NULL;
	greater=NULL;
	preAllocated=YSTRUE;
	/* Ys3dTreeNodeMemLeakTrack++; */

}

template <class T>
Ys3dTreeNode <T>::Ys3dTreeNode(const YsVec3 &p,const T &a) : pos(p),attrib(a)
{
	smaller=NULL;
	greater=NULL;
	preAllocated=YSFALSE;
	/* Ys3dTreeNodeMemLeakTrack++; */
}

template <class T>
Ys3dTreeNode <T>::~Ys3dTreeNode()
{
	/* Ys3dTreeNodeMemLeakTrack--;

	if(Ys3dTreeNodeMemLeakTrack<0)
	{
		YsErrOut("Ys3dTreeNode\n");
		YsErrOut("  Excessive killing of nodes!! %d\n",Ys3dTreeNodeMemoryLeak);
	} */
}

template <class T>
YSRESULT Ys3dTreeNode <T>::SetAttribute(const T &x)
{
	attrib=x;
	return YSOK;
}

template <class T>
const T &Ys3dTreeNode <T>::GetAttribute(void) const
{
	return attrib;
}


template <class T>
YSRESULT Ys3dTreeNode <T>::SetPosition(const YsVec3 &x)
{
	pos=x;
	return YSOK;
}

template <class T>
const YsVec3 &Ys3dTreeNode <T>::GetPosition(void) const
{
	return pos;
}


/* template <class T>
YSRESULT Ys3dTreeNode <T>::AddPoint(int depth,Ys3dTreeNode <T> *newNode)
{
	// 2001/04/23  owner is responsible to set position and attribute of the new node.
	const YsVec3 &newPos=newNode->GetPosition();
	double newValue,currentValue;

	newValue=newPos.GetValue()[depth%3];
	currentValue=pos.GetValue()[depth%3];

	if(newValue<currentValue)
	{
		if(smaller==NULL)
		{
			// 2001/04/23  Now new node must be allocated by the owner
			smaller=newNode;  // new Ys3dTreeNode <T> (newPos,newAttrib);
			return YSOK;
		}
		else
		{
			return smaller->AddPoint(depth+1,newNode);
		}
	}
	else
	{
		if(greater==NULL)
		{
			// 2001/04/23  Now new node must be allocated by the owner
			greater=newNode;  // new Ys3dTreeNode <T> (newPos,newAttrib);
			return YSOK;
		}
		else
		{
			return greater->AddPoint(depth+1,newNode);
		}
	}
} */

template <class T>
const Ys3dTreeNode <T> *Ys3dTreeNode <T>::GetSmallerNode(void) const
{
	return smaller;
}

template <class T>
const Ys3dTreeNode <T> *Ys3dTreeNode <T>::GetGreaterNode(void) const
{
	return greater;
}

////////////////////////////////////////////////////////////


template <class T>
class Ys3dTree
{
friend class Ys3dTreeNode <T>;

public:
	Ys3dTree();
	~Ys3dTree();
	void CleanUp(void);
	YSBOOL watch;

protected:
	void DeleteKdTreeNode(Ys3dTreeNode <T> *root);

public:
	YSRESULT AddPoint(const YsVec3 &pos,const T &attrib);
	const Ys3dTreeNode <T> *GetRootNode(void) const;

	YSRESULT Traverse(const YsVec3 &corner1,const YsVec3 &corner2) const;
	YSRESULT MakePointList(YsArray <YsVec3> &lst,const YsVec3 &corner1,const YsVec3 &corner2) const;
	YSRESULT MakeItemList(YsArray <T> &lst,const YsVec3 &corner1,const YsVec3 &corner2) const;
	YSRESULT MakePointAndItemList
	    (YsArray <YsVec3> &pntList,YsArray <T> &itmList,const YsVec3 &corner1,const YsVec3 &corner2) const;
	YSBOOL PointInRange(const YsVec3 &pos,const double &range) const;

	YSRESULT PreAllocateNode(int numPreAllocation);
protected:
	YSRESULT NodeTraverse(const YsVec3 &bbxMin,const YsVec3 &bbxMax) const;

	Ys3dTreeNode <T> *CreateNode(const YsVec3 &pos,const T &attrib);
	virtual YSRESULT Enumerate(const YsVec3 &pos,const T &attrib) const;
	Ys3dTreeNode <T> *root;

	int nPreAllocatedNode,nPreAllocatedNodeUsed;
	Ys3dTreeNode <T> *preAllocatedNode;

	mutable YsArray <YsVec3> *pointList;
	mutable YsArray <T> *itemList;
};


template <class T>
Ys3dTree <T>::Ys3dTree()
{
	root=NULL;
	nPreAllocatedNode=0;
	nPreAllocatedNodeUsed=0;
	preAllocatedNode=NULL;

	pointList=NULL;
	itemList=NULL;

	watch=YSFALSE;
}

template <class T>
Ys3dTree <T>::~Ys3dTree()
{
	CleanUp();
}

template <class T>
void Ys3dTree <T>::CleanUp(void)
{
	DeleteKdTreeNode(root);
	if(preAllocatedNode!=NULL)
	{
		delete [] preAllocatedNode;
		preAllocatedNode=NULL;
	}

	root=NULL;
	nPreAllocatedNode=0;
	nPreAllocatedNodeUsed=0;
	preAllocatedNode=NULL;

	pointList=NULL;
	itemList=NULL;
}

template <class T>
void Ys3dTree <T>::DeleteKdTreeNode(Ys3dTreeNode <T> *node)
{
	if(node!=NULL)
	{
		Ys3dTree <T>::DeleteKdTreeNode(node->smaller);
		Ys3dTree <T>::DeleteKdTreeNode(node->greater);
		if(node->preAllocated!=YSTRUE)
		{
			delete node;
		}
	}
}

template <class T>
YSRESULT Ys3dTree <T>::AddPoint(const YsVec3 &newPos,const T &attrib)
{
	Ys3dTreeNode <T> *newNode;
	newNode=CreateNode(newPos,attrib);
	if(newNode!=NULL)
	{
		if(root!=NULL)
		{
			int depth;
			Ys3dTreeNode <T> *node;
			node=root;
			depth=0;
			while(1)
			{
				// 2001/04/23  owner is responsible to set position and attribute of the new node.
				double newValue,currentValue;

				newValue=newPos[depth%3];
				currentValue=node->pos[depth%3];

				if(newValue<currentValue)
				{
					if(node->smaller==NULL)
					{
						node->smaller=newNode;
						return YSOK;
					}
					else
					{
						node=node->smaller;
					}
				}
				else
				{
					if(node->greater==NULL)
					{
						node->greater=newNode;
						return YSOK;
					}
					else
					{
						node=node->greater;
					}
				}
				depth++;
			}
		}
		else
		{
			root=newNode;
			return YSOK;
		}
	}
	return YSERR;
}

template <class T>
const Ys3dTreeNode <T> *Ys3dTree <T>::GetRootNode(void) const
{
	return root;
}

template <class T>
YSRESULT Ys3dTree <T>::Traverse(const YsVec3 &corner1,const YsVec3 &corner2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 min,max;

	pointList=NULL;
	itemList=NULL;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	return NodeTraverse(min,max); // root->Traverse(*this,min,max,0);
}

template <class T>
YSRESULT Ys3dTree <T>::MakePointList(YsArray <YsVec3> &lst,const YsVec3 &corner1,const YsVec3 &corner2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 min,max;

	pointList=&lst;
	itemList=NULL;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	lst.Set(0,NULL);
	return NodeTraverse(min,max); // root->Traverse(*this,min,max,0);
}

template <class T>
YSRESULT Ys3dTree <T>::MakeItemList(YsArray <T> &lst,const YsVec3 &corner1,const YsVec3 &corner2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 min,max;

	pointList=NULL;
	itemList=&lst;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	lst.Set(0,NULL);

	if(watch==YSTRUE)
	{
		printf("XX4 %08x\n",root);
	}

	YSRESULT res;
	res=NodeTraverse(min,max); // root->Traverse(*this,min,max,0);

	if(watch==YSTRUE)
	{
		printf("XX5 %d\n",res);
	}
	return res;
}

template <class T>
YSRESULT Ys3dTree <T>::MakePointAndItemList
    (YsArray <YsVec3> &pntList,YsArray <T> &itmList,const YsVec3 &corner1,const YsVec3 &corner2) const
{
	YsBoundingBoxMaker3 bbx;
	YsVec3 min,max;

	pointList=&pntList;
	itemList=&itmList;

	bbx.Begin(corner1);
	bbx.Add(corner2);
	bbx.Get(min,max);

	pntList.Set(0,NULL);
	itmList.Set(0,NULL);
	return NodeTraverse(min,max); // root->Traverse(*this,min,max,0);
}

template <class T>
YSBOOL Ys3dTree <T>::PointInRange(const YsVec3 &pos,const double &range) const
{
	YsVec3 bbx[2];
	YsArray <YsVec3> pntList;
	int i;
	double rangeSq;
	extern const YsVec3 &YsXYZ(void);

	bbx[0]=pos-YsXYZ()*range;
	bbx[1]=pos+YsXYZ()*range;

	MakePointList(pntList,bbx[0],bbx[1]);
	rangeSq=range*range;

	forYsArray(i,pntList)
	{
		if((pntList[i]-pos).GetSquareLength()<=rangeSq)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

template <class T>
YSRESULT Ys3dTree <T>::Enumerate(const YsVec3 &pos,const T &attrib) const
{
	YsErrOut("Ys3dTree <T>::Enumerate()\n");
	YsErrOut("  This function does nothing.\n");
	return YSOK;
}

template <class T>
YSRESULT Ys3dTree <T>::PreAllocateNode(int numPreAllocation)
{
	preAllocatedNode=new Ys3dTreeNode <T> [numPreAllocation];
	if(preAllocatedNode!=NULL)
	{
		int i;
		nPreAllocatedNode=numPreAllocation;
		nPreAllocatedNodeUsed=0;

		for(i=0; i<numPreAllocation; i++)
		{
			preAllocatedNode[i].preAllocated=YSTRUE;
		}

		return YSOK;
	}
	return YSERR;
}

template <class T>
YSRESULT Ys3dTree <T>::NodeTraverse(const YsVec3 &bbxMin,const YsVec3 &bbxMax) const
{
	YsArray <Ys3dTreeNode <T> *> nodeStack;
	YsArray <int> depthStack;

	nodeStack.Append(root);
	depthStack.Append(0);

	if(watch==YSTRUE)
	{
		printf("C %d\n",Ys3dTreeNumTraverse);
	}

	while(nodeStack.GetN()>0)
	{
		int depth;
		Ys3dTreeNode <T> *node;

		node=nodeStack.GetEndItem();
		depth=depthStack.GetEndItem();

		nodeStack.Delete(nodeStack.GetN()-1);
		depthStack.Delete(depthStack.GetN()-1);

		if(node!=NULL)
		{
			Ys3dTreeNumTraverse++;

			double ref,refMin,refMax;

			if(bbxMin.x()<=node->pos.x() && node->pos.x()<bbxMax.x() &&
			   bbxMin.y()<=node->pos.y() && node->pos.y()<bbxMax.y() &&
			   bbxMin.z()<=node->pos.z() && node->pos.z()<bbxMax.z())
			{
				Ys3dTreeNumEnumerate++;
				if(itemList==NULL && pointList==NULL)
				{
					Enumerate(node->pos,node->attrib);
				}
				else
				{
					if(itemList!=NULL)
					{
						itemList->Append(node->attrib);
					}
					if(pointList!=NULL)
					{
						pointList->Append(node->pos);
					}
				}
			}

			ref=node->pos.GetValue()[depth%3];
			refMin=bbxMin.GetValue()[depth%3];
			refMax=bbxMax.GetValue()[depth%3];

			if(watch==YSTRUE)
			{
				printf("E %d %08x %08x\n",depth,this,node->smaller);
			}

			if(refMin<=ref && node->smaller!=NULL)
			{
				nodeStack.Append(node->smaller);
				depthStack.Append(depth+1);
			}

			if(watch==YSTRUE)
			{
				printf("F\n");
			}

			if(ref<=refMax && node->greater!=NULL)
			{
				nodeStack.Append(node->greater);
				depthStack.Append(depth+1);
			}
		}
	}

	if(watch==YSTRUE)
	{
		printf("D\n");
	}

	return YSOK;
}

template <class T>
Ys3dTreeNode <T> *Ys3dTree <T>::CreateNode(const YsVec3 &pos,const T &attrib)
{
	Ys3dTreeNode <T> *newNode;
	if(nPreAllocatedNodeUsed<nPreAllocatedNode)
	{
		newNode=&preAllocatedNode[nPreAllocatedNodeUsed];
		newNode->SetPosition(pos);
		newNode->SetAttribute(attrib);
		nPreAllocatedNodeUsed++;
	}
	else
	{
		// if(nPreAllocatedNode>0)
		// {
		// 	YsErrOut("Ys3dTree::CreateNode()\n");
		// 	YsErrOut("  Exceeded preallocation!!\n");
		// }
		newNode=new Ys3dTreeNode <T> (pos,attrib);
		newNode->preAllocated=YSFALSE;
	}
	return newNode;
}

////////////////////////////////////////////////////////////

/* template <class T>
YSRESULT Ys3dTreeNode <T>::Traverse(const Ys3dTree <T> &callback,const YsVec3 &bbxMin,const YsVec3 &bbxMax,int depth) const
{
	if(callback.watch==YSTRUE)
	{
		printf("C %d %08x\n",Ys3dTreeNumTraverse);
	}

	Ys3dTreeNumTraverse++;
	if(this!=NULL)
	{
		double ref,refMin,refMax;

		if(bbxMin.x()<=pos.x() && pos.x()<bbxMax.x() &&
		   bbxMin.y()<=pos.y() && pos.y()<bbxMax.y() &&
		   bbxMin.z()<=pos.z() && pos.z()<bbxMax.z())
		{
			Ys3dTreeNumEnumerate++;
			if(callback.itemList==NULL && callback.pointList==NULL)
			{
				callback.Enumerate(pos,attrib);
			}
			else
			{
				if(callback.itemList!=NULL)
				{
					callback.itemList->Append(attrib);
				}
				if(callback.pointList!=NULL)
				{
					callback.pointList->Append(pos);
				}
			}
		}

		ref=pos.GetValue()[depth%3];
		refMin=bbxMin.GetValue()[depth%3];
		refMax=bbxMax.GetValue()[depth%3];

		if(callback.watch==YSTRUE)
		{
			printf("E %d %08x %08x\n",depth,this,smaller);
		}

		if(refMin<=ref && smaller!=NULL)
		{
			if(smaller->Traverse(callback,bbxMin,bbxMax,depth+1)!=YSOK)
			{
				return YSERR;
			}
		}

		if(callback.watch==YSTRUE)
		{
			printf("F\n");
		}

		if(ref<=refMax && greater!=NULL)
		{
			if(greater->Traverse(callback,bbxMin,bbxMax,depth+1)!=YSOK)
			{
				return YSERR;
			}
		}
	}

	if(callback.watch==YSTRUE)
	{
		printf("D\n");
	}

	return YSOK;
} */


////////////////////////////////////////////////////////////



/* } */
#endif
