#include <stdio.h>
#include <ysclass.h>

// Example of
//   YsCheckInsidePolygon2
// This function is for 2D polygon. But you also can use
//   YsCheckInsidePolygon3
// in a same manner. Please refer to the function reference.

const int nVtx=5;
double vtxTemplate[nVtx][2]=
{
	{10,10},{40,5},{60,15},{50,20},{10,16}
};


int main(void)
{
	int i,x,y;
	YsVec2 vtx[nVtx];
	YsVec2 poi;  // Point of interest

	for(i=0; i<nVtx; i++)
	{
		vtx[i].Set(vtxTemplate[i]);
	}

	for(y=0; y<24; y++)
	{
		for(x=0; x<75; x++)
		{
			poi.Set(double(x),double(y));
			switch(YsCheckInsidePolygon2(poi,nVtx,vtx))
			{
			case YSINSIDE:
				printf("I");
				break;
			case YSOUTSIDE:
				printf("O");
				break;
			case YSBOUNDARY:
				printf("B");
				break;
			case YSUNKNOWNSIDE:
				printf("?");
				break;
			}
		}
		printf("\n");
	}

	return 0;
}
