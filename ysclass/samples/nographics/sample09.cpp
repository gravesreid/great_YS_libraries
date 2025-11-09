#include <stdio.h>
#include <ysclass.h>

int main(void)
{
	int i;
	char str[256];
	int ac;
	char *av[32];

	strcpy(str,"Cleared for take off, \"Cessna 19015\"");

	YsArguments(&ac,av,32,str);
	// Notice:str is broken in this function.
	// You have to keep a copy if you need it later.

	for(i=0; i<ac; i++)
	{
		printf("%d %s\n",i,av[i]);
	}

	return 0;
}

