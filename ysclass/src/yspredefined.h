#ifndef YSPREDEFINED_IS_INCLUDED
#define YSPREDEFINED_IS_INCLUDED
/* { */


const class YsVec3 &YsOrigin(void);
const class YsAtt3 &YsZeroAtt(void);

const class YsVec3 &YsXVec(void);
const class YsVec3 &YsYVec(void);
const class YsVec3 &YsZVec(void);
const class YsVec3 &YsXYZ(void);

const class YsPlane &YsXYPlane(void);
const class YsPlane &YsXZPlane(void);
const class YsPlane &YsYZPlane(void);

const class YsColor &YsBlack(void);
const class YsColor &YsBlue(void);
const class YsColor &YsRed(void);
const class YsColor &YsMagenta(void);
const class YsColor &YsGreen(void);
const class YsColor &YsCyan(void);
const class YsColor &YsYellow(void);
const class YsColor &YsWhite(void);
const class YsColor &YsDarkBlue(void);
const class YsColor &YsDarkRed(void);
const class YsColor &YsDarkMagenta(void);
const class YsColor &YsDarkGreen(void);
const class YsColor &YsDarkCyan(void);
const class YsColor &YsDarkYellow(void);
const class YsColor &YsGray(void);
const class YsColor &YsDarkGray(void);
const class YsColor &YsThreeBitColor(int idx);
const class YsColor YsGrayScale(const double &scale);

const class YsMatrix4x4 &YsIdentity4x4(void);
const class YsMatrix3x3 &YsIdentity3x3(void);

/* } */
#endif
