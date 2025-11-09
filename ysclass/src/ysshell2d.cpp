#include "ysclass.h"
#include "ysshell2d.h"


YsShell2dVertex::YsShell2dVertex()
{
	Initialize();
}

void YsShell2dVertex::Initialize(void)
{
	p=YsOrigin();
	projected.Set(0.0,0.0);
	refCount=0;
	att1=0;
	att2=0;
	att3=0;
	att4=0;
	vtHdMap=NULL;

	special=NULL;
}

YsShell2dEdge::YsShell2dEdge()
{
	Initialize();
}

void YsShell2dEdge::Initialize(void)
{
	special=NULL;
	att1=0;
	att2=0;
	att3=0;
	att4=0;
	attp=NULL;
	flags=0;
}

////////////////////////////////////////////////////////////

YsShell2dLattice::YsShell2dLattice()
{
	shl=NULL;
	numberOfUndeterminedCellCreatedByDeleteEdge=0;
}

YSRESULT YsShell2dLattice::SetDomain(const class YsShell2d &shl,int nCell)
{
	YsVec2 bbxMin,bbxMax;
	if(shl.GetBoundingBox(bbxMin,bbxMax)==YSOK)
	{
		return SetDomain(shl,nCell,bbxMin,bbxMax);
	}
	return YSERR;
	
}

YSRESULT YsShell2dLattice::SetDomain(const class YsShell2d &shl,int nCell,const YsVec2 &bbxMin,const YsVec2 &bbxMax)
{
	double area,l;
	int nx,ny;
	YsBoundingBoxMaker2 makeBbx;
	YsVec2 bbx[2],diagon;

	makeBbx.Add(bbxMin);
	makeBbx.Add(bbxMax);
	makeBbx.Get(bbx[0],bbx[1]);

	diagon=bbx[1]-bbx[0];
	area=(diagon.x()*diagon.y())/(double)nCell;
	l=sqrt(area);

	nx=1+(int)(diagon.x()/l);
	ny=1+(int)(diagon.y()/l);

	return SetDomain(shl,nx,ny,bbxMin,bbxMax);
}

