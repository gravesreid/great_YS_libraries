#ifndef YSBINARYTREE_IS_INCLUDED
#define YSBINARYTREE_IS_INCLUDED
/* { */

template <class T,class KEYTYPE>
class YsBinaryTreeNode
{
public:
	T dat;
	KEYTYPE key;

	YsBinaryTreeNode()
	{
		left=NULL;
		right=NULL;
		parent=NULL;
	}

	inline YsBinaryTreeNode <T,KEYTYPE> *GetParent(void) const
	{
		return parent;
	}
	inline YsBinaryTreeNode <T,KEYTYPE> *GetLeft(void) const
	{
		return left;
	}
	inline YsBinaryTreeNode <T,KEYTYPE> *GetRight(void) const
	{
		return right;
	}

	inline YSRESULT InsertSort(YsBinaryTreeNode <T,KEYTYPE> *newNode)
	{
		YsBinaryTreeNode <T,KEYTYPE> *track;
		track=this;
		while(1)
		{
			if(newNode->key<=track->key)
			{
				if(track->left==NULL)
				{
					newNode->parent=track;
					track->left=newNode;
					return YSOK;
				}
				else
				{
					track=track->left;
				}
			}
			else
			{
				if(track->right==NULL)
				{
					newNode->parent=track;
					track->right=newNode;
					return YSOK;
				}
				else
				{
					track=track->right;
				}
			}
		}
		return YSERR;
	}

protected:
	YsBinaryTreeNode <T,KEYTYPE> *left,*right,*parent;
};

/* } */
#endif
