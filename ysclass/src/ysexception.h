#ifndef YSEXCEPTION_IS_INCLUDED
#define YSEXCEPTION_IS_INCLUDED
/* { */

// $Id$

class YsExceptionHandler
{
public:
	enum EXCEPTIONTYPE
	{
		GENERAL,
		OUTOFMEMORY
	};

	void Register(void);
	static void Exception(EXCEPTIONTYPE et,const char msg[]);
	static YsExceptionHandler *defaultHandler;

protected:
	virtual void Reaction(EXCEPTIONTYPE et,const char msg[]);
};


/* } */
#endif
