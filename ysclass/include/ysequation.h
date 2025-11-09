#ifndef YSEQUATION_IS_INCLUDED
#define YSEQUATION_IS_INCLUDED
/* { */

/* ax+by+c=0 */
/* px+qy+r=0 */
YSRESULT YsSolveTwoLinearEquation
    (double *x,double *y,
     const double &a,const double &b,const double &c,
     const double &p,const double &q,const double &r);

/* a1 x+b1 y+c1 z+d1=0 */
/* a2 x+b2 y+c2 z+d2=0 */
/* a3 x+b3 y+c3 z+d3=0 */
YSRESULT YsSolveThreeLinearEquation
    (double *x,double *y,double *z,
     const double &a1,const double &b1,const double &c1,const double &d1,
     const double &a2,const double &b2,const double &c2,const double &d2,
     const double &a3,const double &b3,const double &c3,const double &d3);
/* } */
#endif
