#ifndef YSPROPERTY_IS_INCLUDED
#define YSPROPERTY_IS_INCLUDED
/* { */

// Declaration /////////////////////////////////////////////
class YsColor
{
public:
	inline YsColor(){};
	YsColor(double r,double g,double b);

	inline double Rd(void) const
	{
		return double(col[0])/255.0;
	}
	inline double Gd(void) const
	{
		return double(col[1])/255.0;
	}
	inline double Bd(void) const
	{
		return double(col[2])/255.0;
	}
	inline int Ri(void) const
	{
		return int(col[0]);
	}
	inline int Gi(void) const
	{
		return int(col[1]);
	}
	inline int Bi(void) const
	{
		return int(col[2]);
	}

	void Set15BitRGB(int c15bit);
	void SetIntRGB(int r,int g,int b);
	void SetDoubleRGB(double r,double g,double b);
	void SetRainbowColor(const double &t);

	int Get15BitColor(void) const;
	void GetIntRGB(int &r,int &g,int &b) const;
	void GetDoubleRGB(double &r,double &g,double &b) const;

protected:
	unsigned char col[4];
};

inline int operator==(const YsColor &col1,const YsColor &col2)
{
	return (col1.Ri()==col2.Ri() && col1.Gi()==col2.Gi() && col1.Bi()==col2.Bi());
}

inline int operator!=(const YsColor &col1,const YsColor &col2)
{
	return (col1.Ri()!=col2.Ri() || col1.Gi()!=col2.Gi() || col1.Bi()!=col2.Bi());
}

/* } */
#endif
