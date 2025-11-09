#include <stdio.h>
#include <ysclass.h>

YsList <char [256]> *CreateNewList(char str[]);

int main(void)
{
	YsList <char [256]> *lst,*app;

	lst=NULL;

	app=CreateNewList("A");
	lst=lst->Append(app);

	app=CreateNewList("fish");
	lst=lst->Append(app);

	app=CreateNewList("is");
	lst=lst->Append(app);

	app=CreateNewList("flying");
	lst=lst->Append(app);

	app=CreateNewList("across");
	lst=lst->Append(app);

	app=CreateNewList("the");
	lst=lst->Append(app);

	app=CreateNewList("sky.");
	lst=lst->Append(app);


	YsList <char [256]> *tracker;

	for(tracker=lst; tracker!=NULL; tracker=tracker->Next())
	{
		printf("%s ",tracker->dat);
	}

	printf("\n");

	return 0;
}

YsList <char [256]> *CreateNewList(char str[])
{
	YsList <char [256]> *newList;
	newList=new YsList <char [256]>;
	strcpy(newList->dat,str);
	return newList;
}
