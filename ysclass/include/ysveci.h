#ifndef YSVECI_IS_INCLUDED
#define YSVECI_IS_INCLUDED
/* { */



class YsVec2i : protected YsFixedLengthArray <int,2>
{
public:
	inline YsVec2i()
	{
	}
	inline YsVec2i(int x,int y)
	{
		dat[0]=x;
		dat[1]=y;
	}
	YsVec2i(const char *x,const char *y);

	inline const int &operator[](int idx) const
	{
		return dat[idx];
	}
	inline int &operator[](int idx)
	{
		return dat[idx];
	}
	operator const int *() const
	{
		return dat;
	}
	const char *Text(char buf[],const char *fmt="%d %d") const
	{
		sprintf(buf,fmt,x(),y());
		return buf;
	}

	inline void AddX(const int &x)
	{
		dat[0]+=x;
	}
	inline void AddY(const int &y)
	{
		dat[1]+=y;
	}

	inline void SubX(const int &x)
	{
		dat[0]-=x;
	}
	inline void SubY(const int &y)
	{
		dat[1]-=y;
	}

	inline void MulX(const int &x)
	{
		dat[0]*=x;
	}
	inline void MulY(const int &y)
	{
		dat[1]*=y;
	}

	inline void DivX(const int &x)
	{
		dat[0]/=x;
	}
	inline void DivY(const int &y)
	{
		dat[1]/=y;
	}

	inline int x(void) const
	{
		return dat[0];
	}
	inline int y(void) const
	{
		return dat[1];
	}

	inline void Set(int x,int y)
	{
		dat[0]=x;
		dat[1]=y;
	}

	inline void SetX(const int &x)
	{
		dat[0]=x;
	}
	inline void SetY(const int &y)
	{
		dat[1]=y;
	}

	void RotateLeft90(void)
	{
		int x,y;
		x=dat[0];
		y=dat[1];
		dat[0]=-y;
		dat[1]=x;
	}
	void RotateRight90(void)
	{
		int x,y;
		x=dat[0];
		y=dat[1];
		dat[0]=y;
		dat[1]=-x;
	}
};

inline const YsVec2i operator-(const YsVec2i &a)
{
	return YsVec2i(-a.x(),-a.y());
}

inline const YsVec2i operator-(const YsVec2i &a,const YsVec2i &b)
{
	return YsVec2i(a.x()-b.x(),a.y()-b.y());
}

inline const YsVec2i operator+(const YsVec2i &a,const YsVec2i &b)
{
	return YsVec2i(a.x()+b.x(),a.y()+b.y());
}


////////////////////////////////////////////////////////////


class YsVec3i : protected YsFixedLengthArray <int,3>
{
public:
	inline YsVec3i()
	{
	}
	inline YsVec3i(int x,int y,int z)
	{
		dat[0]=x;
		dat[1]=y;
		dat[2]=z;
	}
	YsVec3i(const char *x,const char *y,const char *z);

	inline const int &operator[](int idx) const
	{
		return dat[idx];
	}
	inline int &operator[](int idx)
	{
		return dat[idx];
	}
	operator const int *() const
	{
		return dat;
	}
	const char *Text(char buf[],const char *fmt="%d %d %d") const
	{
		sprintf(buf,fmt,x(),y(),z());
		return buf;
	}

	inline void AddX(const int &x)
	{
		dat[0]+=x;
	}
	inline void AddY(const int &y)
	{
		dat[1]+=y;
	}
	inline void AddZ(const int &z)
	{
		dat[2]+=z;
	}

	inline void SubX(const int &x)
	{
		dat[0]-=x;
	}
	inline void SubY(const int &y)
	{
		dat[1]-=y;
	}
	inline void SubZ(const int &z)
	{
		dat[2]-=z;
	}

	inline void MulX(const int &x)
	{
		dat[0]*=x;
	}
	inline void MulY(const int &y)
	{
		dat[1]*=y;
	}
	inline void MulZ(const int &z)
	{
		dat[2]*=z;
	}

	inline void DivX(const int &x)
	{
		dat[0]/=x;
	}
	inline void DivY(const int &y)
	{
		dat[1]/=y;
	}
	inline void DivZ(const int &z)
	{
		dat[2]/=z;
	}

	inline int x(void) const
	{
		return dat[0];
	}
	inline int y(void) const
	{
		return dat[1];
	}
	inline int z(void) const
	{
		return dat[2];
	}

	inline void Set(int x,int y,int z)
	{
		dat[0]=x;
		dat[1]=y;
		dat[2]=z;
	}

	inline void SetX(const int &x)
	{
		dat[0]=x;
	}
	inline void SetY(const int &y)
	{
		dat[1]=y;
	}
	inline void SetZ(const int &z)
	{
		dat[2]=z;
	}
};

inline const YsVec3i operator-(const YsVec3i &a)
{
	return YsVec3i(-a.x(),-a.y(),-a.z());
}

inline const YsVec3i operator-(const YsVec3i &a,const YsVec3i &b)
{
	return YsVec3i(a.x()-b.x(),a.y()-b.y(),a.z()-b.z());
}

inline const YsVec3i operator+(const YsVec3i &a,const YsVec3i &b)
{
	return YsVec3i(a.x()+b.x(),a.y()+b.y(),a.z()+b.z());
}


/* } */
#endif
