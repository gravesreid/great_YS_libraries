#include "ysclass.h"

#include <stdlib.h>

YsVec2i::YsVec2i(const char *x,const char *y)
{
	dat[0]=atoi(x);
	dat[1]=atoi(y);
}

////////////////////////////////////////////////////////////

YsVec3i::YsVec3i(const char *x,const char *y,const char *z)
{
	dat[0]=atoi(x);
	dat[1]=atoi(y);
	dat[2]=atoi(z);
}

