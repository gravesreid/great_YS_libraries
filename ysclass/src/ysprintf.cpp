#include <stdio.h>
#include <stdarg.h>

#include "ysprintf.h"



// Implementation //////////////////////////////////////////
YsPrintf::YsPrintf()
{
	enabled=YSTRUE;
}

YsPrintf::YsPrintf(const char *fmt,...)
{
	if(def!=NULL && def->enabled!=YSTRUE)
	{
		return;
	}

	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fmt);
	a=vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	if(def!=NULL)
	{
		def->Output(buf);
	}
	else
	{
		printf("%s",buf);
	}
}

YsPrintf::YsPrintf(int x,const char *fmt,...)
{
	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fmt);
	a=vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	if(def!=NULL)
	{
		def->Output(buf);
	}
	else
	{
		printf("%s",buf);
	}
}

void YsPrintf::Output(char str[])
{
	printf("%s",str);
}

int YsPrintf::Printf(const char *fmt,...)
{
	if(this!=NULL && enabled!=YSTRUE)
	{
		return 0;
	}

	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fmt);
	a=vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	if(this!=NULL)
	{
		Output(buf);
	}
	else
	{
		printf("%s",buf);
	}
	return a;
}

YsPrintf *YsPrintf::def=NULL;

void YsPrintf::SetDefault(void)
{
	def=this;
}

YsPrintf *YsPrintf::GetDefault(void)
{
	return def;
}

////////////////////////////////////////////////////////////

static int YsClassOutputError=1;

void YsTurnOnErrorOutput(void)
{
	YsClassOutputError=1;
}

void YsTurnOffErrorOutput(void)
{
	YsClassOutputError=0;
}

int YsErrOut(const char *fmt,...)
{
	if(YsClassOutputError==1)
	{
		int a;
		char buf[4096];
		va_list arg_ptr;

		va_start(arg_ptr,fmt);
		a=vsprintf(buf,fmt,arg_ptr);
		va_end(arg_ptr);

		printf("%s",buf);

		return a;
	}
	return 0;
}


////////////////////////////////////////////////////////////

static int YsDiagnosticOutput=0;

void YsTurnOnDiagnosticOutput(void)
{
	YsDiagnosticOutput=1;
}

void YsTurnOffDiagnosticOutput(void)
{
	YsDiagnosticOutput=0;
}

int YsDiagOut(const char *fmt,...)
{
	if(YsDiagnosticOutput==1)
	{
		int a;
		char buf[4096];
		va_list arg_ptr;

		va_start(arg_ptr,fmt);
		a=vsprintf(buf,fmt,arg_ptr);
		va_end(arg_ptr);

		printf("%s",buf);

		return a;
	}
	return 0;
}

////////////////////////////////////////////////////////////

#include <time.h>

// #ifdef WIN32
// #include <windows.h>
// HWND YsLoopCounter_OutputHWnd=NULL;
// #endif

unsigned int YsLoopCounter::defUpdateInterval=500;  // 500ms
char YsLoopCounter::defNewLine='\r';

YsLoopCounter::YsLoopCounter()
{
	output=NULL;
	lastShowedTime=0;
	length=0;
	current=0;
	updateInterval=defUpdateInterval;
	output=YsPrintf::def;
}

void YsLoopCounter::BeginCounter(unsigned l)
{
	length=l;
	lastShowedTime=clock();
	current=0;
}

void YsLoopCounter::ShowCounter(unsigned ctr)
{
	unsigned t;
	t=clock();
	current=ctr;
	if(t<lastShowedTime)   // Clock Underflow
	{
		lastShowedTime=t;
		return;
	}
	else if(updateInterval<((t-lastShowedTime)*1000/CLOCKS_PER_SEC))
	{
// #ifdef WIN32
// 		if(YsLoopCounter_OutputHWnd!=NULL)
// 		{
// 			char str[256];
// 			if(length!=0)
// 			{
// 				sprintf(str,"%d/%d",ctr,length);
// 			}
// 			else
// 			{
// 				sprintf(str,"%d",ctr);
// 			}
// 			SetWindowText(YsLoopCounter_OutputHWnd,str);
// 		}
// 		else
// #endif

		if(length!=0)
		{
			output->Printf("%d/%d%c",ctr,length,defNewLine);
		}
		else
		{
			output->Printf("%d%c",ctr,defNewLine);
		}
		lastShowedTime=t;
	}
}

void YsLoopCounter::Increment(void)
{
	ShowCounter(current+1);
}

void YsLoopCounter::EndCounter(void)
{
// #ifdef WIN32
// 		if(YsLoopCounter_OutputHWnd!=NULL)
// 		{
// 			SetWindowText(YsLoopCounter_OutputHWnd,"");
// 		}
// 		else
// #endif
	if(length!=0)
	{
		output->Printf("%d/%d\n",length,length);
		output->Printf("Done!\n");
	}
	else
	{
		output->Printf("Done!\n");
	}
}

void YsLoopCounter::SetOutput(YsPrintf *print)
{
	output=print;
}

unsigned YsLoopCounter::GetCount(void) const
{
	return current;
}


