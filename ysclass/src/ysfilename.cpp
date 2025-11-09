#include <stdio.h>
#include <string.h>

#include "ysdef.h"
#include "ysfilename.h"
#include "yslist.h"

#include "ysbase.h"
#include "ysclassmgr.h"

#include "ysmath.h"
#include "ysarray.h"
#include "yslist2.h"
#include "ysstring.h"

#include "ysexception.h"

YSFTYPE YsIdentifyFileType(const char fn[])
{
	int i;
	YsString ext;

	for(i=strlen(fn)-1; i>=0; i--)
	{
		if(fn[i]=='/' || fn[i]=='\\' || fn[i]==':')
		{
			return YSFTYPE_UNKNOWN;
		}
		else if(fn[i]=='.')
		{
			ext.Set(fn+i+1);
			break;
		}
	}

	ext.Capitalize();

	if(strcmp(ext,"SRF")==0)
	{
		return YSFTYPE_SRF;
	}
	else if(strcmp(ext,"SRM")==0)
	{
		return YSFTYPE_SRM;
	}
	else if(strcmp(ext,"DNM")==0)
	{
		return YSFTYPE_DNM;
	}
	else if(strcmp(ext,"FLD")==0)
	{
		return YSFTYPE_FLD;
	}
	else if(strcmp(ext,"STP")==0)
	{
		return YSFTYPE_STP;
	}
	else if(strcmp(ext,"YFS")==0)
	{
		return YSFTYPE_YFS;
	}
	else if(strcmp(ext,"MSH")==0)
	{
		return YSFTYPE_MSH;
	}
	else if(strcmp(ext,"MS3")==0)
	{
		return YSFTYPE_MS3;
	}
	else if(strcmp(ext,"STL")==0)
	{
		return YSFTYPE_STL;
	}
	else if(strcmp(ext,"BMP")==0)
	{
		return YSFTYPE_BMP;
	}
	else if(strcmp(ext,"PNG")==0)
	{
		return YSFTYPE_PNG;
	}
	else if(strcmp(ext,"S2D")==0)
	{
		return YSFTYPE_S2D;
	}
	else if(strcmp(ext,"PAR")==0)
	{
		return YSFTYPE_PAR;
	}
	else if(strcmp(ext,"NAS")==0)
	{
		return YSFTYPE_NAS;
	}
	else if(strcmp(ext,"HGC")==0)
	{
		return YSFTYPE_HGC;
	}
	else if(strcmp(ext,"HBS")==0)
	{
		return YSFTYPE_HBS;
	}
	else if(strcmp(ext,"TSHL")==0)
	{
		return YSFTYPE_TSHL;
	}
	else if(strcmp(ext,"SHL")==0)
	{
		return YSFTYPE_SHL;
	}

	return YSFTYPE_UNKNOWN;
}

void YsPutExt(char *fname,const char *ext)
{
	int i;

	for(i=0; fname[i]!=0; i++)
	{
		if(fname[i]=='.')
		{
			return;
		}
	}

	if(*ext == '*')
	{
		ext++;
	}
	if(*ext == '.')
	{
		ext++;
	}

	sprintf(fname,"%s.%s",fname,ext);
}

unsigned long YsFileSize(const char *fname)
{
	FILE *fp;
    unsigned long edptr;

	fp=fopen(fname,"rb");
	if(fp==NULL)
	{
		return 0;
	}

    fseek(fp,0,2 /* SEEK_END */);
    edptr=ftell(fp);

	fclose(fp);

    return edptr;
}

