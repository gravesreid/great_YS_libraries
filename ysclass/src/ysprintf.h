#ifndef YSPRINTF_IS_INCLUDED
#define YSPRINTF_IS_INCLUDED
/* { */

#include "ysdef.h"

// Declaration /////////////////////////////////////////////
class YsPrintf
{
protected:
	virtual void Output(char str[]);
public:
	YsPrintf();
	YsPrintf(const char *fom,...);
	YsPrintf(int a,const char *fom,...);

	static YsPrintf *def;
	YSBOOL enabled;

	int Printf(const char *fom,...);

	void SetDefault(void);
	static YsPrintf *GetDefault(void);
};

////////////////////////////////////////////////////////////

void YsTurnOnErrorOutput(void);
void YsTurnOffErrorOutput(void);
int YsErrOut(const char *fmt,...);

void YsTurnOnDiagnosticOutput(void);
void YsTurnOffDiagnosticOutput(void);
int YsDiagOut(const char *fmt,...);

////////////////////////////////////////////////////////////

class YsLoopCounter
{
public:
	YsLoopCounter();
	void BeginCounter(unsigned length);
	void ShowCounter(unsigned ctr);
	void Increment(void);
	void EndCounter(void);

	void SetOutput(YsPrintf *print);

	unsigned GetCount(void) const;

	static unsigned int defUpdateInterval;
	static char defNewLine;

protected:
	unsigned lastShowedTime;
	unsigned updateInterval;
	unsigned length;
	unsigned current;
	YsPrintf *output;
};

/* } */
#endif
