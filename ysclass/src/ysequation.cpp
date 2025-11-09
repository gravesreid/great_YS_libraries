#include "ysclass.h"



/*
  ax+by+c=0;
  px+qy+r=0;

  |a   b||x| |-c|
  |     || |=|  |
  |p   q||y| |-r|


  Inverted
   |ia  ib|     1  | q  -b|
   |      | = -----|      |
   |ic  id|   aq-bp|-p   a|


   |x| |ia  ib||-c|
   | |=|      ||  |
   |y| |ic  id||-r|

*/


/* ax+by+c=0 */
/* px+qy+r=0 */
YSRESULT YsSolveTwoLinearEquation
    (double *x,double *y,
     const double &a,const double &b,const double &c,
     const double &p,const double &q,const double &r)
{
	double inv[4],mat[4];
	mat[0]=a;
	mat[1]=b;
	mat[2]=p;
	mat[3]=q;
	if(YsInvertMatrix2x2(inv,mat)==YSOK)
	{
		*x=-c*inv[0]-r*inv[1];
		*y=-c*inv[2]-r*inv[3];
		return YSOK;
	}
	return YSERR;

/*
	double inv;
	double ia,ib,ic,id;

	inv=a*q-b*p;
	if(inv==0)
	{
		return YSERR;
	}

	ia= q/inv;
	ib=-b/inv;
	ic=-p/inv;
	id= a/inv;

	*x=-c*ia-r*ib;
	*y=-c*ic-r*id;
	return YSOK;
*/
}



/*
  ax+by+cz+d=0 defines a plane. (x,y,z) from 3 equations can be calculated
  as a crossing point of 3 planes.
*/
YSRESULT YsSolveThreeLinearEquation
    (double *x,double *y,double *z,
     const double &a1,const double &b1,const double &c1,const double &d1,
     const double &a2,const double &b2,const double &c2,const double &d2,
     const double &a3,const double &b3,const double &c3,const double &d3)
{
	double inv[9],mat[9];
	mat[0]=a1;
	mat[1]=b1;
	mat[2]=c1;
	mat[3]=a2;
	mat[4]=b2;
	mat[5]=c2;
	mat[6]=a3;
	mat[7]=b3;
	mat[8]=c3;
	if(YsInvertMatrixNxN <3> (inv,mat)==YSOK)
	{
		*x=-d1*inv[0]-d2*inv[1]-d3*inv[2];
		*y=-d1*inv[3]-d2*inv[4]-d3*inv[5];
		*z=-d1*inv[6]-d2*inv[7]-d3*inv[8];
		return YSOK;
	}
	return YSERR;
}


