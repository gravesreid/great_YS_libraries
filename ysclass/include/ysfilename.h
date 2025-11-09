#ifndef YSFILENAME_IS_INCLUDED
#define YSFILENAME_IS_INCLUDED
/* { */



enum YSFTYPE
{
	YSFTYPE_UNKNOWN,
	YSFTYPE_SRF,
	YSFTYPE_SRM,
	YSFTYPE_DNM,
	YSFTYPE_FLD,
	YSFTYPE_STP,
	YSFTYPE_YFS,
	YSFTYPE_MSH,
	YSFTYPE_MS3,
	YSFTYPE_STL,
	YSFTYPE_BMP,
	YSFTYPE_PNG,
	YSFTYPE_S2D,
	YSFTYPE_PAR,
	YSFTYPE_NAS,
	YSFTYPE_ABAQUSINP,
	YSFTYPE_STARCDINP,
	YSFTYPE_HGC,
	YSFTYPE_HBS,
	YSFTYPE_SHL,
	YSFTYPE_TSHL
};



YSFTYPE YsIdentifyFileType(const char fn[]);
void YsPutExt(char *fname,const char *ext);
unsigned long YsFileSize(const char *fname);
YSBOOL YsFileExist(const char *fn);
YSRESULT YsSeparatePathFile(char *pth,char *fil,const char *org);
void YsDeleteExtension(char def[]);
void YsReplaceExtension(char fn[],const char ext[]);



class YsBufferedFileReader
{
protected:
	FILE *fp;
	unsigned bufSize,nBufFilled,nBufUsed;
	unsigned char *buf;
	unsigned filePtrHWord,filePtrLWord;
public:
	YsBufferedFileReader(unsigned bufSize=4*1024*1024);
	~YsBufferedFileReader();
	YSRESULT SetBufferSize(unsigned bufSize);
	YSRESULT Open(const char fn[]);
	unsigned Read(void *buf,unsigned readSize);
	YSRESULT Close(void);

	unsigned GetFilePtrKB(void) const;
};

class YsBufferedFileWriter
{
protected:
	FILE *fp;
	unsigned bufSize,nBufFilled;
	unsigned char *buf;
public:
	YsBufferedFileWriter(unsigned bufSize=4*1024*1024);
	~YsBufferedFileWriter();
	YSRESULT SetBufferSize(unsigned bufSize);
	YSRESULT Open(const char fn[]);
	YSRESULT Write(void *buf,unsigned writeSize);
	YSRESULT Close(void);
};

/* } */
#endif
