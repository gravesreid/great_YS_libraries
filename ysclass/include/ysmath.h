#ifndef YSMATH_IS_INCLUDED
#define YSMATH_IS_INCLUDED
/* { */

#define YsAbs(a) ((a)>=0 ? (a) : -(a))
#define YsEqual(a,b) (YsAbs((a)-(b))<=YsTolerance ? YSTRUE : YSFALSE)
#define YsZero(a) YsEqual((a),0.0)
#define YsGreater(a,b) ((a)>(b) ? (a) : (b))
#define YsSmaller(a,b) ((a)<(b) ? (a) : (b))
#define YsBound(x,a,b) ((x)<(a) ? (a) : ((x)>(b) ? (b) : (x)))


#define YsSmallestOf(a,b,c) (((a)<(b) && (a)<(c)) ? (a) : ((b)<(c) ? (b) : (c)))
#define YsGreatestOf(a,b,c) (((a)>(b) && (a)>(c)) ? (a) : ((b)>(c) ? (b) : (c)))
#define YsSameEdge(a,b,c,d) ((((a)==(c) && (b)==(d)) || ((a)==(d) && (b)==(c))) ? YSTRUE : YSFALSE)


#define YSBLUEIMPULSE YSLEFT_ZPLUS_YPLUS
#define YSOPENGL YSRIGHT_ZMINUS_YPLUS

const double YsPi=3.14159265358979323;
const double YsE=2.71828;



const double YsCos1deg=0.999847695156;
const double YsCos3deg=0.998629534755;
const double YsCos5deg=0.996194698092;
const double YsCos10deg=0.984807753012;
const double YsCos15deg=0.965925826289;
const double YsCos20deg=0.939692620786;
const double YsCos25deg=0.906307787037;
const double YsCos30deg=0.866025403784;
const double YsCos35deg=0.819152044289;
const double YsCos40deg=0.766044443119;
const double YsCos45deg=0.707106781187;
const double YsCos50deg=0.642787609687;
const double YsCos55deg=0.573576436351;
const double YsCos60deg=0.500000000000;
const double YsCos65deg=0.422618261741;
const double YsCos70deg=0.342020143326;
const double YsCos75deg=0.258819045103;
const double YsCos80deg=0.173648177667;
const double YsCos85deg=0.087155742748;
const double YsCos90deg=0.000000000000;
const double YsCos95deg=-0.087155742748;
const double YsCos100deg=-0.173648177667;
const double YsCos105deg=-0.258819045103;
const double YsCos110deg=-0.342020143326;
const double YsCos115deg=-0.422618261741;
const double YsCos120deg=-0.500000000000;
const double YsCos125deg=-0.573576436351;
const double YsCos130deg=-0.642787609687;
const double YsCos135deg=-0.707106781187;
const double YsCos140deg=-0.766044443119;
const double YsCos145deg=-0.819152044289;
const double YsCos150deg=-0.866025403784;
const double YsCos155deg=-0.906307787037;
const double YsCos160deg=-0.939692620786;
const double YsCos165deg=-0.965925826289;
const double YsCos170deg=-0.984807753012;
const double YsCos175deg=-0.996194698092;
const double YsCos177deg=-0.998629534755;
const double YsCos179deg=-0.999847695156;
const double YsSin1deg=0.017452406437;
const double YsSin3deg=0.052335956243;
const double YsSin5deg=0.087155742748;
const double YsSin10deg=0.173648177667;
const double YsSin15deg=0.258819045103;
const double YsSin20deg=0.342020143326;
const double YsSin25deg=0.422618261741;
const double YsSin30deg=0.500000000000;
const double YsSin35deg=0.573576436351;
const double YsSin40deg=0.642787609687;
const double YsSin45deg=0.707106781187;
const double YsSin50deg=0.766044443119;
const double YsSin55deg=0.819152044289;
const double YsSin60deg=0.866025403784;
const double YsSin65deg=0.906307787037;
const double YsSin70deg=0.939692620786;
const double YsSin75deg=0.965925826289;
const double YsSin80deg=0.984807753012;
const double YsSin85deg=0.996194698092;
const double YsSin90deg=1.000000000000;
const double YsSin95deg=0.996194698092;
const double YsSin100deg=0.984807753012;
const double YsSin105deg=0.965925826289;
const double YsSin110deg=0.939692620786;
const double YsSin115deg=0.906307787037;
const double YsSin120deg=0.866025403784;
const double YsSin125deg=0.819152044289;
const double YsSin130deg=0.766044443119;
const double YsSin135deg=0.707106781187;
const double YsSin140deg=0.642787609687;
const double YsSin145deg=0.573576436351;
const double YsSin150deg=0.500000000000;
const double YsSin155deg=0.422618261741;
const double YsSin160deg=0.342020143326;
const double YsSin165deg=0.258819045103;
const double YsSin170deg=0.173648177667;
const double YsSin175deg=0.087155742748;
const double YsSin177deg=0.052335956243;
const double YsSin179deg=0.017452406437;
const double YsTan1deg=0.017455064928;
const double YsTan3deg=0.052407779283;
const double YsTan5deg=0.087488663526;
const double YsTan10deg=0.176326980708;
const double YsTan15deg=0.267949192431;
const double YsTan20deg=0.363970234266;
const double YsTan25deg=0.466307658155;
const double YsTan30deg=0.577350269190;
const double YsTan35deg=0.700207538210;
const double YsTan40deg=0.839099631177;
const double YsTan45deg=1.000000000000;
const double YsTan50deg=1.191753592594;
const double YsTan55deg=1.428148006742;
const double YsTan60deg=1.732050807569;
const double YsTan65deg=2.144506920510;
const double YsTan70deg=2.747477419455;
const double YsTan75deg=3.732050807569;
const double YsTan80deg=5.671281819618;
const double YsTan85deg=11.430052302761;

