#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "ysclass.h"




static float YsGetFloat(const unsigned char dat[])
{
	int a;
	char *intelOrMotorola;
	a=0x7f;
	intelOrMotorola=(char *)(&a);
	if(intelOrMotorola[0]==0x7f)
	{
		// For Intel chip
		return *((float *)dat);
	}
	else
	{
		// For Motorola chip
		unsigned char conv[4];
		conv[0]=dat[3];
		conv[1]=dat[2];
		conv[2]=dat[1];
		conv[3]=dat[0];
		return *((float *)conv);
	}
}

YSRESULT YsShell::LoadStl(const char fn[])
{
	CleanUp();
	return MergeStl(fn);
}

YSRESULT YsShell::MergeStl(const char fn[])
{
	FILE *fp;
	char buf[1024];
	int i;
	Ys3dTree <YsShellVertexHandle> vtKdTree;

	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		fread(buf,1,1024,fp);
		fclose(fp);

		for(i=0; i<1024; i++)
		{
			if('A'<=buf[i] && buf[i]<='Z')
			{
				buf[i]=buf[i]+('a'-'A');
			}
		}

		// Is this a binary or ascii?
		YSBOOL solid,facet,loop,vertex;
		solid=YSFALSE;
		facet=YSFALSE;
		loop=YSFALSE;
		vertex=YSFALSE;
		for(i=0; i<1018; i++)
		{
			if(strncmp(buf+i,"solid",5)==0)
			{
				solid=YSTRUE;
			}
			else if(strncmp(buf+i,"facet",5)==0)
			{
				facet=YSTRUE;
			}
			else if(strncmp(buf+i,"loop",4)==0)
			{
				loop=YSTRUE;
			}
			else if(strncmp(buf+i,"vertex",6)==0)
			{
				vertex=YSTRUE;
			}
		}

		if(solid==YSTRUE && facet==YSTRUE && loop==YSTRUE && vertex==YSTRUE)  // It's an ascii STL
		{
			int ac;
			char *av[256];
			int state;   // 0:Outside     1:insie "outer"->"endloop"
			YsVec3 dv,nom;
			YsArray <YsVec3> plVtPos;
			YsArray <YsShellVertexHandle> plVtHd,findVtHd;
			YsLoopCounter ctr;

			YsPrintf("It looks like an ASCII stl.\n");

			fp=fopen(fn,"r");

			int lng,nxt,cur;
			fseek(fp,0,SEEK_END);
			lng=ftell(fp);
			fseek(fp,0,SEEK_SET);

			state=0;
			dv.Set(YsTolerance,YsTolerance,YsTolerance);

			ctr.BeginCounter(lng);

			nxt=0;
			nom=YsOrigin();
			while(fgets(buf,256,fp)!=NULL)
			{
				cur=ftell(fp);
				ctr.ShowCounter(cur);

				YsUncapitalize(buf);
				if(YsArguments(&ac,av,256,buf)==YSOK)
				{
					if(state==0)
					{
						if(ac>=5 && strcmp(av[0],"facet")==0 && strcmp(av[1],"normal")==0)
						{
							double x,y,z;
							x=atof(av[2]);
							y=atof(av[3]);
							z=atof(av[4]);
							nom.Set(x,y,z);
						}
						else if(ac>=2 && strcmp(av[0],"outer")==0 && strcmp(av[1],"loop")==0)
						{
							state=1;
							plVtPos.Set(0,NULL);
						}
					}
					else if(state==1)
					{
						if(ac>=4 && strcmp(av[0],"vertex")==0)
						{
							YsVec3 v;
							v.Set(atof(av[1]),atof(av[2]),atof(av[3]));
							plVtPos.Append(v);
						}
						else if(ac>=1 && strcmp(av[0],"endloop")==0)
						{
							if(plVtPos.GetN()!=3)
							{
								YsPrintf("Warning! Non-triangular polygon exists.\n");
							}

							plVtHd.Set(0,NULL);
							int i;
							for(i=0; i<plVtPos.GetN(); i++)
							{
								if(vtKdTree.MakeItemList(findVtHd,plVtPos[i]-dv,plVtPos[i]+dv)==YSOK &&
								   findVtHd.GetN()>0)
								{
									plVtHd.Append(findVtHd[0]);
								}
								else
								{
									YsShellVertexHandle newVtHd;
									newVtHd=AddVertexH(plVtPos[i]);
									plVtHd.Append(newVtHd);
									vtKdTree.AddPoint(plVtPos[i],newVtHd);
								}
							}

							YsShellPolygonHandle plHd;
							plHd=AddPolygonH(plVtHd.GetN(),plVtHd);
							SetColorOfPolygon(plHd,YsWhite());
							SetNormalOfPolygon(plHd,nom);

							plVtPos.Set(0,NULL);
							plVtHd.Set(0,NULL);
							nom=YsOrigin();
							state=0;
						}
					}
				}
			}

			fclose(fp);
		}
		else if(facet!=YSTRUE && loop!=YSTRUE && vertex!=YSTRUE)  // It's a binary STL
		{
			unsigned char buf[256];
			unsigned char conv[4];
			YsVec3 nom,tri[3];
			YsVec3 dv;
			YsShellVertexHandle plVtHd[3];
			int nxt,cur,nPlg;
			YsLoopCounter ctr;
			YsArray <YsShellVertexHandle> findVtHd;

			nPlg=0;
			dv.Set(YsTolerance,YsTolerance,YsTolerance);

			YsPrintf("It looks like a binary stl.\n");

			fp=fopen(fn,"rb");
			fread(buf,1,80,fp);
			fread(conv,4,1,fp);

			// nPlg=conv[3]+conv[2]*0x100+conv[1]*0x10000+conv[0]*0x1000000;
			nPlg=conv[0]+conv[1]*0x100+conv[2]*0x10000+conv[3]*0x1000000;
			YsPrintf("%d Polygons\n",nPlg);

			ctr.BeginCounter(nPlg);

			cur=0;
			nxt=0;
			while(fread(buf,1,50,fp)==50)
			{
				ctr.ShowCounter(cur);
				cur++;

				nom.Set   (YsGetFloat(buf   ),YsGetFloat(buf+ 4),YsGetFloat(buf+ 8));
				tri[0].Set(YsGetFloat(buf+12),YsGetFloat(buf+16),YsGetFloat(buf+20));
				tri[1].Set(YsGetFloat(buf+24),YsGetFloat(buf+28),YsGetFloat(buf+32));
				tri[2].Set(YsGetFloat(buf+36),YsGetFloat(buf+40),YsGetFloat(buf+44));

				for(i=0; i<3; i++)
				{
					if(vtKdTree.MakeItemList(findVtHd,tri[i]-dv,tri[i]+dv)==YSOK &&
					   findVtHd.GetN()>0)
					{
						plVtHd[i]=findVtHd[0];
					}
					else
					{
						YsShellVertexHandle newVtHd;
						newVtHd=AddVertexH(tri[i]);
						plVtHd[i]=newVtHd;
						vtKdTree.AddPoint(tri[i],newVtHd);
					}
				}

				YsShellPolygonHandle plHd;
				plHd=AddPolygonH(3,plVtHd);
				SetColorOfPolygon(plHd,YsWhite());
				SetNormalOfPolygon(plHd,nom);
			}
			ctr.EndCounter();

			fclose(fp);
		}
		else
		{
			YsPrintf("Cannot identify the STL file (binary or ascii).\n");
			return YSERR;
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SaveStl(int &nIgnored,const char fn[],const char title[]) const
{
	FILE *fp;
	fp=fopen(fn,"w");
	nIgnored=0;
	if(fp!=NULL)
	{
		if(title==NULL)
		{
			fprintf(fp,"solid \"%s\" created by YSCLASS\n",fn);
		}
		else
		{
			fprintf(fp,"solid \"%s\"\n",title);
		}

		YsShellPolygonHandle plHd;
		YsArray <YsVec3> plVtPos;
		YsVec3 nom;
		int i;

		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			if(GetNumVertexOfPolygon(plHd)==3)
			{
				GetVertexListOfPolygon(plVtPos,plHd);
				GetNormalOfPolygon(nom,plHd);
				if(nom.GetSquareLength()<=YsSqr(YsTolerance))
				{
					nom=(plVtPos[1]-plVtPos[0])^(plVtPos[2]-plVtPos[0]);
					nom.Normalize();
				}

				fprintf(fp,"facet normal %lf %lf %lf\n",nom.x(),nom.y(),nom.z());
				fprintf(fp,"outer loop\n");
				for(i=0; i<3; i++)
				{
					fprintf(fp,"vertex %lf %lf %lf\n",plVtPos[i].x(),plVtPos[i].y(),plVtPos[i].z());
				}
				fprintf(fp,"endloop\n");
				fprintf(fp,"endfacet\n");
			}
			else
			{
				nIgnored++;
			}
		}
		fprintf(fp,"end solid\n");
		fclose(fp);
		if(nIgnored>0)
		{
			YsPrintf("%d polygons were not written in STL because they are not triangular.\n",nIgnored);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell::SaveStl(const char fn[],const char title[]) const
{
	int n;
	return SaveStl(n,fn,title);
}



////////////////////////////////////////////////////////////



#define LITTLEENDIAN    0   // eg. Intel CPU
#define BIGENDIAN       1   // eg. Motorola CPU

static int ByteOrder(void)
{
	int a=1;
	unsigned char *tst;
	tst=(unsigned char *)&a;
	if(tst[0]==1)
	{
		return LITTLEENDIAN;
	}
	else
	{
		return BIGENDIAN;
	}
}

static void WriteBinStlTitle(FILE *fp,const char title[])
{
	int i;
	char buf[256];
	sprintf(buf,"binstl%s",title);
	for(i=strlen(buf); i<80; i++)
	{
		buf[i]=' ';
	}
	fwrite(buf,1,80,fp);
}

static void WriteBinStlUnsignedInt(FILE *fp,unsigned int dat)
{
	unsigned char buf[4];
	buf[0]=(unsigned char)( dat     &0xff);
	buf[1]=(unsigned char)((dat>> 8)&0xff);
	buf[2]=(unsigned char)((dat>>16)&0xff);
	buf[3]=(unsigned char)((dat>>24)&0xff);
	fwrite(buf,1,4,fp);
}

static void WriteBinStlFloat(FILE *fp,float dat,int byteOrder)
{
	// This function assumes a float takes four bytes.
	unsigned char buf[4],*ptr;
	ptr=(unsigned char *)(&dat);
	if(byteOrder==LITTLEENDIAN)
	{
		fwrite(ptr,1,4,fp);
	}
	else
	{
		buf[0]=ptr[3];
		buf[1]=ptr[2];
		buf[2]=ptr[1];
		buf[3]=ptr[0];
		fwrite(buf,1,4,fp);
	}
}

static void WriteBinStlTriangle
    (FILE *fp,const double nom[],const double v1[],const double v2[], const double v3[])
{
	int byteOrder;
	byteOrder=ByteOrder();

	WriteBinStlFloat(fp,(float)nom[0],byteOrder);
	WriteBinStlFloat(fp,(float)nom[1],byteOrder);
	WriteBinStlFloat(fp,(float)nom[2],byteOrder);

	WriteBinStlFloat(fp,(float)v1[0],byteOrder);
	WriteBinStlFloat(fp,(float)v1[1],byteOrder);
	WriteBinStlFloat(fp,(float)v1[2],byteOrder);
	WriteBinStlFloat(fp,(float)v2[0],byteOrder);
	WriteBinStlFloat(fp,(float)v2[1],byteOrder);
	WriteBinStlFloat(fp,(float)v2[2],byteOrder);
	WriteBinStlFloat(fp,(float)v3[0],byteOrder);
	WriteBinStlFloat(fp,(float)v3[1],byteOrder);
	WriteBinStlFloat(fp,(float)v3[2],byteOrder);

	char padding[2]={0,0};
	fwrite(padding,1,2,fp);
}



YSRESULT YsShell::SaveBinStl(int &nIgnored,const char fn[],const char title[]) const
{
	FILE *fp;
	fp=fopen(fn,"wb");
	nIgnored=0;
	if(fp!=NULL)
	{
		YsShellPolygonHandle plHd;
		YsArray <YsVec3> plVtPos;
		YsVec3 nom;
		unsigned nTri;


		nTri=0;
		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			if(GetNumVertexOfPolygon(plHd)==3)
			{
				nTri++;
			}
		}

		if(title==NULL)
		{
			WriteBinStlTitle(fp,"STL Created by YSCLASS");
		}
		else
		{
			WriteBinStlTitle(fp,title);
		}
		WriteBinStlUnsignedInt(fp,nTri);


		nIgnored=0;
		plHd=NULL;
		while((plHd=FindNextPolygon(plHd))!=NULL)
		{
			if(GetNumVertexOfPolygon(plHd)==3)
			{
				GetVertexListOfPolygon(plVtPos,plHd);
				GetNormalOfPolygon(nom,plHd);
				if(nom.GetSquareLength()<=YsSqr(YsTolerance))
				{
					nom=(plVtPos[1]-plVtPos[0])^(plVtPos[2]-plVtPos[0]);
					nom.Normalize();
				}

				WriteBinStlTriangle(fp,nom,plVtPos[0],plVtPos[1],plVtPos[2]);
			}
			else
			{
				nIgnored++;
			}
		}
		fclose(fp);
		if(nIgnored>0)
		{
			YsPrintf("%d polygons were not written in STL because they are not triangular.\n",nIgnored);
		}
		return YSOK;
	}
	else
	{
		YsPrintf("YsShell::SaveBinStl()\n");
		YsPrintf("  Cannot open output file.\n");
		return YSERR;
	}
}



YSRESULT YsShell::SaveBinStl(const char fn[],const char title[]) const
{
	int n;
	return SaveBinStl(n,fn,title);
}

