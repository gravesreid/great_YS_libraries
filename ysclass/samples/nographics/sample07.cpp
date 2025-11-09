#include <stdio.h>
#include <ysclass.h>

int main(void)
{
	int i;
	char *str;
	YsArray <char> ary;

	str="YsArray Example";

	for(i=0; str[i]!=0; i++)
	{
		ary.AppendItem(str[i]);
	}

	for(i=0; i<ary.GetNumItem(); i++)
	{
		printf("%c\n",ary.GetItem(i));
	}
	return 0;
}

