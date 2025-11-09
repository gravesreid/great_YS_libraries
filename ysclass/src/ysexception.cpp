// $Id$

#include <stdio.h>
#include "ysexception.h"



YsExceptionHandler *YsExceptionHandler::defaultHandler=NULL;

void YsExceptionHandler::Register(void)
{
	defaultHandler=this;
}

void YsExceptionHandler::Exception(EXCEPTIONTYPE et,const char msg[])
{
	if(defaultHandler!=NULL)
	{
		defaultHandler->Reaction(et,msg);
	}
	// Let it fall to the system default or ignore
}

void YsExceptionHandler::Reaction(EXCEPTIONTYPE et,const char msg[])
{
}

