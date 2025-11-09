#ifndef YSDEF_IS_INCLUDED
#define YSDEF_IS_INCLUDED
/* { */

#ifndef YSRESULT_IS_DEFINED
#define YSRESULT_IS_DEFINED
typedef enum
{
	YSERR,
	YSOK
} YSRESULT;

#endif

#ifndef YSBOOL_IS_DEFINED
#define YSBOOL_IS_DEFINED
typedef enum
{
	YSFALSE,
	YSTRUE,
	YSTFUNKNOWN
} YSBOOL;
#endif

typedef enum
{
	YSLEFT_ZPLUS_YPLUS,
	YSRIGHT_ZMINUS_YPLUS
} YSCOORDSYSMODEL;

typedef enum
{
	YSINSIDE,
	YSOUTSIDE,
	YSBOUNDARY,
	YSUNKNOWNSIDE
} YSSIDE;

typedef enum
{
	YSINTERSECT,
	YSOVERLAP,
	YSTOUCH,
	YSSHAREPOINT,
	YSAPART
} YSINTERSECTION;

typedef enum
{
	YSBOOLBLEND,
	YSBOOLAND,
	YSBOOLOR,
	YSBOOLMINUS
} YSBOOLEANOPERATION;

typedef enum
{
	YSCONVEXNIZEDEFAULT,
	YSCONVEXNIZENOSMALLANGLE,
	YSCONVEXNIZE90DEGREE,
	YSCONVEXNIZEFASTEST,
	YSCONVEXNIZEBALANCEDIST
} YSCONVEXNIZESTRATEGY;

typedef enum
{
	YSFLIPCCW,
	YSFLIPCW,
	YSFLIPUNKNOWN
} YSFLIPDIRECTION;

////////////////////////////////////////////////////////////

inline void YsFlip(YSBOOL &boo)
{
	boo=(boo==YSTRUE ? YSFALSE : YSTRUE);
}

template <class T>
inline void YsSwapSomething(T &a,T &b)
{
	T c;
	c=a;
	a=b;
	b=c;
}

inline void YsSwapDouble(double &a,double &b)
{
	YsSwapSomething <double> (a,b);
}

inline void YsSwapInt(int &a,int &b)
{
	YsSwapSomething <int> (a,b);
}

inline void YsSwapUnsignedInt(unsigned &a,unsigned &b)
{
	YsSwapSomething <unsigned> (a,b);
}

inline void YsSwapBool(YSBOOL &a,YSBOOL &b)
{
	YsSwapSomething <YSBOOL> (a,b);
}
/* } */
#endif