YSBOOL YsFileExist(const char *fn)
{
	FILE *fp;
	fp=fopen(fn,"rb");

	if(fp!=NULL)
	{
		fclose(fp);
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSRESULT YsSeparatePathFile(char *pth,char *fil,const char *org)
{
	char tmp[256],*seekPtr,*cutPtr;

	strcpy(tmp,org);
	cutPtr=tmp;

	/* Ysip Drive Name */
	if(tmp[1]==':')
	{
		cutPtr+=2;
	}

	/* scan until nul Stopper */
	for(seekPtr=cutPtr; *seekPtr!=0; seekPtr++)
	{
		if(*seekPtr=='\\' || *seekPtr=='/' || *seekPtr==':')
		{
			cutPtr=seekPtr+1;
		}
	}

	/* cutPtr points *tmp  or after ':' or after last '\\' */
	strcpy(fil,cutPtr);

	*cutPtr = 0;
	strcpy(pth,tmp);
	return YSOK;
}

void YsDeleteExtension(char def[])
{
	int i;
	for(i=strlen(def)-1; i>=0; i--)
	{
		if(def[i]=='/' || def[i]=='\\')
		{
			break;
		}
		else if(def[i]=='.')
		{
			def[i]=0;
			break;
		}
	}
}

void YsReplaceExtension(char fn[],const char ext[])  // ext can be "*.???",".???" or "???"
{
	int offset;
	offset=0;
	while(ext[offset]=='.' || ext[offset]=='*')
	{
		offset++;  // Skip first "*.", if it is given.
	}

	YsDeleteExtension(fn);

	if(fn[0]!=0)
	{
		strcat(fn,".");
		strcat(fn,ext+offset);
	}
}


////////////////////////////////////////////////////////////

YsBufferedFileReader::YsBufferedFileReader(unsigned size)
{
	fp=NULL;
	nBufFilled=0;
	nBufUsed=0;
	buf=new unsigned char [size];
	if(buf!=NULL)
	{
		bufSize=size;

		filePtrLWord=0;
		filePtrHWord=0;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileReader::YsBufferedFileReader()");
	}
}

YsBufferedFileReader::~YsBufferedFileReader()
{
	if(fp!=NULL)
	{
		fclose(fp);
	}
	if(buf!=NULL)
	{
		delete [] buf;
	}
}

YSRESULT YsBufferedFileReader::SetBufferSize(unsigned size)
{
	if(nBufFilled==0)
	{
		if(buf!=NULL)
		{
			delete [] buf;
		}
		buf=new unsigned char [size];
		if(buf!=NULL)
		{
			bufSize=size;
			return YSOK;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileReader::SetBufferSize()");
		}
	}
	return YSERR;
}

YSRESULT YsBufferedFileReader::Open(const char fn[])
{
	fp=fopen(fn,"rb");
	if(fp!=NULL)
	{
		nBufFilled=0;
		nBufUsed=0;

		filePtrHWord=0;
		filePtrLWord=0;

		return YSOK;
	}
	return YSERR;
}

unsigned YsBufferedFileReader::Read(void *readBufPtr,unsigned readSize)
{
	unsigned nReturned;
	unsigned char *readBuf;
	readBuf=(unsigned char *)readBufPtr;
	nReturned=0;
	while(nReturned<readSize)
	{
		if(nBufFilled<=nBufUsed)
		{
			nBufFilled=fread(buf,1,bufSize,fp);
			nBufUsed=0;
			if(nBufFilled==0)
			{
				return nReturned;
			}
		}

		while(nBufUsed<nBufFilled && nReturned<readSize)
		{
			readBuf[nReturned++]=buf[nBufUsed++];
			filePtrLWord++;
			if(filePtrLWord==0)
			{
				filePtrHWord++;
			}
		}
	}
	return nReturned;
}

YSRESULT YsBufferedFileReader::Close(void)
{
	if(fp!=NULL)
	{
		fclose(fp);
		nBufFilled=0;
		nBufUsed=0;
		return YSOK;
	}
	return YSERR;
}

unsigned YsBufferedFileReader::GetFilePtrKB(void) const
{
	return (filePtrLWord>>10)+(filePtrHWord<<22);

}

////////////////////////////////////////////////////////////

YsBufferedFileWriter::YsBufferedFileWriter(unsigned size)
{
	fp=NULL;
	bufSize=size;
	nBufFilled=0;
	buf=new unsigned char [size];
	if(buf==NULL)
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileWriter::YsBufferedFileWriter()");
	}
}

YsBufferedFileWriter::~YsBufferedFileWriter()
{
	if(fp!=NULL)
	{
		Close();
	}
	if(buf!=NULL)
	{
		delete [] buf;
	}
}

YSRESULT YsBufferedFileWriter::SetBufferSize(unsigned size)
{
	if(nBufFilled==0)
	{
		delete [] buf;
		buf=new unsigned char [size];
		if(buf!=NULL)
		{
			bufSize=size;
			return YSOK;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsBufferedFileWriter::SetBufferSize()");
		}
	}
	return YSERR;
}

YSRESULT YsBufferedFileWriter::Open(const char fn[])
{
	if(fp!=NULL)
	{
		Close();
	}

	fp=fopen(fn,"wb");
	if(fp!=NULL)
	{
		nBufFilled=0;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBufferedFileWriter::Write(void *datPtr,unsigned writeSize)
{
	unsigned i,actualWriteSize;
	unsigned char *dat;
	dat=(unsigned char *)datPtr;

	for(i=0; i<writeSize; i++)
	{
		buf[nBufFilled++]=dat[i];
		if(bufSize<=nBufFilled)
		{
			actualWriteSize=fwrite(buf,1,nBufFilled,fp);
			if(actualWriteSize!=nBufFilled)
			{
				return YSERR;
			}
			nBufFilled=0;
		}
	}
	return YSOK;
}

YSRESULT YsBufferedFileWriter::Close(void)
{
	if(fp!=NULL)
	{
		if(0<nBufFilled)
		{
			fwrite(buf,1,nBufFilled,fp);
		}
		fclose(fp);
		fp=NULL;
		nBufFilled=0;
		return YSOK;
	}
	return YSERR;
}