const double YsMeterPerNm=1852.0;
const double YsMeterPerSm=1609.0;
const double YsKmPerNm=1.852;
const double YsKmPerSm=1.609;
const double YsMeterPerInch=0.0254;
const double YsMeterPerFt=0.3048;
const double YsCmPerInch=2.54;
const double YsCmPerFt=30.48;
const double YsKgPerLb=0.453597;
const double YsKgPerPound=0.453597;

const double YsInfinity=1.0E+32;

extern double YsTolerance;
extern double YsZero;
extern YSCOORDSYSMODEL YsCoordSysModel;

#define YsToleranceSqr (YsTolerance*YsTolerance)



//extern const double YsZero;
//extern const double YsOne;




////////////////////////////////////////////////////////////

inline double YsDegToRad(const double &deg)
{
	return deg*YsPi/180.0;
}

inline double YsRadToDeg(const double &rad)
{
	return rad*180.0/YsPi;
}

#define YsSqr(x) ((x)*(x))   // Outer () : credit to Ved
//inline double YsSqr(const double &x)
//{
//	return x*x;
//}

////////////////////////////////////////////////////////////

template <class T>
const T &YsGreatest(int n,const T v[])
{
	if(n>0 && v!=NULL)
	{
		int i,iRef;
		iRef=0;
		for(i=1; i<n; i++)
		{
			if(v[i]>v[iRef])
			{
				iRef=i;
			}
		}
		return v[iRef];
	}
	return v[0];
}

template <class T>
const T &YsSmallest(int n,const T v[])
{
	if(n>0 && v!=NULL)
	{
		int i,iRef;
		iRef=0;
		for(i=1; i<n; i++)
		{
			if(v[i]<v[iRef])
			{
				iRef=i;
			}
		}
		return v[iRef];
	}
	return v[0];
}

template <class T>
const T &YsMedian(const T &a,const T &b,const T &c)
{
	if((a<=b && b<=c) || (c<=b && b<=a))
	{
		return b;
	}
	else if((b<=a && a<=c) || (c<=a && a<=b))
	{
		return a;
	}
	else
	{
		return c;
	}
}


/* } */
#endif
