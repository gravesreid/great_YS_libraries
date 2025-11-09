#ifndef YSCLIP_IS_INCLUDED
#define YSCLIP_IS_INCLUDED
/* { */

YSRESULT YsClipLineSeg2(YsVec2 &c1,YsVec2 &c2,const YsVec2 &l1,const YsVec2 &l2,const YsVec2 &range1,const YsVec2 &range2);

YSRESULT YsClipInfiniteLine2
    (YsVec2 &c1,YsVec2 &c2,
     const YsVec2 &org,const YsVec2 &vec,const YsVec2 &range1,const YsVec2 &range2);

YSRESULT YsClipInfiniteLine3
    (YsVec3 &c1,YsVec3 &c2,
     const YsVec3 &org,const YsVec3 &vec,const YsVec3 &range1,const YsVec3 &range2);

YSRESULT YsClipPolygon3
   (int &newNp,YsVec3 newp[],int bufLen,int np,const YsVec3 p[],const YsVec3 &range1,const YsVec3 &range2);

YSRESULT YsClipLineSeg3
   (YsVec3 &newP1,YsVec3 &newP2,
    const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &range1,const YsVec3 &range2);


/* } */
#endif