YSRESULT YsShell2dLattice::SetDomain(const YsShell2d &s,int nx,int ny)
{
	YsVec2 bbxMin,bbxMax;

	if(s.GetBoundingBox(bbxMin,bbxMax)==YSOK)
	{
		return SetDomain(s,nx,ny,bbxMin,bbxMax);
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::SetDomain(const class YsShell2d &s,int nx,int ny,const YsVec2 &min,const YsVec2 &max)
{
	YsVec2 bbxMin,bbxMax,cen;

	cen=(min+max)/2.0;
	bbxMin=cen+(min-cen)*1.02;
	bbxMax=cen+(max-cen)*1.02;
	if(Create(nx,ny,bbxMin,bbxMax)==YSOK)
	{
		int x,y;
		shl=&s;
		for(y=0; y<ny; y++)
		{
			for(x=0; x<nx; x++)
			{
				GetEditableBlock(x,y)->side=YSUNKNOWNSIDE;
				GetEditableBlock(x,y)->vtxList.Set(0,NULL);
				GetEditableBlock(x,y)->edgList.Set(0,NULL);
			}
		}


		YsShell2dVertexHandle vtx;
		vtx=NULL;
		while((vtx=s.FindNextVertex(vtx))!=NULL)
		{
			AddVertex(vtx);
		}


		YsShell2dEdgeHandle edg;
		edg=NULL;
		while((edg=s.FindNextEdge(edg))!=NULL)
		{
			AddEdge(edg);
		}

		FillUndeterminedBlock();

		return YSOK;
	}
	return YSERR;
}


YSRESULT YsShell2dLattice::AddVertex(YsShell2dVertexHandle vtHd)
{
	if(shl!=NULL)
	{
		YsVec2 pnt;
		if(shl->GetProjectedVertexPosition(pnt,vtHd)==YSOK)
		{
			int bx,by;
			if(GetBlockIndex(bx,by,pnt)==YSOK)
			{
				YsShell2dLatticeElem *elm;
				elm=GetEditableBlock(bx,by);
				if(elm!=NULL)
				{
					elm->vtxList.AppendItem(vtHd);
					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::DeleteVertex(YsShell2dVertexHandle vtHd)
{
	if(shl!=NULL)
	{
		YsVec2 pnt;
		if(shl->GetProjectedVertexPosition(pnt,vtHd)==YSOK)
		{
			int i,bx,by;
			if(GetBlockIndex(bx,by,pnt)==YSOK)
			{
				YsShell2dLatticeElem *elm;
				elm=GetEditableBlock(bx,by);
				if(elm!=NULL)
				{
					for(i=elm->vtxList.GetNumItem()-1; i>=0; i--)
					{
						if(elm->vtxList[i]==vtHd)
						{
							elm->vtxList.Delete(i);
						}
					}
					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::AddEdge(YsShell2dEdgeHandle edHd)
{
	if(shl!=NULL)
	{
		YsShell2dVertexHandle edVtId1,edVtId2;
		if(shl->GetEdgeVertexHandle(edVtId1,edVtId2,edHd)==YSOK)
		{
			YsVec2 p1,p2;
			int bx1,by1,bx2,by2;
			if(shl->GetProjectedVertexPosition(p1,edVtId1)==YSOK &&
			   shl->GetProjectedVertexPosition(p2,edVtId2)==YSOK &&
			   GetBlockIndex(bx1,by1,p1)==YSOK &&
			   GetBlockIndex(bx2,by2,p2)==YSOK)
			{
				YsVec2 r1,r2,c1,c2;
				int bx,by,x1,y1,x2,y2;
				x1=YsSmaller(bx1,bx2);
				y1=YsSmaller(by1,by2);
				x2=YsGreater(bx1,bx2);
				y2=YsGreater(by1,by2);

				if(shl->GetEdgeVirtualFlag(edHd)!=YSTRUE)                          // 2005/07/11
				{
					for(by=y1; by<=y2; by++)
					{
						for(bx=x1; bx<=x2; bx++)
						{
							GetBlockRange(r1,r2,bx,by);
							if(YsClipLineSeg2(c1,c2,p1,p2,r1,r2)==YSOK)
							{
								GetEditableBlock(bx,by)->edgList.AppendItem(edHd);
								GetEditableBlock(bx,by)->side=YSBOUNDARY;
							}
						}
					}
				}
				else                                                               // 2005/07/11
				{                                                                  // 2005/07/11
					for(by=y1; by<=y2; by++)                                       // 2005/07/11
					{                                                              // 2005/07/11
						for(bx=x1; bx<=x2; bx++)                                   // 2005/07/11
						{                                                          // 2005/07/11
							GetBlockRange(r1,r2,bx,by);                            // 2005/07/11
							if(YsClipLineSeg2(c1,c2,p1,p2,r1,r2)==YSOK)            // 2005/07/11
							{                                                      // 2005/07/11
								GetEditableBlock(bx,by)->edgList.AppendItem(edHd); // 2005/07/11
							}                                                      // 2005/07/11
						}                                                          // 2005/07/11
					}                                                              // 2005/07/11
				}                                                                  // 2005/07/11

				return YSOK;
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::DeleteEdge(YsShell2dEdgeHandle edHd,YSSIDE cellMustBecome)
{
	YsShell2dVertexHandle edVtId1,edVtId2;
	if(shl->GetEdgeVertexHandle(edVtId1,edVtId2,edHd)==YSOK)
	{
		YsVec2 p1,p2;
		int bx1,by1,bx2,by2;
		if(shl->GetProjectedVertexPosition(p1,edVtId1)==YSOK &&
		   shl->GetProjectedVertexPosition(p2,edVtId2)==YSOK &&
		   GetBlockIndex(bx1,by1,p1)==YSOK &&
		   GetBlockIndex(bx2,by2,p2)==YSOK)
		{
			int bx,by,x1,y1,x2,y2;
			x1=YsSmaller(bx1,bx2);
			y1=YsSmaller(by1,by2);
			x2=YsGreater(bx1,bx2);
			y2=YsGreater(by1,by2);

			for(by=y1; by<=y2; by++)
			{
				for(bx=x1; bx<=x2; bx++)
				{
					YsVec2 r1,r2,c1,c2;
					GetBlockRange(r1,r2,bx,by);
					if(YsClipLineSeg2(c1,c2,p1,p2,r1,r2)==YSOK)
					{
						YsShell2dLatticeElem *blk;
						blk=GetEditableBlock(bx,by);

						int i;
						YSBOOL found;
						found=YSFALSE;
						for(i=blk->edgList.GetNumItem()-1; i>=0; i--)
						{
							if(blk->edgList[i]==edHd)
							{
								blk->edgList.Delete(i);
								if(found!=YSTRUE)
								{
									found=YSTRUE;
								}
								else
								{
									YsErrOut("YsShell2dLattice::DeleteEdge()\n");
									YsErrOut("  Doubly registered edge is found..\n");
									YsErrOut("  %d %d\n",shl->GetSearchKey(edVtId1),shl->GetSearchKey(edVtId2));
								}
							}
						}
						if(found==YSTRUE)
						{
							int nNonVirtualEdge;
							nNonVirtualEdge=0;
							forYsArray(i,blk->edgList)
							{
								if(shl->GetEdgeVirtualFlag(blk->edgList[i])!=YSTRUE)
								{
									nNonVirtualEdge++;
								}
							}

							if(nNonVirtualEdge==0 &&
							   shl->GetEdgeVirtualFlag(edHd)!=YSTRUE) // 2005/07/11
							{
								blk->side=cellMustBecome;  // Experimental YSUNKNOWNSIDE;
								numberOfUndeterminedCellCreatedByDeleteEdge++;
							}
						}
						else
						{
							printf("YsShell2dLattice::DeleteEdge()\n");
							printf("  Edge not found.\n");
						}
					}
				}
			}

			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::FillUndeterminedBlock(void)
{
	if(shl!=NULL)
	{
		int n;
		int bx,by;
		YsVec2 p1,p2,c;
		n=0;
		for(by=0; by<GetNumBlockY(); by++)
		{
			for(bx=0; bx<GetNumBlockX(); bx++)
			{
				if(GetBlock(bx,by)->side==YSUNKNOWNSIDE)
				{
					GetBlockRange(p1,p2,bx,by);
					c=(p1+p2)/2.0;

					YSSIDE side;
					side=shl->CheckInsidePolygon(c);

					// if(side==YSUNKNOWNSIDE)//####
					//{
					//	printf("Bingo!\n");
					//	YsShell2dVertexHandle vtHd;
					//	YsShell2dEdgeHandle edHd;
					//
					//	FILE *fp;
					//	fp=fopen("debug.srf","w");
					//	fprintf(fp,"SURF\n");
					//
					//	shl->Encache();
					//
					//	vtHd=NULL;
					//	while((vtHd=shl->FindNextVertex(vtHd))!=NULL)
					//	{
					//		YsVec2 vec;
					//		shl->GetProjectedVertexPosition(vec,vtHd);
					//		fprintf(fp,"V %lf %lf %lf\n",vec.x(),vec.y(),0.0);
					//	}
					//
					//	edHd=NULL;
					//	while((edHd=shl->FindNextEdge(edHd))!=NULL)
					//	{
					//		YsShell2dVertexHandle vtHd1,vtHd2;
					//		int id1,id2;
					//		shl->GetEdgeVertexHandle(vtHd1,vtHd2,edHd);
					//		id1=shl->GetVtIdFromHandle(vtHd1);
					//		id2=shl->GetVtIdFromHandle(vtHd2);
					//		fprintf(fp,"F\n");
					//		fprintf(fp,"V %d %d %d\n",id1,id2,id1);
					//		fprintf(fp,"E\n");
					//	}
					//
					//	fprintf(fp,"E\n");
					//	fclose(fp);
					//	gets("    ");
					//}

					if(side!=YSUNKNOWNSIDE)
					{
						n++;
						FillUndeterminedBlock(bx,by,side);
					}
					else
					{
						YsErrOut("Warning!\n",n);
						YsErrOut("  At least one block is indeterminable.\n");
					}
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}

YSSIDE YsShell2dLattice::CheckInsidePolygon(const YsVec2 &pnt,YSBOOL showWhichFunctionDetermined) const
{
	YSSIDE sd;
	if(shl!=NULL)
	{
		int bx,by;
		const YsShell2dLatticeElem *elm;
		GetBlockIndex(bx,by,pnt);

		elm=GetBlock(bx,by);

		if(elm!=NULL)
		{
			YsShell2dVertexHandle edVtId1,edVtId2;
			if(elm->side!=YSUNKNOWNSIDE && elm->side!=YSBOUNDARY)
			{
				if(showWhichFunctionDetermined==YSTRUE)
				{
					YsPrintf("(*)\n");
				}
				return elm->side;
			}

			if(elm->edgList.GetNumItem()==1 &&
			   shl->GetEdgeVirtualFlag(elm->edgList[0])!=YSTRUE && // 2005/07/11
			   shl->GetEdgeVertexHandle(edVtId1,edVtId2,elm->edgList[0])==YSOK)
			{
				int x,y,i;
				int offset[]=
				{
					 1, 0,
				    -1, 0,
				     0, 1,
				     0,-1,
				};
				YsVec2 edVt1,edVt2,nea;

				edVt1=shl->GetProjectedVertexPosition(edVtId1);
				edVt2=shl->GetProjectedVertexPosition(edVtId2);

				if(YsGetNearestPointOnLine2(nea,edVt1,edVt2,pnt)==YSOK && nea==pnt)
				{
					if(showWhichFunctionDetermined==YSTRUE)
					{
						YsPrintf("(0)\n");
					}
					return YSBOUNDARY;
				}

				for(i=0; i<4; i++)
				{
					x=bx+offset[i*2];
					y=by+offset[i*2+1];
					elm=GetBlock(x,y);
					if(elm!=NULL && (elm->side==YSINSIDE || elm->side==YSOUTSIDE))
					{
						double flip1,flip2;  // Flip Direction
						YsVec2 b1,b2,c;
						GetBlockRange(b1,b2,x,y);
						c=(b1+b2)/2.0;

						flip1=(edVt2-edVt1)^(pnt-edVt1);
						flip2=(edVt2-edVt1)^(c-edVt1);
						if(flip1*flip2>YsTolerance)
						{
							if(showWhichFunctionDetermined==YSTRUE)
							{
								YsPrintf("(1)\n");
							}
							return elm->side;
						}
						else if(flip1*flip2<-YsTolerance)
						{
							if(showWhichFunctionDetermined==YSTRUE)
							{
								YsPrintf("(1)\n");
							}
							return (elm->side==YSINSIDE ? YSOUTSIDE : YSINSIDE);
						}
					}
				}
			}

			sd=SophisticatedCheckInsidePolygonWhenTheCellIsMarkedAsBoundary(pnt,showWhichFunctionDetermined);
			if(sd!=YSUNKNOWNSIDE)
			{
				if(showWhichFunctionDetermined==YSTRUE)
				{
					YsPrintf("(2)\n");
				}
				return sd;
			}
		}
		if(showWhichFunctionDetermined==YSTRUE)
		{
			YsPrintf("(3)\n");
		}
		return shl->CheckInsidePolygon(pnt,showWhichFunctionDetermined);
	}
	return YSUNKNOWNSIDE;
}

YSSIDE YsShell2dLattice::CheckInsidePolygon(const YsVec3 &pnt,YSBOOL showWhichFunctionDetermined) const
{
	// Check after applying shl->matrix
	YsVec3 x;
	YsVec2 pnt2d;
	shl->GetProjectionMatrix().MulInverse(x,pnt,1.0);

	pnt2d.Set(x.x(),x.y());

	return CheckInsidePolygon(pnt2d,showWhichFunctionDetermined);
}

YSRESULT YsShell2dLattice::FindVertexListByPoint(YsArray <YsShell2dVertexHandle> &vtHdList,const YsVec2 &pnt) const
{
	int bx0,by0,bx1,by1,bx,by;
	const YsShell2dLatticeElem *elm;
	vtHdList.Set(0,NULL);
	if(GetBlockIndex(bx,by,pnt)==YSOK && (elm=GetBlock(bx,by))!=NULL)
	{
		YsVec2 bbx[2];
		double s,t;
		GetBlockRange(bbx[0],bbx[1],bx,by);

		s=(pnt.x()-bbx[0].x())/(bbx[1].x()-bbx[0].x());
		t=(pnt.y()-bbx[0].y())/(bbx[1].y()-bbx[0].y());

		if(s<0.03)
		{
			bx0=bx-1;
			bx1=bx;
		}
		else if(0.97<s)
		{
			bx0=bx;
			bx1=bx+1;
		}
		else
		{
			bx0=bx;
			bx1=bx;
		}

		if(t<0.03)
		{
			by0=by-1;
			by1=by;
		}
		else if(0.97<t)
		{
			by0=by;
			by1=by+1;
		}
		else
		{
			by0=by;
			by1=by;
		}

		for(bx=bx0; bx<=bx1; bx++)
		{
			for(by=by0; by<=by1; by++)
			{
				if((elm=GetBlock(bx,by))!=NULL)
				{
					vtHdList.Append(elm->vtxList.GetN(),elm->vtxList);
				}
			}
		}

		if(bx0!=bx1 || by0!=by1)
		{
			int i,j;
			for(i=0; i<vtHdList.GetN(); i++)
			{
				for(j=vtHdList.GetN()-1; j>i; j--)
				{
					if(vtHdList[i]==vtHdList[j])
					{
						vtHdList.DeleteBySwapping(j);
					}
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::FindVertexListByRegion
   (YsArray <YsShell2dVertexHandle> &v2HdList,const YsVec2 &p0,const YsVec2 &p1) const
{
	int bx0,by0,bx1,by1;
	if(GetBlockIndex(bx0,by0,p0)==YSOK && GetBlockIndex(bx1,by1,p1)==YSOK)
	{
		return MakeListOfVertexInBlockRegion(v2HdList,bx0,by0,bx1,by1);
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::FindEdgeListByRegion
   (YsArray <YsShell2dEdgeHandle> &edHdList,const YsVec2 &p0,const YsVec2 &p1) const
{
	int bx0,by0,bx1,by1;
	if(GetBlockIndex(bx0,by0,p0)==YSOK && GetBlockIndex(bx1,by1,p1)==YSOK)
	{
		return MakeListOfEdgesThatGoThroughTheBlockRegion(edHdList,bx0,by0,bx1,by1);
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::FindEdgeListByPoint(YsArray <YsShell2dEdgeHandle> &edHdList,const YsVec2 &pnt) const
{
	int bx,by,bx0,by0,bx1,by1;
	const YsShell2dLatticeElem *elm;

	edHdList.Set(0,NULL);

	if(GetBlockIndex(bx,by,pnt)==YSOK)
	{
		YsVec2 bbx[2];
		double s,t;
		GetBlockRange(bbx[0],bbx[1],bx,by);

		s=(pnt.x()-bbx[0].x())/(bbx[1].x()-bbx[0].x());
		t=(pnt.y()-bbx[0].y())/(bbx[1].y()-bbx[0].y());

		if(s<0.03)
		{
			bx0=bx-1;
			bx1=bx;
		}
		else if(0.97<s)
		{
			bx0=bx;
			bx1=bx+1;
		}
		else
		{
			bx0=bx;
			bx1=bx;
		}

		if(t<0.03)
		{
			by0=by-1;
			by1=by;
		}
		else if(0.97<t)
		{
			by0=by;
			by1=by+1;
		}
		else
		{
			by0=by;
			by1=by;
		}

		for(bx=bx0; bx<=bx1; bx++)
		{
			for(by=by0; by<=by1; by++)
			{
				if((elm=GetBlock(bx,by))!=NULL)
				{
					edHdList.Append(elm->edgList.GetN(),elm->edgList);
				}
			}
		}

		if(bx0!=bx1 || by0!=by1)
		{
			int i,j;
			for(i=0; i<edHdList.GetN(); i++)
			{
				for(j=edHdList.GetN()-1; j>i; j--)
				{
					if(edHdList[i]==edHdList[j])
					{
						edHdList.DeleteBySwapping(j);
					}
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2dLattice::FillUndeterminedBlock(int bx,int by,YSSIDE side,int limitter)
{
	YsArray <YsFixedLengthArray <int,2>,128> todo;
	YsFixedLengthArray <int,2> p;
	int maxDepth;

	p.dat[0]=bx;
	p.dat[1]=by;
	todo.AppendItem(p);

	maxDepth=1;
	while(todo.GetNumItem()>0)
	{
		// Pop & delete last one in the queue
		int nTodo;
		nTodo=todo.GetNumItem();
		p=todo[nTodo-1];
		todo.Delete(nTodo-1);

		YsShell2dLatticeElem *elm;
		bx=p.dat[0];
		by=p.dat[1];
		elm=GetEditableBlock(bx,by);
		if(elm!=NULL && elm->side==YSUNKNOWNSIDE)
		{
			elm->side=side;
			p.dat[0]=bx+1;
			p.dat[1]=by;
			todo.AppendItem(p);
			p.dat[0]=bx-1;
			p.dat[1]=by;
			todo.AppendItem(p);
			p.dat[0]=bx;
			p.dat[1]=by+1;
			todo.AppendItem(p);
			p.dat[0]=bx;
			p.dat[1]=by-1;
			todo.AppendItem(p);

			maxDepth=YsGreater(todo.GetNumItem(),maxDepth);
		}
	}

	return YSOK;
}

YSSIDE YsShell2dLattice::SophisticatedCheckInsidePolygonWhenTheCellIsMarkedAsBoundary
    (const YsVec2 &pnt,YSBOOL showDetail) const
{
	// See note for more detail. I don't want the source messed up with the
	// flood of comment lines.

	// This function is used when the cell is marked as boundary or unknown,
	// Try best effort to avoid full scale geometry check.

	int attempt;
	int bx,by,sx,sy,vx,vy,count;  // count,vx,vy makes spiral search pattern
	const YsShell2dLatticeElem *elm;

	if(GetBlockIndex(bx,by,pnt)==YSOK)
	{
		attempt=4;
		sx=bx;
		sy=by;
		vx=1;
		vy=0;
		count=2;
		// Search a closest cell that is marked as outside/inside cell
		if(showDetail==YSTRUE)
		{
			YsPrintf("----\n");
		}

		// while(attempt>0)  // Modified 03/03/2001
		while(attempt>0 && YsAbs(sx-bx)<GetNumBlockX()+1 && YsAbs(sy-by)<GetNumBlockY()+1)
		{
			int i,j;
			for(i=0; i<count/2 && attempt>0; i++) // Make spiral search pattern
			{
				sx+=vx;
				sy+=vy;

				elm=GetBlock(sx,sy);
				if(elm!=NULL)
				{
					YSSIDE sideAtSearchBlock;
					sideAtSearchBlock=elm->side;
					if(elm->side==YSINSIDE || elm->side==YSOUTSIDE)
					{
						int CrossCounter;
						YsShell2dEdgeHandle edId;
						YsVec2 bp1,bp2,c,ray;
						YsArray <YsShell2dEdgeHandle> edgList;

						GetBlockRange(bp1,bp2,sx,sy);
						c=(bp1+bp2)/2.0;

						if(showDetail==YSTRUE)
						{
							YsPrintf("%8f %8f %8f %8f\n",pnt.x(),pnt.y(),c.x(),c.y());
						}

						if(MakeListOfEdgesThatGoThroughTheBlockRegion(edgList,bx,by,sx,sy)==YSOK)
						{
							// In the future, make separate function called:
							// MakeListOfNonVirtualEdgesThatGoThroughTheBlockRegion
							// for more efficiency.
							forYsArrayRev(j,edgList)                              // 2005/07/11
							{                                                     // 2005/07/11
								if(shl->GetEdgeVirtualFlag(edgList[j])==YSTRUE)   // 2005/07/11
								{                                                 // 2005/07/11
									edgList.DeleteBySwapping(j);                  // 2005/07/11
								}                                                 // 2005/07/11
							}                                                     // 2005/07/11

							if(edgList.GetN()==0)
							{
								YsErrOut("YsShell2dLattice::SophisticatedCheckInsidePolygonWhenTheCellIsMarkedAsBoundary()\n");
								YsErrOut("  Edge not found\n");
								return YSUNKNOWNSIDE;
							}

							CrossCounter=0;
							ray=c-pnt;

							for(j=0; j<edgList.GetNumItem(); j++)
							{
								YsVec2 vec,crs,edVt1,edVt2;
								YsShell2dVertexHandle edVtH1,edVtH2;
								double edSqLng;

								edId=edgList[j];
								shl->GetEdgeVertexHandle(edVtH1,edVtH2,edId);
								edVt1=shl->GetProjectedVertexPosition(edVtH1);
								edVt2=shl->GetProjectedVertexPosition(edVtH2);
								edSqLng=(edVt1-edVt2).GetSquareLength();

								if(pnt==edVt1 || pnt==edVt2)
								{
									return YSBOUNDARY;
								}

								if(edVt1==edVt2)
								{
									continue;
								}

								if(showDetail==YSTRUE)
								{
									YsPrintf("%08x %08x %8f %8f %8f %8f\n",
									    edVtH1,edVtH2,edVt1.x(),edVt1.y(),edVt2.x(),edVt2.y());
								}

								if(YsGetLineIntersection2(crs,edVt1,edVt2,pnt,pnt+ray)==YSOK)
								{
									if(showDetail==YSTRUE)
									{
										YsPrintf("%s\n",crs.Txt());
									}

									if(crs==pnt && YsCheckInBetween2(crs,edVt1,edVt2)==YSTRUE)
									{
										return YSBOUNDARY;
									}
									if(crs==edVt1 || crs==edVt2)
									{
										if(showDetail==YSTRUE)
										{
											YsPrintf("N-1\n");
										}
										goto NEXTATTEMPT;
									}


									double vecRay;
									vec=crs-pnt;
									vecRay=vec*ray;


									// Modified 2000/12/07
									if(vecRay>0.0 &&   // <- This condition is added on 2004/09/09
									   ((crs-edVt1).GetSquareLength()<edSqLng/10000.0 ||
									    (crs-edVt2).GetSquareLength()<edSqLng/10000.0))
									{
										// If the vertex-(end_of_edge) distance is
										// less than 1% of the edge-length, try next edge.
										if(showDetail==YSTRUE)
										{
											YsPrintf("N-2\n");
										}
										goto NEXTATTEMPT;
									}


									if(vecRay>0.0 &&
									   YsCheckInBetween2(crs,edVt1,edVt2)==YSTRUE &&
									   YsCheckInBetween2(crs,pnt,c)==YSTRUE)// This is a finite ray. Must Check Last Condition.
									{
										CrossCounter++;
										if(showDetail==YSTRUE)
										{
											YsPrintf("+ %lf %lf\n",crs.x(),crs.y());
										}
									}
									else
									{
										if(showDetail==YSTRUE)
										{
											YsPrintf("- %lf %lf\n",crs.x(),crs.y());
										}
									}
								}
								else if(YsGetLinePenetration2(edVt1,edVt2,pnt,pnt+ray)!=YSAPART)
								{
									if(showDetail==YSTRUE)
									{
										YsPrintf("N-3\n");
									}
									goto NEXTATTEMPT;
								}
							}
							if(CrossCounter%2==0)
							{
								if(showDetail==YSTRUE)
								{
									YsPrintf("BX BY(%d %d),Offset (%d %d)\n",bx,by,sx-bx,sy-by);
									YsPrintf("  Counter=%d\n",CrossCounter);
								}
								return sideAtSearchBlock;
							}
							else
							{
								if(showDetail==YSTRUE)
								{
									YsPrintf("BX BY(%d %d),Offset (%d %d)\n",bx,by,sx-bx,sy-by);
									YsPrintf("  Counter=%d\n",CrossCounter);
								}
								return (sideAtSearchBlock==YSINSIDE ? YSOUTSIDE : YSINSIDE);
							}
						}

					NEXTATTEMPT:
						attempt--;
					}
				}
			}
			count++;
			int nextVx,nextVy;
			nextVx=-vy;
			nextVy= vx;
			vx=nextVx;
			vy=nextVy;
		}
	}
	return YSUNKNOWNSIDE;
}

YSRESULT YsShell2dLattice::MakeListOfEdgesThatGoThroughTheBlockRegion
    (YsArray <YsShell2dEdgeHandle> &edgList,int xa,int ya,int xb,int yb) const
{
	int i,x,y,x1,y1,x2,y2;
	const YsShell2dLatticeElem *elm;

	edgList.Set(0,NULL);

	x1=YsSmaller(xa,xb);
	y1=YsSmaller(ya,yb);
	x2=YsGreater(xa,xb);
	y2=YsGreater(ya,yb);

	// Known working set (2005/07/11)
	// int j;
	// for(y=y1; y<=y2; y++)
	// {
	// 	for(x=x1; x<=x2; x++)
	// 	{
	// 		elm=GetBlock(x,y);
	// 		if(elm!=NULL)
	// 		{
	// 			for(i=0; i<elm->edgList.GetNumItem(); i++)
	// 			{
	// 				for(j=0; j<edgList.GetNumItem(); j++)
	// 				{
	// 					if(elm->edgList[i]==edgList[j])  // If already checked, don't add again.
	// 					{
	// 						goto NEXTEDGE;
	// 					}
	// 				}
	// 				edgList.AppendItem(elm->edgList[i]);
	// 			NEXTEDGE:
	// 				;
	// 			}
	// 		}
	// 	}
	// }

	YsArray <int,256> att4Save;
	YsArray <YsShell2dEdgeHandle,256> allEdHd;
	for(y=y1; y<=y2; y++)
	{
		for(x=x1; x<=x2; x++)
		{
			elm=GetBlock(x,y);
			if(elm!=NULL)
			{
				allEdHd.Append(elm->edgList.GetN(),elm->edgList);
			}
		}
	}

	forYsArray(i,allEdHd)
	{
		att4Save.Append(shl->GetEdgeAttrib4(allEdHd[i]));
		shl->SetEdgeAttrib4(allEdHd[i],0);
	}
	forYsArray(i,allEdHd)
	{
		if(shl->GetEdgeAttrib4(allEdHd[i])==0)
		{
			edgList.Append(allEdHd[i]);
			shl->SetEdgeAttrib4(allEdHd[i],1);
		}
	}
	forYsArrayRev(i,allEdHd)
	{
		shl->SetEdgeAttrib4(allEdHd[i],att4Save[i]);
	}

	return YSOK;
}

YSRESULT YsShell2dLattice::MakeListOfVertexInBlockRegion
   (YsArray <YsShell2dVertexHandle> &vtxList,int xa,int ya,int xb,int yb) const
{
	int i,j,x,y,x1,y1,x2,y2;
	const YsShell2dLatticeElem *elm;

	vtxList.Set(0,NULL);

	x1=YsSmaller(xa,xb);
	y1=YsSmaller(ya,yb);
	x2=YsGreater(xa,xb);
	y2=YsGreater(ya,yb);

	for(y=y1; y<=y2; y++)
	{
		for(x=x1; x<=x2; x++)
		{
			elm=GetBlock(x,y);
			if(elm!=NULL)
			{
				for(i=0; i<elm->vtxList.GetNumItem(); i++)
				{
					for(j=0; j<vtxList.GetNumItem(); j++)
					{
						if(elm->vtxList[i]==vtxList[j])  // If already checked, don't add again.
						{
							goto NEXTVTX;
						}
					}
					vtxList.AppendItem(elm->vtxList[i]);
				NEXTVTX:
					;
				}
			}
		}
	}
	return YSOK;
}



// Implementation //////////////////////////////////////////

unsigned YsShell2d::uniqueSearchKeyGenerator=1;

YsListAllocator <YsShell2dVertex> YsShell2d::vtxAllocator(16);
YsListAllocator <YsShell2dEdge> YsShell2d::edgAllocator(16);

YsShell2d::YsShell2d() : vtxList(vtxAllocator), edgList(edgAllocator)
{
	projectionIsCached=YSFALSE;

	boundingBoxIsValid=YSFALSE;
	bbxMin2.Set(0.0,0.0);
	bbxMax2.Set(0.0,0.0);
	bbxMin3.Set(0.0,0.0,0.0);
	bbxMax3.Set(0.0,0.0,0.0);

	searchTable=NULL;
}

YsShell2d::YsShell2d(const YsShell2d &from) : vtxList(vtxAllocator), edgList(edgAllocator)
{
	printf("Copy constructor for YsShell2d is no longer available!\n");
	exit(1);
}

YsShell2d::~YsShell2d()
{
	CleanUp();
}

const YsShell2d &YsShell2d::operator=(const YsShell2d &from)
{
	printf("Copy opearator for YsShell2d is no longer available!\n");
	exit(1);
	return *this;
}

void YsShell2d::Encache(void) const
{
	vtxList.Encache();
	edgList.Encache();
}

YSRESULT YsShell2d::AttachSearchTable(class YsShell2dSearchTable *search)
{
	if(searchTable==NULL)
	{
		searchTable=search;
		BuildSearchTable(*search);
		return YSOK;
	}
	else
	{
		YsErrOut("YsShell2d::AttachSearchTable()\n");
		YsErrOut("  SearchTable is already attached\n");
		return YSERR;
	}
}

YSRESULT YsShell2d::DetachSearchTable(void)
{
	if(searchTable!=NULL)
	{
		searchTable=NULL;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

const YsShell2dSearchTable *YsShell2d::GetSearchTable(void) const
{
	return searchTable;
}

void YsShell2d::CleanUp(void)
{
	basePlane.Set(YsOrigin(),YsVec3(0.0,1.0,0.0));
	proj.Initialize();


	YsListItem <YsShell2dVertex> *vtHd;
	vtxList.RewindPointer();
	while(vtHd=vtxList.StepPointer())
	{
		DestroyVertex(vtHd);
	}

	YsListItem <YsShell2dEdge> *edHd;
	edgList.RewindPointer();
	while(edHd=edgList.StepPointer())
	{
		DestroyEdge(edHd);
	}

	vtxList.CleanUp();
	edgList.CleanUp();

	projectionIsCached=YSFALSE;

	if(searchTable!=NULL)
	{
		searchTable->Prepare();
	}
}

void YsShell2d::CleanUpVertexEdgeStore(void)
{
	vtxAllocator.CollectGarbage();
	edgAllocator.CollectGarbage();
}

void YsShell2d::SetBasePlane(const YsPlane &pln)
{
	YsVec3 org,nom;
	YsAtt3 att(YsZeroAtt());

	basePlane=pln;

	nom=pln.GetNormal();
	org=pln.GetOrigin();

	att.SetForwardVector(nom);

	proj.Initialize();
	proj.Translate(org);
	proj.Rotate(att);

	projectionIsCached=YSFALSE;
	boundingBoxIsValid=YSFALSE;
}

void YsShell2d::SetNoTransformation(void)  // For purely 2d operation.
{
	proj.Initialize();
	projectionIsCached=YSTRUE;
	boundingBoxIsValid=YSFALSE;

	YsListItem <YsShell2dVertex> *vtx;
	vtxList.RewindPointer();
	while(vtx=vtxList.StepPointer())
	{
		vtx->dat.projected.Set(vtx->dat.p.x(),vtx->dat.p.y());
	}
}

YSRESULT YsShell2d::AutoComputeBasePlane(void)
{
	int i;
	YsVec3 *vtxList;
	YsOneMemoryBlock <YsVec3,128> vtxListBlock;
	YsPlane pln;
	YsShell2dVertexHandle vtx;

	boundingBoxIsValid=YSFALSE;

	vtxList=vtxListBlock.GetMemoryBlock(GetNumVertex());

	vtx=NULL;
	i=0;
	while((vtx=FindNextVertex(vtx))!=NULL)
	{
		vtxList[i]=GetVertexPosition(vtx);
		i++;
	}

	if(pln.MakeBestFitPlane(GetNumVertex(),vtxList)==YSOK)
	{
		SetBasePlane(pln);
		projectionIsCached=YSFALSE;
		return YSOK;
	}
	else
	{
		return YSERR;
	}

}

const YsPlane &YsShell2d::GetBasePlane(void) const
{
	return basePlane;
}


const YsMatrix4x4 &YsShell2d::GetProjectionMatrix(void) const
{
	return proj;
}

YSRESULT YsShell2d::MakeEdgeChain
	   (YsArray <YsShell2dEdgeHandle,16> &edHdList,YsArray <YsShell2dVertexHandle,16> &vtHdList,
	    YsShell2dEdgeHandle seedEdHd,const double &angleThr) const
{
	const YsShell2dSearchTable *search;
	search=GetSearchTable();
	if(search==NULL)
	{
		YsPrintf("YsShell2d::MakeEdgeChain()\n");
		YsPrintf("  Requires a search table.\n");
		return YSERR;
	}


	int i;
	YsShell2dEdgeHandle curEdHd,nxtEdHd;
	YsShell2dVertexHandle startVtHd[2],edVtHd[2];
	const YsArray <YsShell2dEdgeHandle> *vtxToEdg;
	YSBOOL closedLoop;
	double cosAngleThr;

	cosAngleThr=cos(angleThr);

	edHdList.Set(0,NULL);
	vtHdList.Set(0,NULL);

	GetEdgeVertexHandle(startVtHd[0],startVtHd[1],seedEdHd);

	edHdList.Append(seedEdHd);
	vtHdList.Append(2,startVtHd);

	closedLoop=YSFALSE;
	for(i=0; i<2 && closedLoop!=YSTRUE; i++)
	{
		YsShell2dVertexHandle prvVtHd,curVtHd,nxtVtHd;
		YsVec2 curVec,nxtVec,p1,p2;

		curEdHd=seedEdHd;
		prvVtHd=startVtHd[i];
		curVtHd=startVtHd[1-i];

		GetProjectedVertexPosition(p1,prvVtHd);
		GetProjectedVertexPosition(p2,curVtHd);
		curVec=p2-p1;
		curVec.Normalize();

		while(1)
		{
			vtxToEdg=search->GetVtxToEdgTable(curVtHd);
			if(vtxToEdg!=NULL && vtxToEdg->GetN()==2)
			{
				nxtVtHd=NULL;
				nxtEdHd=NULL;
				if((*vtxToEdg)[0]==curEdHd)
				{
					nxtEdHd=(*vtxToEdg)[1];
				}
				else if((*vtxToEdg)[1]==curEdHd)
				{
					nxtEdHd=(*vtxToEdg)[0];
				}

				if(nxtEdHd!=NULL)
				{
					GetEdgeVertexHandle(edVtHd[0],edVtHd[1],nxtEdHd);
					if(edVtHd[0]==curVtHd)
					{
						nxtVtHd=edVtHd[1];
					}
					else if(edVtHd[1]==curVtHd)
					{
						nxtVtHd=edVtHd[0];
					}
				}

				if(nxtVtHd!=NULL)
				{
					p1=p2;
					GetProjectedVertexPosition(p2,nxtVtHd);
					nxtVec=p2-p1;
					if(nxtVec.Normalize()==YSOK && curVec*nxtVec<cosAngleThr)
					{
						break;
					}

					edHdList.Append(nxtEdHd);
					vtHdList.Append(nxtVtHd);

					prvVtHd=curVtHd;
					curVtHd=nxtVtHd;
					curEdHd=nxtEdHd;

					curVec=nxtVec;
				}
				else
				{
					break;
				}

				if(nxtVtHd==startVtHd[0] || nxtVtHd==startVtHd[1])
				{
					closedLoop=YSTRUE;
					break;
				}
			}
			else
			{
				break;
			}
		}

		edHdList.Invert();
		vtHdList.Invert();
	}


	return YSOK;
}

YSRESULT YsShell2d::FindConnectedEdge
	   (YsArray <YsShell2dEdgeHandle,16> &edHdList,YsShell2dEdgeHandle seedEdHd) const
{
	YsHashTable <YsShell2dEdgeHandle> visited(16);
	YsArray <YsShell2dEdgeHandle,256> todo;

	visited.EnableAutoResizing(16,65536);

	const YsShell2dSearchTable *search;
	search=GetSearchTable();
	if(search==NULL)
	{
		edHdList.Set(0,NULL);
		YsPrintf("YsShell2d::FindConnectedEdge()\n");
		YsPrintf("  Requires a search table.\n");
		return YSERR;
	}

	edHdList.Set(seedEdHd);
	visited.AddElement(GetSearchKey(seedEdHd),seedEdHd);
	todo.Set(seedEdHd);

	while(todo.GetN()>0)
	{
		YsShell2dEdgeHandle edHd;
		YsShell2dVertexHandle edVtHd[2];
		const YsArray <YsShell2dEdgeHandle> *foundEdHd;
		int i,j;

		edHd=todo[0];

		GetEdgeVertexHandle(edVtHd[0],edVtHd[1],edHd);
		for(i=0; i<2; i++)
		{
			foundEdHd=search->GetVtxToEdgTable(edVtHd[i]);
			if(foundEdHd!=NULL)
			{
				for(j=0; j<foundEdHd->GetN(); j++)
				{
					if(visited.CheckKeyExist(GetSearchKey((*foundEdHd)[j]))!=YSTRUE)
					{
						edHdList.Append((*foundEdHd)[j]);
						visited.AddElement(GetSearchKey((*foundEdHd)[j]),(*foundEdHd)[j]);
						todo.Append((*foundEdHd)[j]);
					}
				}
			}
		}

		todo.DeleteBySwapping(0);
	}

	return YSOK;
}

YSRESULT YsShell2d::GetBoundingBox(YsVec2 &mn,YsVec2 &mx) const
{
	if(boundingBoxIsValid==YSTRUE)
	{
		mn=bbxMin2;
		mx=bbxMax2;
		return YSOK;
	}
	else
	{
		if(ComputeBoundingBox()==YSOK)
		{
			mn=bbxMin2;
			mx=bbxMax2;
			return YSOK;
		}
		else
		{
			mn.Set(0.0,0.0);
			mx.Set(0.0,0.0);
			return YSERR; // Means no boundary is defined
		}
	}
}


int YsShell2d::GetNumVertex(void) const
{
	return vtxList.GetN();
}

const YsVec3 &YsShell2d::GetVertexPosition(int vtId) const
{
	return vtxList.GetItemFromId(vtId)->dat.p;
}

int YsShell2d::GetVertexAttrib1(int vtId) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return GetVertexAttrib1(vtHd);
}

int YsShell2d::GetVertexAttrib2(int vtId) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return GetVertexAttrib2(vtHd);
}

int YsShell2d::GetVertexAttrib3(int vtId) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return GetVertexAttrib3(vtHd);
}

int YsShell2d::GetVertexAttrib4(int vtId) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return GetVertexAttrib4(vtHd);
}

// void *YsShell2d::GetVertexAttribPointer(int vtId) const
// {
// 	YsShell2dVertexHandle vtHd;
// 	vtHd=GetVertexHandleFromId(vtId);
// 	return GetVertexAttribPointer(vtHd);
// }

YSRESULT YsShell2d::SetVertexAttrib1(int vtId,int att) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return SetVertexAttrib1(vtHd,att);
}

YSRESULT YsShell2d::SetVertexAttrib2(int vtId,int att) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return SetVertexAttrib2(vtHd,att);
}

YSRESULT YsShell2d::SetVertexAttrib3(int vtId,int att) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return SetVertexAttrib3(vtHd,att);
}

YSRESULT YsShell2d::SetVertexAttrib4(int vtId,int att) const
{
	YsShell2dVertexHandle vtHd;
	vtHd=GetVertexHandleFromId(vtId);
	return SetVertexAttrib4(vtHd,att);
}

// YSRESULT YsShell2d::SetVertexAttribPointer(int vtId,void *att) const
// {
// 	YsShell2dVertexHandle vtHd;
// 	vtHd=GetVertexHandleFromId(vtId);
// 	return SetVertexAttribPointer(vtHd,att);
// }

int YsShell2d::GetVertexReferenceCount(int vtId) const
{
	return vtxList.GetItemFromId(vtId)->dat.refCount;
}


int YsShell2d::GetNumEdge(void) const
{
	return edgList.GetN();
}

YSRESULT YsShell2d::GetEdgeVertexId(int &edVt1,int &edVt2,int edId) const
{
	YsListItem <YsShell2dEdge> *seeker;
	seeker=edgList.GetItemFromId(edId);
	if(seeker!=NULL)
	{
		edVt1=GetVtIdFromHandle(seeker->dat.vtId1);
		edVt2=GetVtIdFromHandle(seeker->dat.vtId2);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

int YsShell2d::GetEdgeAttrib1(int edId) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return GetEdgeAttrib1(vtHd);
}

int YsShell2d::GetEdgeAttrib2(int edId) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return GetEdgeAttrib2(vtHd);
}

int YsShell2d::GetEdgeAttrib3(int edId) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return GetEdgeAttrib3(vtHd);
}

int YsShell2d::GetEdgeAttrib4(int edId) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return GetEdgeAttrib4(vtHd);
}

unsigned YsShell2d::GetSearchKey(YsShell2dEdgeHandle edHd) const
{
	return edHd->dat.searchKey;
}

// void *YsShell2d::GetEdgeAttribPointer(int edId) const
// {
// 	YsShell2dEdgeHandle vtHd;
// 	vtHd=GetEdgeHandleFromId(edId);
// 	return GetEdgeAttribPointer(vtHd);
// }

YSRESULT YsShell2d::SetEdgeAttrib1(int edId,int att) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return SetEdgeAttrib1(vtHd,att);
}

YSRESULT YsShell2d::SetEdgeAttrib2(int edId,int att) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return SetEdgeAttrib2(vtHd,att);
}

YSRESULT YsShell2d::SetEdgeAttrib3(int edId,int att) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return SetEdgeAttrib3(vtHd,att);
}

YSRESULT YsShell2d::SetEdgeAttrib4(int edId,int att) const
{
	YsShell2dEdgeHandle vtHd;
	vtHd=GetEdgeHandleFromId(edId);
	return SetEdgeAttrib4(vtHd,att);
}

// YSRESULT YsShell2d::SetEdgeAttribPointer(int edId,void *att) const
// {
// 	YsShell2dEdgeHandle vtHd;
// 	vtHd=GetEdgeHandleFromId(edId);
// 	return SetEdgeAttribPointer(vtHd,att);
// }

YSRESULT YsShell2d::GetProjectedVertexPosition(YsVec2 &pnt,int vtId) const
{
	ComputeProjection();
	YsListItem <YsShell2dVertex> *seeker;
	seeker=vtxList.GetItemFromId(vtId);
	if(seeker!=NULL)
	{
		pnt=seeker->dat.projected;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::GetProjectedVertexPosition(YsVec2 &pnt,YsShell2dVertexHandle vtx) const
{
	ComputeProjection();
	if(vtx!=NULL)
	{
		pnt=vtx->dat.projected;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::BuildSearchTable(class YsShell2dSearchTable &search) const
{
	YsShell2dEdgeHandle edg;

	search.Prepare();

	edg=NULL;
	while((edg=FindNextEdge(edg))!=NULL)
	{
		if(search.AddEdge(*this,edg)!=YSOK)
		{
			YsErrOut("YsShell2d::BuildSearchTable()\n");
			YsErrOut("  Error1\n");
			return YSERR;
		}
	}
	return YSOK;
}


int YsShell2d::AddVertex(const YsVec3 &pos,int attrib)
{
	YsShell2dVertexHandle neo;
	neo=AddVertexH(pos,attrib);
	if(neo!=NULL)
	{
		return vtxList.GetN()-1;
	}
	else
	{
		return -1;
	}
}

int YsShell2d::AddEdge(int vtId1,int vtId2)
{
	YsShell2dEdgeHandle neo;
	YsShell2dVertexHandle vtHd1,vtHd2;

	vtHd1=GetVertexHandleFromId(vtId1);
	vtHd2=GetVertexHandleFromId(vtId2);

	neo=AddEdgeH(vtHd1,vtHd2);
	if(neo!=NULL)
	{
		return edgList.GetN()-1;
	}
	else
	{
		return -1;
	}
}

YSRESULT YsShell2d::DeleteEdge(int edId)
{
	YsShell2dEdgeHandle edHd;
	edHd=GetEdgeHandleFromId(edId);
	return DeleteEdge(edHd);
}

YsShell2dVertexHandle YsShell2d::AddVertexH(const YsVec3 &pos,int attrib)
{
	YsListItem <YsShell2dVertex> *neo;

	neo=CreateVertex();
	if(neo!=NULL)
	{
		neo->dat.p=pos;
		neo->dat.att1=attrib;
		neo->dat.searchKey=GetUniqueSearchKey();
		neo->dat.thisInTheList=neo;

		if(projectionIsCached==YSTRUE)
		{
			YsVec3 projected;
			proj.MulInverse(projected,pos,1.0);
			neo->dat.projected.Set(projected.x(),projected.y());
		}

		if(searchTable!=NULL)
		{
			searchTable->AddVertex(*this,neo);
		}

		boundingBoxIsValid=YSFALSE;
		return neo;
	}
	else
	{
		return NULL;
	}
}

YsShell2dVertexHandle YsShell2d::AddVertexH(const YsVec2 &pos,int attrib)
{
	YsListItem <YsShell2dVertex> *neo;

	neo=CreateVertex();
	if(neo!=NULL)
	{
		neo->dat.projected=pos;
		neo->dat.p.Set(pos.x(),pos.y(),0.0);

		neo->dat.att1=attrib;
		neo->dat.searchKey=GetUniqueSearchKey();
		neo->dat.thisInTheList=neo;

		if(searchTable!=NULL)
		{
			searchTable->AddVertex(*this,neo);
		}

		boundingBoxIsValid=YSFALSE;
		return neo;
	}
	else
	{
		return NULL;
	}
}

YsShell2dEdgeHandle YsShell2d::AddEdgeH(YsShell2dVertexHandle vtId1,YsShell2dVertexHandle vtId2)
{
	YsListItem <YsShell2dEdge> *neo;
	YsListItem <YsShell2dVertex> *vtx1,*vtx2;

	if(vtId1!=NULL && vtId2!=NULL)
	{
		neo=CreateEdge();
		if(neo!=NULL)
		{
			neo->dat.vtId1=vtId1;
			neo->dat.vtId2=vtId2;
			neo->dat.searchKey=GetUniqueSearchKey();
			neo->dat.thisInTheList=neo;

			vtx1=GetVertexPointerFromHandle(vtId1);
			vtx2=GetVertexPointerFromHandle(vtId2);

			vtx1->dat.refCount++;
			vtx2->dat.refCount++;

			if(searchTable!=NULL)
			{
				searchTable->AddEdge(*this,neo);
			}

			return neo;
		}
	}
	return NULL;
}

YSRESULT YsShell2d::SetVertexPosition(YsShell2dVertexHandle vtHd,const YsVec3 &pos)
{
	YsListItem <YsShell2dVertex> *vtx;
	vtx=GetVertexPointerFromHandle(vtHd);
	if(vtx!=NULL)
	{
		vtx->dat.p=pos;

		if(projectionIsCached==YSTRUE)
		{
			YsVec3 projected;
			proj.MulInverse(projected,pos,1.0);
			vtx->dat.projected.Set(projected.x(),projected.y());
		}

		boundingBoxIsValid=YSFALSE;

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell2d::SetVertexPosition(YsShell2dVertexHandle vtHd,const YsVec2 &pos)
{
	YsListItem <YsShell2dVertex> *vtx;
	vtx=GetVertexPointerFromHandle(vtHd);
	if(vtx!=NULL)
	{
		vtx->dat.projected=pos;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

//DeleteVertex
//{
//    Do something....
//    boundingBoxIsValid=YSFALSE;
//    don't directly do vtx->DeleteList(), but use DestroyVertex(vtx)
//}

YSRESULT YsShell2d::DeleteVertex(YsShell2dVertexHandle vtHd)
{
	if(vtHd!=NULL && GetVertexReferenceCount(vtHd)==0)
	{
		YsListItem <YsShell2dVertex> *vtx;
		vtx=GetVertexPointerFromHandle(vtHd);

		boundingBoxIsValid=YSFALSE;

		if(searchTable!=NULL)
		{
			searchTable->DeleteVertex(*this,vtHd);
		}
		DestroyVertex(vtx);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell2d::DeleteEdge(YsShell2dEdgeHandle edId)
{
	YsShell2dVertexHandle vtHd1,vtHd2;
	YsListItem <YsShell2dVertex> *vtx1,*vtx2;
	YsListItem <YsShell2dEdge> *edg;

	if(edId!=NULL)
	{
		GetEdgeVertexHandle(vtHd1,vtHd2,edId);

		vtx1=GetVertexPointerFromHandle(vtHd1);
		vtx2=GetVertexPointerFromHandle(vtHd2);

		vtx1->dat.refCount--;
		vtx2->dat.refCount--;

		edg=GetEdgePointerFromHandle(edId);

		if(searchTable!=NULL)
		{
			searchTable->DeleteEdge(*this,edId);
		}
		DestroyEdge(edg);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}


YSSIDE YsShell2d::CheckInsidePolygon(const YsVec2 &pnt,YSBOOL showDetail) const
{
	ComputeProjection();

	// Most part is the copy of YsCheckInsidePolygon3
	int i,CrossCounter;
	YsVec2 crs,vec;
	YsVec2 ray,avn;

	YsVec2 bbxMin,bbxMax;
	if(GetBoundingBox(bbxMin,bbxMax)!=YSOK)
	{
		return YSOUTSIDE; // Means no boundary is defined
	}

	if(YsCheckInsideBoundingBox2(pnt,bbxMin,bbxMax)!=YSTRUE)
	{
		return YSOUTSIDE;
	}


	YSBOOL reliability;
	int vote[2];  // 0:Outside  1:Inside
	double angle;

	angle=YsPi/10.0;  // Fixing for debugging YsPi*2.0*(double)(rand()%255)/255.0;
	vote[0]=0;
	vote[1]=0;

	for(i=0; i<8; i++)
	{
		YsVec2 edVt1,edVt2;
		YsShell2dVertexHandle edVtH1,edVtH2;
		YsShell2dEdgeHandle edg2Handle;

		ray.Set(cos(angle),sin(angle));

		CrossCounter=0;
		edg2Handle=NULL;
		reliability=YSTRUE;
		while((edg2Handle=FindNextEdge(edg2Handle))!=NULL)
		{
			if(GetEdgeVirtualFlag(edg2Handle)==YSTRUE)  // 2005/07/11
			{                                           // 2005/07/11
				continue;                               // 2005/07/11
			}                                           // 2005/07/11

			GetEdgeVertexHandle(edVtH1,edVtH2,edg2Handle);
			edVt1=GetProjectedVertexPosition(edVtH1);
			edVt2=GetProjectedVertexPosition(edVtH2);

			if(pnt==edVt1 || pnt==edVt2)
			{
				return YSBOUNDARY;
			}

			if(edVt1==edVt2) // (*B)Do not confuse with (*A)
			{
				continue;
			}

			if(YsIsLineSegIntersectionReliable2(edVt1,edVt2,pnt,pnt+ray)!=YSTRUE)
			{
				reliability=YSFALSE;
			}

			if(YsGetLineIntersection2(crs,edVt1,edVt2,pnt,pnt+ray)==YSOK)
			{
				if(crs==pnt && YsCheckInBetween2(crs,edVt1,edVt2)==YSTRUE)
				{
					return YSBOUNDARY;
				}
				if(crs==edVt1 || crs==edVt2)
				{
					goto NEXTANGLE;
				}

				if(showDetail==YSTRUE)
				{
					YsPrintf("CRS=%s\n",crs.Txt());
				}

				vec=crs-pnt;
				if(vec*ray>0.0 && YsCheckInBetween2(crs,edVt1,edVt2)==YSTRUE)
				{
					if(showDetail==YSTRUE)
					{
						YsPrintf("Counter++\n");
					}
					CrossCounter++;
				}

			}
			else if(YsGetLinePenetration2(edVt1,edVt2,pnt,pnt+ray)!=YSAPART)
			{
				goto NEXTANGLE;
			}
		}

		if(showDetail==YSTRUE)
		{
			YsPrintf("CTR=%d\n",CrossCounter);
		}

		if(reliability==YSTRUE)
		{
			if(CrossCounter%2==0)
			{
				return YSOUTSIDE;
			}
			else
			{
				return YSINSIDE;
			}
		}
		else
		{
			vote[CrossCounter%2]++;
			if(showDetail==YSTRUE)
			{
				YsPrintf("Unreliable.  Vote Out %d vs. In %d\n",vote[0],vote[1]);
			}
		}

	NEXTANGLE:
		angle+=YsPi*3.0/4.0;;
	}

	if(vote[0]>vote[1])
	{
		if(showDetail==YSTRUE)
		{
			YsPrintf("Voted outside. %d vs %d\n",vote[0],vote[1]);
		}
		return YSOUTSIDE;
	}
	else if(vote[1]>vote[0])
	{
		if(showDetail==YSTRUE)
		{
			YsPrintf("Voted inside. %d vs %d\n",vote[0],vote[1]);
		}
		return YSINSIDE;
	}

	YsErrOut("YsShell2d::CheckInsidePolygon\n  Could not determine inside/outside/boundary.\n");
	return YSUNKNOWNSIDE;
}

YSSIDE YsShell2d::CheckInsidePolygon(const YsVec3 &pnt3d) const
{
	YsVec2 pnt;
	YsVec3 tmp;

	proj.MulInverse(tmp,pnt3d,1.0);
	pnt.Set(tmp.x(),tmp.y());

	return CheckInsidePolygon(pnt);
}

YSRESULT YsShell2d::ShootFiniteRay
    (YsArray <YsVec3> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
     const YsVec3 &p1,const YsVec3 &p2,const YsShell2dLattice *ltc) const
{
	YsArray <YsVec2> is2d;
	YsVec2 q1,q2;
	YsVec3 tmp;

	proj.MulInverse(tmp,p1,1.0);
	q1.Set(tmp.x(),tmp.y());
	proj.MulInverse(tmp,p2,1.0);
	q2.Set(tmp.x(),tmp.y());

	is.Set(0,NULL);
	edHdLst.Set(0,NULL);

	if(ShootFiniteRay(is2d,edHdLst,q1,q2,ltc)==YSOK)
	{
		int i;
		for(i=0; i<is2d.GetNumItem(); i++)
		{
			tmp.Set(is2d[i].x(),is2d[i].y(),0.0);
			proj.Mul(tmp,tmp,1.0);
			is.AppendItem(tmp);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::ShootFiniteRay
    (YsArray <YsVec2> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
     const YsVec2 &pnt1,const YsVec2 &pnt2,const YsShell2dLattice *ltc,
     YsShell2dVertexHandle excludeVtx1,YsShell2dVertexHandle excludeVtx2) const
{
	ComputeProjection();

	YsShell2dEdgeHandle edId;
	YsShell2dVertexHandle edVtId1,edVtId2;
	YsVec2 edVt1,edVt2,crs;

	is.Set(0,NULL);
	edHdLst.Set(0,NULL);

	if(ltc==NULL)
	{
		edId=NULL;
		while((edId=FindNextEdge(edId))!=NULL)
		{
			GetEdgeVertexHandle(edVtId1,edVtId2,edId);
			if(edVtId1!=excludeVtx1 && edVtId1!=excludeVtx2 &&
			   edVtId2!=excludeVtx1 && edVtId2!=excludeVtx2)
			{
				edVt1=GetProjectedVertexPosition(edVtId1);
				edVt2=GetProjectedVertexPosition(edVtId2);

				if(YsGetLineIntersection2(crs,edVt1,edVt2,pnt1,pnt2)==YSOK &&
				   YsCheckInBetween2(crs,edVt1,edVt2)==YSTRUE &&
				   YsCheckInBetween2(crs,pnt1,pnt2)==YSTRUE)
				{
					is.AppendItem(crs);
					edHdLst.AppendItem(edId);
				}
			}
		}
	}
	else
	{
		int i,j;
		YsArray <YsShell2dEdgeHandle> toCheck;
		int bx,by,bx1,by1,bx2,by2;
		YsVec2 range1,range2;
		YsVec2 clip1,clip2;

		// Modified 2000/01/04
		ltc->GetLatticeRange(range1,range2);
		YsClipLineSeg2(clip1,clip2,pnt1,pnt2,range1,range2);
		if(ltc->GetBlockIndexAutoBound(bx1,by1,clip1)==YSOK &&
		   ltc->GetBlockIndexAutoBound(bx2,by2,clip2)==YSOK)  // Error check added on 2003/08/04
		{
			for(by=YsSmaller(by1,by2); by<=YsGreater(by1,by2); by++)
			{
				for(bx=YsSmaller(bx1,bx2); bx<=YsGreater(bx1,bx2); bx++)
				{
					const YsShell2dLatticeElem *elm;
					elm=ltc->GetBlock(bx,by);
					if(elm!=NULL)
					{
						for(i=0; i<elm->edgList.GetNumItem(); i++)
						{
							edId=elm->edgList[i];
							GetEdgeVertexHandle(edVtId1,edVtId2,edId);
							if(edVtId1!=excludeVtx1 && edVtId1!=excludeVtx2 &&
							   edVtId2!=excludeVtx1 && edVtId2!=excludeVtx2)
							{
								for(j=0; j<toCheck.GetNumItem(); j++)
								{
									if(toCheck[j]==edId)
									{
										goto NEXTEDGE;
									}
								}
								toCheck.AppendItem(edId);
							}
						NEXTEDGE:
							;
						}
					}
					else
					{
						YsErrOut("YsShell2d::ShootFiniteRay()\n");
						YsErrOut("  Cannot find block element that is supposed to exist.\n");
					}
				}
			}
			for(i=0; i<toCheck.GetNumItem(); i++)
			{
				edId=toCheck[i];

				GetEdgeVertexHandle(edVtId1,edVtId2,edId);
				edVt1=GetProjectedVertexPosition(edVtId1);
				edVt2=GetProjectedVertexPosition(edVtId2);

				if(YsGetLineIntersection2(crs,edVt1,edVt2,pnt1,pnt2)==YSOK &&
				   YsCheckInBetween2(crs,edVt1,edVt2)==YSTRUE &&
				   YsCheckInBetween2(crs,pnt1,pnt2)==YSTRUE)
				{
					is.AppendItem(crs);
					edHdLst.AppendItem(edId);
				}
			}
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShell2d::ShootFiniteRay
    (YsArray <YsVec3> &is,
     const YsVec3 &p1,const YsVec3 &p2,const YsShell2dLattice *ltc) const
{
	YsArray <YsShell2dEdgeHandle> edHdLst;
	return ShootFiniteRay(is,edHdLst,p1,p2,ltc);
}

YSRESULT YsShell2d::ShootFiniteRay
    (YsArray <YsVec2> &is,
     const YsVec2 &pnt1,const YsVec2 &pnt2,const YsShell2dLattice *ltc,
     YsShell2dVertexHandle excludeVtx1,YsShell2dVertexHandle excludeVtx2) const
{
	YsArray <YsShell2dEdgeHandle> edHdLst;
	return ShootFiniteRay(is,edHdLst,pnt1,pnt2,ltc,excludeVtx1,excludeVtx2);
}

YSRESULT YsShell2d::ShootInfiniteRay
    (YsArray <YsVec3> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
     const YsVec3 &org,const YsVec3 &vec,const YsShell2dLattice *ltc) const
{
	YsArray <YsVec2> is2d;
	YsVec2 org2d,vec2d;
	YsVec3 tmp;

	proj.MulInverse(tmp,org,1.0);
	org2d.Set(tmp.x(),tmp.y());
	proj.MulInverse(tmp,vec,0.0);
	vec2d.Set(tmp.x(),tmp.y());

	is.Set(0,NULL);
	edHdLst.Set(0,NULL);

	if(ShootInfiniteRay(is2d,edHdLst,org2d,vec2d,ltc)==YSOK)
	{
		int i;
		for(i=0; i<is2d.GetNumItem(); i++)
		{
			tmp.Set(is2d[i].x(),is2d[i].y(),0.0);
			proj.Mul(tmp,tmp,1.0);
			is.AppendItem(tmp);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::ShootInfiniteRay
	    (YsArray <YsVec2> &is,YsArray <YsShell2dEdgeHandle> &edHdLst,
	     const YsVec2 &org,const YsVec2 &vec,const YsShell2dLattice *ltc,
	     YsShell2dVertexHandle excludeVtx1,YsShell2dVertexHandle excludeVtx2) const
{
	YsVec2 bbx1,bbx2;

	is.Set(0,NULL);
	edHdLst.Set(0,NULL);

	if(GetBoundingBox(bbx1,bbx2)==YSOK)
	{
		YsVec2 clip1,clip2;
		if(YsClipInfiniteLine2(clip1,clip2,org,vec,bbx1,bbx2)==YSOK)
		{
			double ip1,ip2;
			ip1=(clip1-org)*vec;
			ip2=(clip2-org)*vec;

			if(ip1>=0.0 && ip2>=0.0)
			{
				return ShootFiniteRay(is,edHdLst,clip1,clip2,ltc);
			}
			else if(ip1>=0.0)
			{
				return ShootFiniteRay(is,edHdLst,org,clip1,ltc);
			}
			else if(ip2>=0.0)
			{
				return ShootFiniteRay(is,edHdLst,org,clip2,ltc);
			}
		}

		// Falling into this point means either:
		//   1. even the ray didn't hit the bounding box, or
		//   2. the ray did hit the bounding box, but both intersections were behind.

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

const YsVec2 &YsShell2d::GetProjectedVertexPosition(int vtId) const
{
	return vtxList.GetItemFromId(vtId)->dat.projected;
}

const YsVec2 &YsShell2d::GetProjectedVertexPosition(YsShell2dVertexHandle vtx) const
{
	return vtx->dat.projected;
}

YsListItem <YsShell2dVertex> *YsShell2d::CreateVertex(void)
{
	YsListItem <YsShell2dVertex> *neo;
	neo=vtxList.Create();
	if(neo!=NULL)
	{
		neo->dat.Initialize();
		AttachSpecialAttributeToNewVertex(neo->dat,neo);
		return neo;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell2d::CreateVertex()");
		return NULL;
	}
}

YsListItem <YsShell2dEdge> *YsShell2d::CreateEdge(void)
{
	YsListItem <YsShell2dEdge> *neo;
	neo=edgList.Create();
	if(neo!=NULL)
	{
		neo->dat.Initialize();
		AttachSpecialAttributeToNewEdge(neo->dat,neo);
		return neo;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell2d::CreateEdge()");
		return NULL;
	}
}

void YsShell2d::DestroyVertex(YsListItem <YsShell2dVertex> *vtx)
{
	DetachSpecialAttributeBeforeDeletingVertex(vtx->dat);
	vtxList.Delete(vtx);
	return;
}

void YsShell2d::DestroyEdge(YsListItem <YsShell2dEdge> *edg)
{
	DetachSpecialAttributeBeforeDeletingEdge(edg->dat);
	edgList.Delete(edg);
	return;
}

YSRESULT YsShell2d::ComputeProjection(void) const
{
	if(projectionIsCached!=YSTRUE)
	{
		YsMatrix4x4 inv(YSFALSE);
		YsShell2dVertexHandle vtx;
		vtx=NULL;
		inv=proj;
		inv.Invert();
		while((vtx=FindNextVertex(vtx))!=NULL)
		{
			YsVec3 projected;
			inv.Mul(projected,GetVertexPosition(vtx),1.0);
			vtx->dat.projected.Set(projected.x(),projected.y());
		}
		projectionIsCached=YSTRUE;
	}
	return YSOK;
}

YSRESULT YsShell2d::ComputeBoundingBox(void) const
{
	YSBOOL first;
	YsBoundingBoxMaker2 bbx2;
	YsBoundingBoxMaker3 bbx3;
	YsShell2dVertexHandle vtxHandle;

	ComputeProjection();

	first=YSTRUE;
	vtxHandle=NULL;
	while((vtxHandle=FindNextVertex(vtxHandle))!=NULL)
	{
		// if(GetVertexReferenceCount(vtxHandle)>0)  Commented out 2004/09/08
		{
			if(first==YSTRUE)
			{
				bbx2.Begin(GetProjectedVertexPosition(vtxHandle));
				bbx3.Begin(GetVertexPosition(vtxHandle));
				first=YSFALSE;
			}
			else
			{
				bbx2.Add(GetProjectedVertexPosition(vtxHandle));
				bbx3.Add(GetVertexPosition(vtxHandle));
			}
		}
	}
	if(first==YSTRUE)
	{
		return YSERR; // Means no boundary is defined
	}
	else
	{
		bbx2.Get(bbxMin2,bbxMax2);
		bbx3.Get(bbxMin3,bbxMax3);
		boundingBoxIsValid=YSTRUE;
		return YSOK;
	}
}

YSRESULT YsShell2d::FixLatticeAfterRemovingTriangle
	    (YsShell2dLattice &ltc,YsShell2dVertexHandle v1,YsShell2dVertexHandle v2,YsShell2dVertexHandle v3)
{
	YsVec2 tri[3],bbxMin,bbxMax;
	YsBoundingBoxMaker2 bbx;
	int bx,by,bx1,by1,bx2,by2;

	tri[0]=GetProjectedVertexPosition(v1);
	tri[1]=GetProjectedVertexPosition(v2);
	tri[2]=GetProjectedVertexPosition(v3);

	bbx.Begin(tri[0]);
	bbx.Add(tri[1]);
	bbx.Add(tri[2]);
	bbx.Get(bbxMin,bbxMax);

	ltc.GetBlockIndex(bx1,by1,bbxMin);
	ltc.GetBlockIndex(bx2,by2,bbxMax);

	for(by=by1; by<=by2; by++)
	{
		for(bx=bx1; bx<=bx2; bx++)
		{
			YsShell2dLatticeElem *elm;
			elm=ltc.GetEditableBlock(bx,by);
			if(elm->side==YSINSIDE)
			{
				YsVec2 p1,p2,c;
				ltc.GetBlockRange(p1,p2,bx,by);
				c=(p1+p2)/2.0;
				if(YsCheckInsideTriangle2(c,tri)==YSINSIDE)
				{
					elm->side=YSOUTSIDE;
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsShell2d::FixLatticeAfterUndoingTriangle
	    (YsShell2dLattice &ltc,YsShell2dVertexHandle v1,YsShell2dVertexHandle v2,YsShell2dVertexHandle v3)
{
	YsVec2 tri[3],bbxMin,bbxMax;
	YsBoundingBoxMaker2 bbx;
	int bx,by,bx1,by1,bx2,by2;

	tri[0]=GetProjectedVertexPosition(v1);
	tri[1]=GetProjectedVertexPosition(v2);
	tri[2]=GetProjectedVertexPosition(v3);

	bbx.Begin(tri[0]);
	bbx.Add(tri[1]);
	bbx.Add(tri[2]);
	bbx.Get(bbxMin,bbxMax);

	ltc.GetBlockIndex(bx1,by1,bbxMin);
	ltc.GetBlockIndex(bx2,by2,bbxMax);

	for(by=by1; by<=by2; by++)
	{
		for(bx=bx1; bx<=bx2; bx++)
		{
			YsShell2dLatticeElem *elm;
			elm=ltc.GetEditableBlock(bx,by);
			if(elm->side==YSOUTSIDE)      // <- Difference from FixLatticeAfterRemovingTriangle
			{
				YsVec2 p1,p2,c;
				ltc.GetBlockRange(p1,p2,bx,by);
				c=(p1+p2)/2.0;
				if(YsCheckInsideTriangle2(c,tri)==YSINSIDE)
				{
					elm->side=YSINSIDE;   // <- Difference from FixLatticeAfterRemovingTriangle
				}
			}
		}
	}
	return YSOK;
}

YsListItem <YsShell2dVertex> *YsShell2d::GetVertexPointerFromHandle(YsShell2dVertexHandle vtHd) const
{
	return (YsListItem <YsShell2dVertex> *)vtHd;
}

YsListItem <YsShell2dEdge> *YsShell2d::GetEdgePointerFromHandle(YsShell2dEdgeHandle edHd) const
{
	return (YsListItem <YsShell2dEdge> *)edHd;
}

unsigned YsShell2d::GetUniqueSearchKey(void)
{
	return uniqueSearchKeyGenerator++;
}

YSBOOL YsShell2d::CheckNeighborEdge(int edVtId[3],int edId1,int edId2) const
{
	YsShell2dVertexHandle edVtHd[3];
	YsShell2dEdgeHandle edHd1,edHd2;

	edHd1=GetEdgeHandleFromId(edId1);
	edHd2=GetEdgeHandleFromId(edId2);

	if(CheckNeighborEdge(edVtHd,edHd1,edHd2)==YSTRUE)
	{
		edVtId[0]=GetVtIdFromHandle(edVtHd[0]);
		edVtId[1]=GetVtIdFromHandle(edVtHd[1]);
		edVtId[2]=GetVtIdFromHandle(edVtHd[2]);
		return YSTRUE;
	}
	return YSFALSE;
}

double YsShell2d::GetCosineCornerAngle(YsShell2dVertexHandle v2Hd) const
{
	const class YsShell2dSearchTable *search;
	search=GetSearchTable();

	const YsArray <YsShell2dEdgeHandle> *edList;
	edList=search->GetVtxToEdgTable(v2Hd);
	if(edList!=NULL && edList->GetN()==2)
	{
		YsShell2dVertexHandle edV2Hd[2];
		YsShell2dVertexHandle endV2Hd[2];
		YsVec2 v[2],p[2];

		GetEdgeVertexHandle(edV2Hd[0],edV2Hd[1],(*edList)[0]);
		endV2Hd[0]=(edV2Hd[0]==v2Hd ? edV2Hd[1] : edV2Hd[0]);

		GetEdgeVertexHandle(edV2Hd[0],edV2Hd[1],(*edList)[1]);
		endV2Hd[1]=(edV2Hd[0]==v2Hd ? edV2Hd[1] : edV2Hd[0]);

		GetProjectedVertexPosition(p[0],v2Hd);

		GetProjectedVertexPosition(p[1],endV2Hd[0]);
		v[0]=p[1]-p[0];

		GetProjectedVertexPosition(p[1],endV2Hd[1]);
		v[1]=p[1]-p[0];

		v[0].Normalize();
		v[1].Normalize();
		return v[0]*v[1];
	}

	return 0.0;
}

double YsShell2d::GetEdgeSquareLength(YsShell2dEdgeHandle edHd) const
{
	YsShell2dVertexHandle edV2Hd[2];
	YsVec2 edV2Pos[2];

	GetEdgeVertexHandle(edV2Hd[0],edV2Hd[1],edHd);
	GetProjectedVertexPosition(edV2Pos[0],edV2Hd[0]);
	GetProjectedVertexPosition(edV2Pos[1],edV2Hd[1]);

	return (edV2Pos[1]-edV2Pos[0]).GetSquareLength();
}

double YsShell2d::GetEdgeLength(YsShell2dEdgeHandle edHd) const
{
	return sqrt(GetEdgeSquareLength(edHd));
}

YSBOOL YsShell2d::CheckNeighborEdge
    (YsShell2dVertexHandle edVtHd[3],YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2) const
{
	YsShell2dVertexHandle edVtHd1,edVtHd2,edVtHd3,edVtHd4;
	if(GetEdgeVertexHandle(edVtHd1,edVtHd2,edHd1)==YSOK &&
	   GetEdgeVertexHandle(edVtHd3,edVtHd4,edHd2)==YSOK)
	{
		YsShell2dVertexHandle sharedVtHd,nonSharedVtHd1,nonSharedVtHd2;

		if(edVtHd1==edVtHd3 || edVtHd1==edVtHd4)
		{
			sharedVtHd=edVtHd1;
			nonSharedVtHd1=edVtHd2;
			nonSharedVtHd2=(edVtHd3!=sharedVtHd ? edVtHd3 : edVtHd4);
		}
		else if(edVtHd2==edVtHd3 || edVtHd2==edVtHd4)
		{
			sharedVtHd=edVtHd2;
			nonSharedVtHd1=edVtHd1;
			nonSharedVtHd2=(edVtHd3!=sharedVtHd ? edVtHd3 : edVtHd4);
		}
		else
		{
			return YSFALSE; // No shared vertex
		}

		if(sharedVtHd!=nonSharedVtHd1 &&
		   sharedVtHd!=nonSharedVtHd2 &&
		   nonSharedVtHd1!=nonSharedVtHd2)
		{
			if(edVtHd!=NULL)
			{
				edVtHd[0]=nonSharedVtHd1;
				edVtHd[1]=sharedVtHd;
				edVtHd[2]=nonSharedVtHd2;
			}
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsShell2d::CheckEdgeVertexTriangleIsRemovable
    (YsShell2dEdgeHandle edHd,YsShell2dVertexHandle vtHd,const YsShell2dLattice *ltc) const
{
	YsShell2dVertexHandle edVtHd[2];
	YsVec2 edVt[3],x;

	if(GetEdgeVertexHandle(edVtHd[0],edVtHd[1],edHd)==YSOK)
	{
		// Condition
		//   1. The triangle does not enclose any other vertices
		//   2. Both mid points of line segments edVt1-x,edVt2-x are inside (not even on boundary) the shell
		//        Can I say, one point in the new triangle is enough?
		//   3. Line segment edVt1-x,edVt2-x do not intersect with other edges

		ComputeProjection();

		edVt[0]=GetProjectedVertexPosition(edVtHd[0]);
		edVt[1]=GetProjectedVertexPosition(edVtHd[1]);
		x=GetProjectedVertexPosition(vtHd);


// Experimental
//		YsVec2 mp1,mp2;
//		YSSIDE sd1,sd2;
//		mp1=(edVt[0]+x)/2.0;
//		mp2=(edVt[1]+x)/2.0;
//
//		if(ltc!=NULL)
//		{
//			sd1=ltc->CheckInsidePolygon(mp1);
//			sd2=ltc->CheckInsidePolygon(mp2);
//		}
//		else
//		{
//			sd1=CheckInsidePolygon(mp1);
//			sd2=CheckInsidePolygon(mp2);
//		}

		YSSIDE side;
		YsVec2 check;
		if(GetVertexReferenceCount(vtHd)==0)
		{
			check=x;
		}
		else
		{
			check=(x+edVt[0]+edVt[1])/3.0;
		}
		if(ltc!=NULL)
		{
			side=ltc->CheckInsidePolygon(check);
		}
		else
		{
			side=CheckInsidePolygon(check);
		}


		if(side==YSINSIDE)
		{
			int i,j;
			YsArray <YsVec2> is;
			for(i=0; i<2; i++)
			{
				if(ShootFiniteRay(is,edVt[i],x,ltc,edVtHd[i],NULL)==YSOK)
				{
					for(j=0; j<is.GetNumItem(); j++)
					{
						if(is[j]!=x && is[j]!=edVt[i])
						{
							return YSFALSE;
						}
					}
				}
				else
				{
					YsErrOut("YsShell2d::TriangleIsRemovable()\n");
					YsErrOut("  Cannot find intersections.\n");
					return YSFALSE;
				}
			}
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsShell2d::CheckEdgeEdgeTriangleIsRemovable
    (YsShell2dEdgeHandle edId1,YsShell2dEdgeHandle edId2,const YsShell2dLattice *ltc) const
{
	YsShell2dVertexHandle edVtId[3];

	if(CheckNeighborEdge(edVtId,edId1,edId2)==YSTRUE)
	{
		ComputeProjection();

		YsArray <YsVec2> is;
		YsVec2 edVt[3];

		edVt[0]=GetProjectedVertexPosition(edVtId[0]);
		edVt[1]=GetProjectedVertexPosition(edVtId[1]);
		edVt[2]=GetProjectedVertexPosition(edVtId[2]);

		if(ShootFiniteRay(is,edVt[0],edVt[2],ltc,edVtId[0],edVtId[2])==YSOK)
		{
			int i;
			for(i=0; i<is.GetNumItem(); i++)
			{
				if(edVt[0]!=is[i] && edVt[2]!=is[i])
				{
					return YSFALSE;
				}
			}

			YSSIDE sd;
			if(ltc!=NULL)
			{
				sd=ltc->CheckInsidePolygon((edVt[0]+edVt[2])/2.0);
			}
			else
			{
				sd=CheckInsidePolygon((edVt[0]+edVt[2])/2.0);
			}

			if(sd!=YSINSIDE)
			{
				return YSFALSE;
			}
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsShell2d::CheckThreeEdgeTriangleIsRemovable
    (YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2,YsShell2dEdgeHandle edHd3,const YsShell2dLattice *ltc) const
{
	int nTrV2Hd;
	YsShell2dVertexHandle trV2Hd[6],edV2Hd[2];
	YsShell2dEdgeHandle edHd[3];

	edHd[0]=edHd1;
	edHd[1]=edHd2;
	edHd[2]=edHd3;

	int i,j;
	nTrV2Hd=0;
	for(i=0; i<3; i++)
	{
		GetEdgeVertexHandle(edV2Hd[0],edV2Hd[1],edHd[i]);
		for(j=0; j<2; j++)
		{
			if(YsIsIncluded <YsShell2dVertexHandle> (nTrV2Hd,trV2Hd,edV2Hd[j])!=YSTRUE)
			{
				trV2Hd[nTrV2Hd++]=edV2Hd[j];
			}
		}
	}

	if(nTrV2Hd==3)
	{
		YSSIDE sd;
		YsVec2 cen,trV2Pos[3];
		trV2Pos[0]=GetProjectedVertexPosition(trV2Hd[0]);
		trV2Pos[1]=GetProjectedVertexPosition(trV2Hd[1]);
		trV2Pos[2]=GetProjectedVertexPosition(trV2Hd[2]);

		cen=(trV2Pos[0]+trV2Pos[1]+trV2Pos[2])/3.0;
		if(ltc!=NULL)
		{
			sd=ltc->CheckInsidePolygon(cen);
		}
		else
		{
			sd=CheckInsidePolygon(cen);
		}

		if(sd!=YSINSIDE)
		{
			return YSFALSE;
		}
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsShell2d::CheckEdgeNewVertexTriangleIsRemovable
    (YsShell2dEdgeHandle edId,const YsVec3 &newVtx,const YsShell2dLattice *ltc) const
{
	int i;
	YsShell2dVertexHandle edVtId1,edVtId2;
	YsVec2 edVt1,edVt2,newVtxProjected;
	YsVec3 tmp;

	if(GetEdgeVertexHandle(edVtId1,edVtId2,edId)==YSOK)
	{
		ComputeProjection();

		edVt1=GetProjectedVertexPosition(edVtId1);
		edVt2=GetProjectedVertexPosition(edVtId2);

		proj.MulInverse(tmp,newVtx,1.0);
		newVtxProjected.Set(tmp.x(),tmp.y());

		YsArray <YsVec2> is;
		if(ShootFiniteRay(is,edVt1,newVtxProjected,ltc)==YSOK)
		{
			for(i=0; i<is.GetN(); i++)
			{
				if(is[i]!=edVt1 && is[i]!=newVtxProjected)
				{
					return YSFALSE;
				}
			}

			if(ShootFiniteRay(is,edVt2,newVtxProjected,ltc)==YSOK)
			{
				for(i=0; i<is.GetN(); i++)
				{
					if(is[i]!=edVt2 && is[i]!=newVtxProjected)
					{
						return YSFALSE;
					}
				}

				YSSIDE sd;
				if(ltc!=NULL)
				{
					sd=ltc->CheckInsidePolygon(newVtx);
				}
				else
				{
					sd=CheckInsidePolygon(newVtx);
				}

				if(sd==YSINSIDE)
				{
					return YSTRUE;
				}
			}
		}
	}
	return YSFALSE;
}

YSBOOL YsShell2d::CheckSelfIntersection(const YsShell2dLattice *ltc) const
{
	YsShell2dEdgeHandle edHd,edHd2;
	YsArray <YsShell2dEdgeHandle> edHdList;
	edHd=NULL;
	while((edHd=FindNextEdge(edHd))!=NULL)
	{
		YsShell2dVertexHandle vtHd[2];
		YsVec2 edPos[2],rgn[2];
		GetEdgeVertexHandle(vtHd[0],vtHd[1],edHd);
		GetProjectedVertexPosition(edPos[0],vtHd[0]);
		GetProjectedVertexPosition(edPos[1],vtHd[1]);
		if(ltc!=NULL)
		{
			rgn[0].Set(YsSmaller(edPos[0].x(),edPos[1].x()),YsSmaller(edPos[0].y(),edPos[1].y()));
			rgn[1].Set(YsGreater(edPos[0].x(),edPos[1].x()),YsGreater(edPos[0].y(),edPos[1].y()));
			if(ltc->FindEdgeListByRegion(edHdList,rgn[0],rgn[1])==YSOK)
			{
				int i;
				for(i=0; i<edHdList.GetN(); i++)
				{
					edHd2=edHdList[i];
					if(CheckEdgeIntersection(edHd,edHd2)==YSTRUE)
					{
						return YSTRUE;
					}
				}
			}
		}
		else
		{
			edHd2=NULL;
			while((edHd2=FindNextEdge(edHd2))!=NULL)
			{
				if(CheckEdgeIntersection(edHd,edHd2)==YSTRUE)
				{
					return YSTRUE;
				}
			}
		}
	}
	return YSFALSE;
}

YSBOOL YsShell2d::CheckEdgeIntersection(YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2) const
{
	YsShell2dVertexHandle vtHd[2][2];
	GetEdgeVertexHandle(vtHd[0][0],vtHd[0][1],edHd1);
	GetEdgeVertexHandle(vtHd[1][0],vtHd[1][1],edHd2);
	if(vtHd[0][0]==vtHd[1][0] || vtHd[0][0]==vtHd[1][1] ||
	   vtHd[0][1]==vtHd[1][0] || vtHd[0][1]==vtHd[1][1])
	{
		return YSFALSE;  // Sharing a vertex
	}

	YsVec2 p[2][2];
	GetProjectedVertexPosition(p[0][0],vtHd[0][0]);
	GetProjectedVertexPosition(p[0][1],vtHd[0][1]);
	GetProjectedVertexPosition(p[1][0],vtHd[1][0]);
	GetProjectedVertexPosition(p[1][1],vtHd[1][1]);
	if(p[0][0]==p[1][0] || p[0][0]==p[1][1] ||
	   p[0][1]==p[1][0] || p[0][1]==p[1][1])
	{
		return YSTRUE;  // Not sharing a vertex, but two vertices are at the same location -> overlaping vertex
	}

	YsVec2 crs;
	if(YsGetLineIntersection2(crs,p[0][0],p[0][1],p[1][0],p[1][1])==YSOK)
	{
		if(YsCheckInBetween2(crs,p[0][0],p[0][1])==YSTRUE &&
		   YsCheckInBetween2(crs,p[1][0],p[1][1])==YSTRUE)
		{
			return YSTRUE;  // Ordinary intersection
		}
		if((crs==p[0][0] || crs==p[0][1]) && YsCheckInBetween2(crs,p[1][0],p[1][1])==YSTRUE)
		{
			return YSTRUE;  // Touching
		}
		if(YsCheckInBetween2(crs,p[0][0],p[0][1])==YSTRUE && (crs==p[1][0] || crs==p[1][1]))
		{
			return YSTRUE;  // Touching
		}
	}
	else
	{
		if(YsCheckInBetween2(p[0][0],p[1][0],p[1][1])==YSTRUE &&
		   YsGetPointLineDistance2(p[1][0],p[1][1],p[0][0])<YsTolerance)
		{
			return YSTRUE;
		}
		if(YsCheckInBetween2(p[0][1],p[1][0],p[1][1])==YSTRUE &&
		   YsGetPointLineDistance2(p[1][0],p[1][1],p[0][1])<YsTolerance)
		{
			return YSTRUE;
		}
		if(YsCheckInBetween2(p[1][0],p[0][0],p[0][1])==YSTRUE &&
		   YsGetPointLineDistance2(p[0][0],p[0][1],p[1][0])<YsTolerance)
		{
			return YSTRUE;
		}
		if(YsCheckInBetween2(p[1][1],p[0][0],p[0][1])==YSTRUE &&
		   YsGetPointLineDistance2(p[0][0],p[0][1],p[1][1])<YsTolerance)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSRESULT YsShell2d::RemoveEdgeVertexTriangle
    (YsShell2dEdgeHandle edHd,YsShell2dVertexHandle vtId,YSBOOL iKnowItIsValid,YsShell2dLattice *ltc)
{
	if(iKnowItIsValid==YSTRUE || CheckEdgeVertexTriangleIsRemovable(edHd,vtId,ltc)==YSTRUE)
	{
		YsShell2dVertexHandle edVtId1,edVtId2;
		YsShell2dEdgeHandle newEdHd1,newEdHd2;

		if(GetEdgeVertexHandle(edVtId1,edVtId2,edHd)==YSOK)
		{
			newEdHd1=AddEdgeH(edVtId1,vtId);
			newEdHd2=AddEdgeH(edVtId2,vtId);

			if(newEdHd1!=NULL && newEdHd2!=NULL)
			{
				if(GetEdgeVirtualFlag(edHd)!=YSTRUE)
				{
					if(ltc!=NULL)
					{
						ltc->DeleteEdge(edHd,YSOUTSIDE);
						ltc->AddEdge(newEdHd1);
						ltc->AddEdge(newEdHd2);

						FixLatticeAfterRemovingTriangle(*ltc,edVtId1,edVtId2,vtId);
					}

					if(DeleteEdge(edHd)==YSOK)
					{
						return YSOK;
					}
				}
				else
				{
					if(ltc!=NULL)
					{
						ltc->DeleteEdge(edHd,YSOUTSIDE);
						ltc->AddEdge(newEdHd1);
						ltc->AddEdge(newEdHd2);
					}
					SetEdgeVirtualFlag(edHd,YSFALSE);
					if(ltc!=NULL)
					{
						ltc->AddEdge(edHd);
						FixLatticeAfterRemovingTriangle(*ltc,edVtId1,edVtId2,vtId);
					}
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell2d::RemoveEdgeEdgeTriangle
    (YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2,YSBOOL iKnowItIsValid,YsShell2dLattice *ltc)
{
	if(iKnowItIsValid==YSTRUE || CheckEdgeEdgeTriangleIsRemovable(edHd1,edHd2,ltc)==YSTRUE)
	{
		YsShell2dVertexHandle edVtId[3];
		YsShell2dEdgeHandle newEdHd;
		int i;

		if(CheckNeighborEdge(edVtId,edHd1,edHd2)==YSTRUE)
		{
			newEdHd=AddEdgeH(edVtId[0],edVtId[2]);

			if(newEdHd!=NULL)
			{
				if(GetEdgeVirtualFlag(edHd1)!=YSTRUE && GetEdgeVirtualFlag(edHd2)!=YSTRUE)
				{
					if(ltc!=NULL)
					{
						ltc->DeleteEdge(edHd1,YSOUTSIDE);
						ltc->DeleteEdge(edHd2,YSOUTSIDE);
						ltc->AddEdge(newEdHd);

						FixLatticeAfterRemovingTriangle(*ltc,edVtId[0],edVtId[1],edVtId[2]);
					}

					if(DeleteEdge(edHd1)==YSOK && DeleteEdge(edHd2)==YSOK)
					{
						return YSOK;
					}
				}
				else
				{
					if(ltc!=NULL)
					{
						ltc->DeleteEdge(edHd1,YSOUTSIDE);
						ltc->DeleteEdge(edHd2,YSOUTSIDE);
						ltc->AddEdge(newEdHd);
					}

					YsShell2dEdgeHandle edHd[2];
					edHd[0]=edHd1;
					edHd[1]=edHd2;

					for(i=0; i<2; i++)
					{
						if(GetEdgeVirtualFlag(edHd[i])==YSTRUE)
						{
							SetEdgeVirtualFlag(edHd[i],YSFALSE);
							if(ltc!=NULL)
							{
								ltc->AddEdge(edHd[i]);
							}
						}
						else
						{
							DeleteEdge(edHd[i]);
						}
					}

					if(ltc!=NULL)
					{
						FixLatticeAfterRemovingTriangle(*ltc,edVtId[0],edVtId[1],edVtId[2]);
					}

					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShell2d::RemoveThreeEdgeTriangle
    (YsShell2dEdgeHandle edHd1,YsShell2dEdgeHandle edHd2,YsShell2dEdgeHandle edHd3,
     YSBOOL iKnowItIsValid,YsShell2dLattice *ltc)
{
	if(iKnowItIsValid==YSTRUE || CheckThreeEdgeTriangleIsRemovable(edHd1,edHd2,edHd3,ltc)==YSTRUE)
	{
		int nTrV2Hd;
		YsShell2dVertexHandle trV2Hd[6],edV2Hd[2];
		YsShell2dEdgeHandle edHd[3];

		edHd[0]=edHd1;
		edHd[1]=edHd2;
		edHd[2]=edHd3;

		int i,j;
		nTrV2Hd=0;
		for(i=0; i<3; i++)
		{
			GetEdgeVertexHandle(edV2Hd[0],edV2Hd[1],edHd[i]);
			for(j=0; j<2; j++)
			{
				if(YsIsIncluded <YsShell2dVertexHandle> (nTrV2Hd,trV2Hd,edV2Hd[j])!=YSTRUE)
				{
					trV2Hd[nTrV2Hd++]=edV2Hd[j];
				}
			}
		}

		if(nTrV2Hd==3)
		{
			if(GetEdgeVirtualFlag(edHd1)!=YSTRUE &&
			   GetEdgeVirtualFlag(edHd2)!=YSTRUE &&
			   GetEdgeVirtualFlag(edHd3)!=YSTRUE)
			{
				if(ltc!=NULL)
				{
					ltc->DeleteEdge(edHd1,YSOUTSIDE);
					ltc->DeleteEdge(edHd2,YSOUTSIDE);
					ltc->DeleteEdge(edHd3,YSOUTSIDE);
					FixLatticeAfterRemovingTriangle(*ltc,trV2Hd[0],trV2Hd[1],trV2Hd[2]);
				}
				DeleteEdge(edHd1);
				DeleteEdge(edHd2);
				DeleteEdge(edHd3);
			}
			else
			{
				YsShell2dEdgeHandle edHd[3];
				edHd[0]=edHd1;
				edHd[1]=edHd2;
				edHd[2]=edHd3;

				if(ltc!=NULL)
				{
					ltc->DeleteEdge(edHd1,YSOUTSIDE);
					ltc->DeleteEdge(edHd2,YSOUTSIDE);
					ltc->DeleteEdge(edHd3,YSOUTSIDE);
				}

				for(i=0; i<3; i++)
				{
					if(GetEdgeVirtualFlag(edHd[i])==YSTRUE)
					{
						SetEdgeVirtualFlag(edHd[i],YSFALSE);
						if(ltc!=NULL)
						{
							ltc->AddEdge(edHd[i]);
						}
					}
					else
					{
						DeleteEdge(edHd[i]);
					}
				}

				if(ltc!=NULL)
				{
					FixLatticeAfterRemovingTriangle(*ltc,trV2Hd[0],trV2Hd[1],trV2Hd[2]);
				}
			}
			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShell2d::RemoveEdgeNewVertexTriangle
    (YsShell2dEdgeHandle edHd,const YsVec3 &newVtx,YSBOOL iKnowItIsValid,YsShell2dLattice *ltc)
{
	if(iKnowItIsValid==YSTRUE || CheckEdgeNewVertexTriangleIsRemovable(edHd,newVtx)==YSTRUE)
	{
		YsShell2dVertexHandle newVtId,edVtId1,edVtId2;
		YsShell2dEdgeHandle newEdHd1,newEdHd2;

		if(GetEdgeVertexHandle(edVtId1,edVtId2,edHd)==YSOK)
		{
			newVtId=AddVertexH(newVtx);
			newEdHd1=AddEdgeH(newVtId,edVtId1);
			newEdHd2=AddEdgeH(newVtId,edVtId2);

			if(newEdHd1!=NULL && newEdHd2!=NULL)
			{
				if(GetEdgeVirtualFlag(edHd)!=YSTRUE)
				{
					if(ltc!=NULL)
					{
						ltc->DeleteEdge(edHd,YSOUTSIDE);
						ltc->AddEdge(newEdHd1);
						ltc->AddEdge(newEdHd2);

						FixLatticeAfterRemovingTriangle(*ltc,edVtId1,edVtId2,newVtId);
					}

					if(newVtId!=NULL && DeleteEdge(edHd)==YSOK)
					{
						return YSOK;
					}
				}
				else
				{
					if(ltc!=NULL)
					{
						ltc->DeleteEdge(edHd,YSOUTSIDE);
						ltc->AddEdge(newEdHd1);
						ltc->AddEdge(newEdHd2);
					}

					SetEdgeVirtualFlag(edHd,YSFALSE);

					if(ltc!=NULL)
					{
						ltc->AddEdge(edHd);
						FixLatticeAfterRemovingTriangle(*ltc,edVtId1,edVtId2,newVtId);
					}

				}
			}
		}
	}
	return YSERR;
}

YsShell2dSpecialAttribute *YsShell2d::GetSpecialAttributeVertex(YsShell2dVertexHandle vtHd)
{
	YsListItem <YsShell2dVertex> *vtx;
	vtx=GetVertexPointerFromHandle(vtHd);
	if(vtx!=NULL)
	{
		return vtx->dat.special;
	}
	else
	{
		return NULL;
	}
}

const YsShell2dSpecialAttribute *YsShell2d::GetSpecialAttributeVertex(YsShell2dVertexHandle vtHd) const
{
	YsListItem <YsShell2dVertex> *vtx;
	vtx=GetVertexPointerFromHandle(vtHd);
	if(vtx!=NULL)
	{
		return vtx->dat.special;
	}
	else
	{
		return NULL;
	}
}

YsShell2dSpecialAttribute *YsShell2d::GetSpecialAttributeEdge(YsShell2dEdgeHandle edHd)
{
	YsListItem <YsShell2dEdge> *edg;
	edg=GetEdgePointerFromHandle(edHd);
	if(edg!=NULL)
	{
		return edg->dat.special;
	}
	else
	{
		return NULL;
	}
}

const YsShell2dSpecialAttribute *YsShell2d::GetSpecialAttributeEdge(YsShell2dEdgeHandle edHd) const
{
	YsListItem <YsShell2dEdge> *edg;
	edg=GetEdgePointerFromHandle(edHd);
	if(edg!=NULL)
	{
		return edg->dat.special;
	}
	else
	{
		return NULL;
	}
}

YSRESULT YsShell2d::SaveSrf(const char fn[])
{
	FILE *fp;
	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		Encache();

		YsShell2dVertexHandle vtHd,edVtHd1,edVtHd2;
		YsShell2dEdgeHandle edHd;

		fprintf(fp,"SURF\n");

		vtHd=NULL;
		while((vtHd=FindNextVertex(vtHd))!=NULL)
		{
			fprintf(fp,"V %lf %lf %lf\n",
			    GetVertexPosition(vtHd).x(),
			    GetVertexPosition(vtHd).y(),
			    GetVertexPosition(vtHd).z());
		}

		edHd=NULL;
		while((edHd=FindNextEdge(edHd))!=NULL)
		{
			GetEdgeVertexHandle(edVtHd1,edVtHd2,edHd);

			fprintf(fp,"F\n");
			fprintf(fp,"V %d %d\n",
				GetVtIdFromHandle(edVtHd1),
				GetVtIdFromHandle(edVtHd2));
			fprintf(fp,"E\n");
		}
		fprintf(fp,"E\n");
		fclose(fp);

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell2d::SaveS2d(const char fn[])
{
	FILE *fp;
	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		YsShell2dVertexHandle vtHd2d;
		Encache();
		vtHd2d=NULL;
		while((vtHd2d=FindNextVertex(vtHd2d))!=NULL)
		{
			YsVec3 p;
			p=GetVertexPosition(vtHd2d);
			fprintf(fp,"V %lf %lf %lf\n",p.x(),p.y(),p.z());
		}

		YsShell2dEdgeHandle edHd2d;
		YsShell2dVertexHandle edVtHd2d[2];
		edHd2d=NULL;
		while((edHd2d=FindNextEdge(edHd2d))!=NULL)
		{
			GetEdgeVertexHandle(edVtHd2d[0],edVtHd2d[1],edHd2d);
			fprintf(fp,"E %d %d",
			    GetVtIdFromHandle(edVtHd2d[0]),
			    GetVtIdFromHandle(edVtHd2d[1]));

			if(GetEdgeVirtualFlag(edHd2d)==YSTRUE)
			{
				fprintf(fp," V");
			}

			fprintf(fp,"\n");
		}
		fclose(fp);
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell2d::LoadS2d(const char fn[])
{
	FILE *fp;
	char buf[256];
	int ac;
	char *av[16];
	YsArray <YsShell2dVertexHandle> vtHdList;
	int ln;

	ln=0;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		CleanUp();

		while((fgets(buf,256,fp))!=NULL)
		{
			if(YsArguments(&ac,av,16,buf)==YSOK)
			{
				if((av[0][0]=='v' || av[0][0]=='V') && ac==4)
				{
					YsVec3 v;
					v.Set(atof(av[1]),atof(av[2]),atof(av[3]));
					vtHdList.Append(AddVertexH(v));
				}
				else if((av[0][0]=='e' || av[0][0]=='E') && ac>=3)
				{
					int i;
					YsShell2dEdgeHandle edHd;
					YsShell2dVertexHandle vtHd[2];

					vtHd[0]=vtHdList[atoi(av[1])];
					vtHd[1]=vtHdList[atoi(av[2])];
					edHd=AddEdgeH(vtHd[0],vtHd[1]);
					for(i=3; i<ac; i++)
					{
						if(av[i][0]=='V' || av[i][0]=='v')
						{
							SetEdgeVirtualFlag(edHd,YSTRUE);
						}
					}
				}
			}
		}

		fclose(fp);

		Encache();
		AutoComputeBasePlane();
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::AttachSpecialAttributeToNewVertex(YsShell2dVertex &vtx,YsShell2dVertexHandle vtHd)
{
	return YSOK;
}

YSRESULT YsShell2d::AttachSpecialAttributeToNewEdge(YsShell2dEdge &plg,YsShell2dEdgeHandle edHd)
{
	return YSOK;
}

YSRESULT YsShell2d::DetachSpecialAttributeBeforeDeletingVertex(YsShell2dVertex &vtx)
{
	return YSOK;
}

YSRESULT YsShell2d::DetachSpecialAttributeBeforeDeletingEdge(YsShell2dEdge &plg)
{
	return YSOK;
}

int YsShell2d::GetVtIdFromHandle(YsShell2dVertexHandle vtx) const
{
	return vtxList.GetIdFromItem(vtx);
}

YsShell2dVertexHandle YsShell2d::GetVertexHandleFromId(int vtId) const
{
	return vtxList.GetItemFromId(vtId);
}

void YsShell2d::RewindVtxPtr(void) const
{
	vtxList.RewindPointer();
}

YsShell2dVertexHandle YsShell2d::StepVtxPtr(void) const
{
	return vtxList.StepPointer();
}

YsShell2dVertexHandle YsShell2d::FindNextVertex(YsShell2dVertexHandle currentVtx) const
{
	return vtxList.FindNext(currentVtx);
}

const YsVec3 &YsShell2d::GetVertexPosition(YsShell2dVertexHandle vtx) const
{
	return vtx->dat.p;
}

int YsShell2d::GetVertexReferenceCount(YsShell2dVertexHandle vtx) const
{
	return vtx->dat.refCount;
}

int YsShell2d::GetVertexAttrib1(YsShell2dVertexHandle vtHd) const
{
	return vtHd->dat.att1;
}

int YsShell2d::GetVertexAttrib2(YsShell2dVertexHandle vtHd) const
{
	return vtHd->dat.att2;
}

int YsShell2d::GetVertexAttrib3(YsShell2dVertexHandle vtHd) const
{
	return vtHd->dat.att3;
}

int YsShell2d::GetVertexAttrib4(YsShell2dVertexHandle vtHd) const
{
	return vtHd->dat.att4;
}

YsShellVertexHandle YsShell2d::GetVtHdMap(YsShell2dVertexHandle v2Hd) const
{
	return v2Hd->dat.vtHdMap;
}

unsigned YsShell2d::GetSearchKey(YsShell2dVertexHandle vtHd) const
{
	return vtHd->dat.searchKey;
}

YSRESULT YsShell2d::SetVertexAttrib1(YsShell2dVertexHandle vtHd,int att) const
{
	if(vtHd!=NULL)
	{
		GetVertexPointerFromHandle(vtHd)->dat.att1=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetVertexAttrib2(YsShell2dVertexHandle vtHd,int att) const
{
	if(vtHd!=NULL)
	{
		GetVertexPointerFromHandle(vtHd)->dat.att2=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetVertexAttrib3(YsShell2dVertexHandle vtHd,int att) const
{
	if(vtHd!=NULL)
	{
		GetVertexPointerFromHandle(vtHd)->dat.att3=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetVertexAttrib4(YsShell2dVertexHandle vtHd,int att) const
{
	if(vtHd!=NULL)
	{
		GetVertexPointerFromHandle(vtHd)->dat.att4=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetVtHdMap(YsShell2dVertexHandle v2Hd,YsShellVertexHandle vtHd) const
{
	if(v2Hd!=NULL)
	{
		GetVertexPointerFromHandle(v2Hd)->dat.vtHdMap=vtHd;
		return YSOK;
	}
	return YSERR;
}

int YsShell2d::GetEdgeAttrib1(YsShell2dEdgeHandle edHd) const
{
	return edHd->dat.att1;
}

int YsShell2d::GetEdgeAttrib2(YsShell2dEdgeHandle edHd) const
{
	return edHd->dat.att2;
}

int YsShell2d::GetEdgeAttrib3(YsShell2dEdgeHandle edHd) const
{
	return edHd->dat.att3;
}

int YsShell2d::GetEdgeAttrib4(YsShell2dEdgeHandle edHd) const
{
	return edHd->dat.att4;
}

void *YsShell2d::GetEdgeAttribPointer(YsShell2dEdgeHandle edHd) const
{
	return edHd->dat.attp;
}

YSRESULT YsShell2d::SetEdgeAttrib1(YsShell2dEdgeHandle edHd,int att) const
{
	if(edHd!=NULL)
	{
		GetEdgePointerFromHandle(edHd)->dat.att1=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetEdgeAttrib2(YsShell2dEdgeHandle edHd,int att) const
{
	if(edHd!=NULL)
	{
		GetEdgePointerFromHandle(edHd)->dat.att2=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetEdgeAttrib3(YsShell2dEdgeHandle edHd,int att) const
{
	if(edHd!=NULL)
	{
		GetEdgePointerFromHandle(edHd)->dat.att3=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetEdgeAttrib4(YsShell2dEdgeHandle edHd,int att) const
{
	if(edHd!=NULL)
	{
		GetEdgePointerFromHandle(edHd)->dat.att4=att;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2d::SetEdgeAttribPointer(YsShell2dEdgeHandle edHd,void *ptr) const
{
	if(edHd!=NULL)
	{
		GetEdgePointerFromHandle(edHd)->dat.attp=ptr;
		return YSOK;
	}
	return YSERR;
}

int YsShell2d::GetEdIdFromHandle(YsShell2dEdgeHandle edg) const
{
	return edgList.GetIdFromItem(edg);
}

YsShell2dEdgeHandle YsShell2d::GetEdgeHandleFromId(int edId) const
{
	return edgList.GetItemFromId(edId);
}

YSRESULT YsShell2d::SetEdgeFlag(YsShell2dEdgeHandle edHd,unsigned flag)
{
	YsListItem <YsShell2dEdge> *edge;
	edge=GetEdgePointerFromHandle(edHd);
	if(edge!=NULL)
	{
		edge->dat.flags=flag;
		return YSOK;
	}
	return YSERR;
}

unsigned int YsShell2d::GetEdgeFlag(YsShell2dEdgeHandle edHd) const
{
	const YsListItem <YsShell2dEdge> *edge;
	edge=GetEdgePointerFromHandle(edHd);
	if(edge!=NULL)
	{
		return edge->dat.flags;
	}
	return 0;
}

YSRESULT YsShell2d::SetEdgeVirtualFlag(YsShell2dEdgeHandle edHd,YSBOOL flag)
{
	YsListItem <YsShell2dEdge> *edge;
	edge=GetEdgePointerFromHandle(edHd);
	if(edge!=NULL)
	{
		if(flag==YSTRUE)
		{
			edge->dat.flags|=YSS2DEDGE_VIRTUAL;
		}
		else
		{
			edge->dat.flags&=~YSS2DEDGE_VIRTUAL;
		}
		return YSOK;
	}
	return YSERR;
}

YSBOOL YsShell2d::GetEdgeVirtualFlag(YsShell2dEdgeHandle edHd) const
{
	const YsListItem <YsShell2dEdge> *edge;
	edge=GetEdgePointerFromHandle(edHd);
	if(edge!=NULL)
	{
		if(edge->dat.flags&YSS2DEDGE_VIRTUAL)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

void YsShell2d::RewindEdgePtr(void) const
{
	edgList.RewindPointer();
}

YsShell2dEdgeHandle YsShell2d::StepEdgePtr(void) const
{
	return edgList.StepPointer();
}

YsShell2dEdgeHandle YsShell2d::FindNextEdge(YsShell2dEdgeHandle currentEdg) const
{
	return edgList.FindNext(currentEdg);
}

YsShell2dEdgeHandle YsShell2d::FindPrevEdge(YsShell2dEdgeHandle currentEdg) const
{
	return edgList.FindPrev(currentEdg);
}

YSRESULT YsShell2d::GetEdgeVertexHandle
    (YsShell2dVertexHandle &vtx1,YsShell2dVertexHandle &vtx2,YsShell2dEdgeHandle edHd) const
{
	if(edHd!=NULL)
	{
		vtx1=edHd->dat.vtId1;
		vtx2=edHd->dat.vtId2;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}



////////////////////////////////////////////////////////////

YsShell2dVtxToEdgTable::YsShell2dVtxToEdgTable() : hashTable(HashSize)
{
}

YsShell2dVtxToEdgTable::~YsShell2dVtxToEdgTable()
{
}


YSRESULT YsShell2dVtxToEdgTable::Prepare(void)
{
	hashTable.PrepareTable();
	return YSOK;
}

YSRESULT YsShell2dVtxToEdgTable::AddEdge(const YsShell2d &shl,YsShell2dEdgeHandle edHd)
{
	int i;
	YsShell2dVertexHandle vtId[2];
	unsigned key[1];
	if(shl.GetEdgeVertexHandle(vtId[0],vtId[1],edHd)==YSOK)
	{
		for(i=0; i<2; i++)
		{
			key[0]=shl.GetSearchKey(vtId[i]);
			hashTable.AddElement(1,key,edHd);

//			YsShell2dVtxToEdgTableEntry *entry;
//			entry=FindEntry(vtId[i],YSTRUE);
//
//			if(entry!=NULL)
//			{
//				entry->edgList.AppendItem(edg);
//			}
//			else
//			{
//				YsErrOut("YsShell2dVtxToEdgTable::AddEdge()\n");
//				YsErrOut("  Error2\n");
//				return YSERR;
//			}
		}
		return YSOK;
	}
	YsErrOut("YsShell2dVtxToEdgTable::AddEdge()\n");
	YsErrOut("  Error1\n");
	return YSERR;
}

YSRESULT YsShell2dVtxToEdgTable::DeleteEdge(const YsShell2d &shl,YsShell2dEdgeHandle edHd)
{
	int i;
	YsShell2dVertexHandle vtId[2];
	unsigned key[1];
	if(shl.GetEdgeVertexHandle(vtId[0],vtId[1],edHd)==YSOK)
	{
		for(i=0; i<2; i++)
		{
			key[0]=shl.GetSearchKey(vtId[i]);
			if(hashTable.DeleteElement(1,key,edHd)!=YSOK)
			{
				YsErrOut("YsShell2dVtxToEdgTable::DeleteEdge()\n");
				YsErrOut("  Tried to delete an unregistered edge.\n");
			}

//			YsShell2dVtxToEdgTableEntry *entry;
//			YSBOOL found;
//			entry=FindEntry(vtId[i]);
//
//			if(entry!=NULL)
//			{
//				found=YSFALSE;
//				for(j=entry->edgList.GetNumItem()-1; j>=0; j--)
//				{
//					if(entry->edgList[j]==edg)
//					{
//						entry->edgList.Delete(j);
//						if(found!=YSTRUE)
//						{
//							found=YSTRUE;
//						}
//						else
//						{
//							YsErrOut("YsShell2dVtxToEdgTable::DeleteEdge()\n");
//							YsErrOut("  Doubly registered edge is found.\n");
//							YsErrOut("  %d %d\n",shl.GetSearchKey(vtId[0]),shl.GetSearchKey(vtId[1]));
//						}
//					}
//				}
//
//				if(found!=YSTRUE)
//				{
//					YsErrOut("YsShell2dVtxToEdgTable::DeleteEdge()\n");
//					YsErrOut("  Error3\n");
//				}
//
//				if(entry->edgList.GetNumItem()==0)
//				{
//					DeleteEntry(entry,vtId[i]);
//				}
//			}
//			else
//			{
//				YsErrOut("YsShell2dVtxToEdgTable::DeleteEdge()\n");
//				YsErrOut("  Error2\n");
//				return YSERR;
//			}
		}
		return YSOK;
	}
	YsErrOut("YsShell2dVtxToEdgTable::DeleteEdge()\n");
	YsErrOut("  Error1\n");
	return YSERR;
}

const YsArray <YsShell2dEdgeHandle> *YsShell2dVtxToEdgTable::GetVtxToEdgTable(YsShell2dVertexHandle v2Hd) const
{
	unsigned key[1];
	const YsArray <YsShell2dEdgeHandle> *lst;
	key[0]=v2Hd->dat.searchKey;
	lst=hashTable.FindElement(1,key);
	if(lst!=NULL && lst->GetN()>0)
	{
		return lst;
	}
	return NULL;

//	YsShell2dVtxToEdgTableEntry *entry;
//	entry=FindEntry(vtx);
//	if(entry!=NULL && entry->edgList.GetNumItem()>0)
//	{
//		return &entry->edgList;
//	}
//	return NULL;
}


//YsShell2dVtxToEdgTableEntry *YsShell2dVtxToEdgTable::FindEntry(YsShell2dVertexHandle vtx,YSBOOL autoCreate)
//{
//	YsShell2dVtxToEdgTableEntry *entry;
//
//	entry=FindEntry(vtx);
//	if(entry!=NULL)
//	{
//		return entry;
//	}
//	else
//	{
//		if(autoCreate==YSTRUE)
//		{
//			unsigned serial,hashId;
//			serial=vtx->dat.searchKey;
//			hashId=serial%HashSize;
//
//			YsList <YsShell2dVtxToEdgTableEntry> *neo;
//			neo=new YsList <YsShell2dVtxToEdgTableEntry>;
//			if(neo!=NULL)
//			{
//				neo->dat.vtx=vtx;
//				neo->dat.edgList.Set(0,NULL);
//				neo->dat.thisInTheList=neo;
//				hashTable[hashId]=hashTable[hashId]->Append(neo);
//				return &neo->dat;
//			}
//			else
//			{
//				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell2dVtxToEdgeTable::FindEntry()");
//			}
//		}
//	}
//	return NULL;
//}
//
//YsShell2dVtxToEdgTableEntry *YsShell2dVtxToEdgTable::FindEntry(YsShell2dVertexHandle vtx) const
//{
//	unsigned serial,hashId;
//	serial=vtx->dat.searchKey;
//	hashId=serial%HashSize;
//
//	YsList <YsShell2dVtxToEdgTableEntry> *seeker;
//	for(seeker=hashTable[hashId]; seeker!=NULL; seeker=seeker->Next())
//	{
//		if(seeker->dat.vtx==vtx)
//		{
//			return &seeker->dat;
//		}
//	}
//
//	return NULL;
//}
//
//YSRESULT YsShell2dVtxToEdgTable::DeleteEntry(YsShell2dVtxToEdgTableEntry *entry,YsShell2dVertexHandle vtx)
//{
//	unsigned serial,hashId;
//	serial=vtx->dat.searchKey;
//	hashId=serial%HashSize;
//
//	if(entry->thisInTheList->SeekTop()==hashTable[hashId])
//	{
//		hashTable[hashId]=entry->thisInTheList->DeleteFromList();
//		return YSOK;
//	}
//	YsErrOut("YsShell2dVtxToEdgTable::DeleteEntry()\n");
//	YsErrOut("  Error1\n");
//	return YSERR;
//}


////////////////////////////////////////////////////////////


YsShell2dEdgeFinder::YsShell2dEdgeFinder() : hashTable(HashSize)
{
}

YsShell2dEdgeFinder::~YsShell2dEdgeFinder()
{
}

YSRESULT YsShell2dEdgeFinder::Prepare(void)
{
	hashTable.PrepareTable();
	return YSOK;
}

YSRESULT YsShell2dEdgeFinder::AddEdge(const YsShell2d &shl,YsShell2dEdgeHandle edHd)
{
	YsShell2dVertexHandle vtx1,vtx2;
	if(shl.GetEdgeVertexHandle(vtx1,vtx2,edHd)==YSOK)
	{
		unsigned key[2];
		key[0]=shl.GetSearchKey(vtx1);
		key[1]=shl.GetSearchKey(vtx2);
		hashTable.AddElement(2,key,edHd);
		return YSOK;

		//YsShell2dEdgeFinderEntry *entry;
		//entry=FindEntry(vtx1,vtx2,YSTRUE);
		//if(entry!=NULL)
		//{
		//	entry->edgList.AppendItem(edHd);
		//	return YSOK;
		//}
	}
	return YSERR;
}

YSRESULT YsShell2dEdgeFinder::DeleteEdge(const YsShell2d &shl,YsShell2dEdgeHandle edHd)
{
	YsShell2dVertexHandle vtx1,vtx2;
	if(shl.GetEdgeVertexHandle(vtx1,vtx2,edHd)==YSOK)
	{
		unsigned key[2];
		key[0]=shl.GetSearchKey(vtx1);
		key[1]=shl.GetSearchKey(vtx2);
		if(hashTable.DeleteElement(2,key,edHd)==YSOK)
		{
			return YSOK;
		}
		else
		{
			YsErrOut("YsShell2dEdgeFinder::DeleteEdge()\n");
			YsErrOut("  Tried to delete an unregistered edge.\n");
		}

//		YsShell2dEdgeFinderEntry *entry;
//		YSBOOL found;
//		entry=FindEntry(vtx1,vtx2);
//		found=YSFALSE;
//		if(entry!=NULL)
//		{
//			int i;
//			for(i=entry->edgList.GetNumItem()-1; i>=0; i--)
//			{
//				if(entry->edgList[i]==edg)
//				{
//					entry->edgList.Delete(i);
//					if(found!=YSTRUE)
//					{
//						found=YSTRUE;
//					}
//					else
//					{
//						YsErrOut("YsShell2dEdgeFinder::DeleteEdge()\n");
//						YsErrOut("  Doubly registered edge is found.\n");
//						YsErrOut("  %d %d\n",shl.GetSearchKey(vtx1),shl.GetSearchKey(vtx2));
//					}
//				}
//			}
//
//			if(found!=YSTRUE)
//			{
//				YsErrOut("YsShell2dEdgeFinder::DeleteEdge()\n");
//				YsErrOut("  Edge not found.\n");
//			}
//
//			if(entry->edgList.GetNumItem()==0)
//			{
//				DeleteEntry(entry,vtx1,vtx2);
//			}
//			return YSOK;
//		}
	}
	return YSERR;
}

const YsArray <YsShell2dEdgeHandle> *YsShell2dEdgeFinder::FindEdge
    (YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2) const
{
	const YsArray <YsShell2dEdgeHandle> *lst;
	unsigned key[2];
	key[0]=vtx1->dat.searchKey;
	key[1]=vtx2->dat.searchKey;
	lst=hashTable.FindElement(2,key);
	if(lst!=NULL && lst->GetN()>0)
	{
		return lst;
	}
	
//	YsShell2dEdgeFinderEntry *entry;
//	entry=FindEntry(vtx1,vtx2);
//	if(entry!=NULL && entry->edgList.GetNumItem()>0)
//	{
//		return &entry->edgList;
//	}
	return NULL;
}


//YsShell2dEdgeFinderEntry *YsShell2dEdgeFinder::FindEntry
//    (YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2,YSBOOL autoCreate)
//{
//	YsShell2dEdgeFinderEntry *entry;
//	entry=FindEntry(vtx1,vtx2);
//	if(entry!=NULL)
//	{
//		return entry;
//	}
//	else
//	{
//		if(autoCreate==YSTRUE)
//		{
//			unsigned sumSerial,hashId;
//			sumSerial=vtx1->dat.searchKey+vtx2->dat.searchKey;
//			hashId=sumSerial%HashSize;
//
//			YsList <YsShell2dEdgeFinderEntry> *neo;
//			neo=new YsList <YsShell2dEdgeFinderEntry>;
//			if(neo!=NULL)
//			{
//				neo->dat.vtId1=vtx1;
//				neo->dat.vtId2=vtx2;
//				neo->dat.thisInTheList=neo;
//
//				hashTable[hashId]=hashTable[hashId]->Append(neo);
//				return &neo->dat;
//			}
//			else
//			{
//				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsShell2dEdgeFinder::FindEntry()");
//				return NULL;
//			}
//		}
//		else
//		{
//			return NULL;
//		}
//	}
//}
//
//YsShell2dEdgeFinderEntry *YsShell2dEdgeFinder::FindEntry
//    (YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2) const
//{
//	unsigned sumSerial,hashId;
//	sumSerial=vtx1->dat.searchKey+vtx2->dat.searchKey;
//	hashId=sumSerial%HashSize;
//
//	YsList <YsShell2dEdgeFinderEntry> *seeker;
//	for(seeker=hashTable[hashId]; seeker!=NULL; seeker=seeker->Next())
//	{
//		if((seeker->dat.vtId1==vtx1 && seeker->dat.vtId2==vtx2) ||
//		   (seeker->dat.vtId1==vtx2 && seeker->dat.vtId2==vtx1))
//		{
//			return &seeker->dat;
//		}
//	}
//	return NULL;
//}
//
//YSRESULT YsShell2dEdgeFinder::DeleteEntry
//    (YsShell2dEdgeFinderEntry *entry,YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2)
//{
//	unsigned sumSerial,hashId;
//	sumSerial=vtx1->dat.searchKey+vtx2->dat.searchKey;
//	hashId=sumSerial%HashSize;
//
//	if(entry->thisInTheList->SeekTop()==hashTable[hashId])
//	{
//		hashTable[hashId]=entry->thisInTheList->DeleteFromList();
//		return YSOK;
//	}
//	return YSERR;
//}


////////////////////////////////////////////////////////////


YSRESULT YsShell2dSearchTable::Prepare(void)
{
	vtxToEdg.Prepare();
	edgFind.Prepare();
	return YSOK;
}

YSRESULT YsShell2dSearchTable::AddEdge(const YsShell2d &shl,YsShell2dEdgeHandle edHd)
{
	edgeKeyToEdHd.AddElement(shl.GetSearchKey(edHd),edHd);
	if(vtxToEdg.AddEdge(shl,edHd)==YSOK &&
	   edgFind.AddEdge(shl,edHd)==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell2dSearchTable::DeleteEdge(const YsShell2d &shl,YsShell2dEdgeHandle edHd)
{
	edgeKeyToEdHd.DeleteElement(shl.GetSearchKey(edHd),edHd);
	if(vtxToEdg.DeleteEdge(shl,edHd)==YSOK &&
	   edgFind.DeleteEdge(shl,edHd)==YSOK)
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSRESULT YsShell2dSearchTable::AddVertex(const YsShell2d &shl,YsShell2dVertexHandle vtx)
{
	// Currently nothing to do
	return YSOK;
}

YSRESULT YsShell2dSearchTable::DeleteVertex(const YsShell2d &shl,YsShell2dVertexHandle vtx)
{
	// Currently nothing to do
	return YSOK;
}

const YsArray <YsShell2dEdgeHandle> *YsShell2dSearchTable::GetVtxToEdgTable(YsShell2dVertexHandle vtx) const
{
	return vtxToEdg.GetVtxToEdgTable(vtx);
}

const YsArray <YsShell2dEdgeHandle> *YsShell2dSearchTable::FindEdge(YsShell2dVertexHandle vtx1,YsShell2dVertexHandle vtx2) const
{
	return edgFind.FindEdge(vtx1,vtx2);
}

YsShell2dEdgeHandle YsShell2dSearchTable::FindEdge(unsigned int key) const
{
	YsShell2dEdgeHandle edHd;
	if(edgeKeyToEdHd.FindElement(edHd,key)==YSOK)
	{
		return edHd;
	}
	return NULL;
}



////////////////////////////////////////////////////////////



void YsShell2dTessTriangle::Initialize(void)
{
}






YsShell2dTessellator::YsShell2dTessellator() : triAlloc(16),triList(triAlloc)
{
	search2d=new YsShell2dSearchTable;
	shl2d.AttachSearchTable(search2d);
}

YsShell2dTessellator::~YsShell2dTessellator()
{
	shl2d.DetachSearchTable();
	delete search2d;
}

YSRESULT YsShell2dTessellator::SetDomain(int nVt,const YsVec3 vtPos[])
{
	if(nVt>=3)
	{
		int i;
		YsArray <YsShell2dVertexHandle> vtHdList;

		shl2d.CleanUp();

		vtHdList.Set(nVt+1,NULL);

		for(i=0; i<nVt; i++)
		{
			vtHdList[i]=shl2d.AddVertexH(vtPos[i]);
		}
		vtHdList[nVt]=vtHdList[0];

		for(i=0; i<nVt; i++)
		{
			shl2d.AddEdgeH(vtHdList[i],vtHdList[i+1]);
		}

		shl2d.AutoComputeBasePlane();

		ltc2d.SetDomain(shl2d,nVt);

		triList.CleanUp();

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2dTessellator::SetDomain(int nVt,const YsVec2 vtPos[])
{
	if(nVt>=3)
	{
		int i;
		YsArray <YsShell2dVertexHandle> vtHdList;

		shl2d.CleanUp();

		vtHdList.Set(nVt+1,NULL);

		for(i=0; i<nVt; i++)
		{
			vtHdList[i]=shl2d.AddVertexH(vtPos[i]);
		}
		vtHdList[nVt]=vtHdList[0];

		for(i=0; i<nVt; i++)
		{
			shl2d.AddEdgeH(vtHdList[i],vtHdList[i+1]);
		}

		shl2d.SetNoTransformation();

		ltc2d.SetDomain(shl2d,nVt);

		triList.CleanUp();

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell2dTessellator::RemoveEdge(YsShell2dEdgeHandle edHd,int searchRange)
{
	YsShell2dVertexHandle edVtHd[2];
	YsVec2 edVtPos[2];
	YsArray <YsShell2dEdgeHandle> edHdList;
	int i,j;

	shl2d.GetEdgeVertexHandle(edVtHd[0],edVtHd[1],edHd);
	shl2d.GetProjectedVertexPosition(edVtPos[0],edVtHd[0]);
	shl2d.GetProjectedVertexPosition(edVtPos[1],edVtHd[1]);


	// Intersection Check >>
	if(ltc2d.FindEdgeListByRegion(edHdList,edVtPos[0],edVtPos[1])==YSOK)
	{
		forYsArray(i,edHdList)
		{
			YsShell2dVertexHandle edVtHd2[2];
			YsVec2 edVtPos2[2];

			shl2d.GetEdgeVertexHandle(edVtHd2[0],edVtHd2[1],edHdList[i]);
			/* if(edHdList[i]!=edHd && YsSameEdge(edVtHd[0],edVtHd[1],edVtHd2[0],edVtHd2[1])==YSTRUE) // Two duplicate edges
			{
				shl2d.DeleteEdge(edHd);
				shl2d.DeleteEdge(edHdList[i]);
				return YSOK;
			} */

			if(edVtHd2[0]!=edVtHd[0] && edVtHd2[1]!=edVtHd[0] && edVtHd2[0]!=edVtHd[1] && edVtHd2[1]!=edVtHd[1])
			{
				shl2d.GetProjectedVertexPosition(edVtPos2[0],edVtHd2[0]);
				shl2d.GetProjectedVertexPosition(edVtPos2[1],edVtHd2[1]);

				if(YsGetLinePenetration2(edVtPos[0],edVtPos[1],edVtPos2[0],edVtPos2[1])!=YSAPART)
				{
					return YSERR;
				}
			}
		}
	}
	// Intersection Check <<



	int dx,dy,vx,vy,nStep,ox,oy;
	dx=0;
	dy=0;
	vx=1;
	vy=0;
	nStep=2;


	if(ltc2d.GetBlockIndex(ox,oy,(edVtPos[0]+edVtPos[1])/2.0)==YSOK)
	{
		while(-searchRange<dx && dx<searchRange)
		{
			for(i=0; i<nStep/2; i++)
			{
				const YsShell2dLatticeElem *blk;
				blk=ltc2d.GetBlock(ox+dx,oy+dy);
				if(blk!=NULL)
				{
					forYsArray(j,blk->vtxList)
					{
						YsShell2dEdgeHandle triEdHd[3];
						int nEdge;
						const YsArray <YsShell2dEdgeHandle> *edHdList;

						nEdge=1;
						triEdHd[0]=edHd;
						edHdList=search2d->FindEdge(edVtHd[0],blk->vtxList[j]);
						if(edHdList!=NULL && edHdList->GetN()>0)
						{
							triEdHd[nEdge++]=(*edHdList)[0];
						}
						edHdList=search2d->FindEdge(edVtHd[1],blk->vtxList[j]);
						if(edHdList!=NULL && edHdList->GetN()>0)
						{
							triEdHd[nEdge++]=(*edHdList)[0];
						}

						if((nEdge==1 && shl2d.RemoveEdgeVertexTriangle(edHd,blk->vtxList[j],YSFALSE,&ltc2d)==YSOK) || 
						   (nEdge==2 && shl2d.RemoveEdgeEdgeTriangle(triEdHd[0],triEdHd[1],YSFALSE,&ltc2d)==YSOK) ||
						   (nEdge==3 && shl2d.RemoveThreeEdgeTriangle(triEdHd[0],triEdHd[1],triEdHd[2],YSFALSE,&ltc2d)==YSOK))
						{
							YsListItem <YsShell2dTessTriangle> *newTri;
							newTri=triList.Create();
							newTri->dat.Initialize();
							newTri->dat.trVtHd[0]=edVtHd[0];
							newTri->dat.trVtHd[1]=edVtHd[1];
							newTri->dat.trVtHd[2]=blk->vtxList[j];
							return YSOK;
						}
					}
				}

				dx+=vx;
				dy+=vy;
			}

			int a;
			a=vx;
			vx=-vy;
			vy=a;

			nStep++;
		}
	}

	return YSERR;
}

////////////////////////////////////////////////////////////

