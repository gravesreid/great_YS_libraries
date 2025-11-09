#ifndef YSSORT_IS_INCLUDED
#define YSSORT_IS_INCLUDED
/* { */

template < class keyType,class assocType >
YSRESULT YsQuickSort(int n,keyType keyValue[],assocType associatedValue[],keyType rangeMinimum,keyType rangeMaximum)
{
	// int i;  // ####
	// printf("*\n");
	// printf("n=%d\n",n);
	// for(i=0; i<n; i++)
	// {
	// 	printf("%04d ",keyValue[i]);
	// 	if(i%8==7)
	// 	{
	// 		printf("\n");
	// 	}
	// }
	// printf("\n");


	if(n>1)
	{
		keyType threshold;

		threshold=(rangeMinimum+rangeMaximum)/keyType(2);

		// printf("thr=%d\n",threshold); // ####

		// smaller comes first
		keyType d0;
		int i,nSmall;

		nSmall=0;
		d0=keyValue[0];

		keyType maximumAmongSmaller,minimumAmongLarger;
		maximumAmongSmaller=rangeMinimum;
		minimumAmongLarger=rangeMaximum;
		for(i=0; i<n; i++) // Actually can begin with 1
		{
			keyType var;
			var=keyValue[i];
			if(var<=threshold)
			{
				maximumAmongSmaller=YsGreater(maximumAmongSmaller,var);

				keyType a;
				a=keyValue[i];
				keyValue[i]=keyValue[nSmall];
				keyValue[nSmall]=a;

				if(associatedValue!=NULL)
				{
					assocType x;
					x=associatedValue[i];
					associatedValue[i]=associatedValue[nSmall];
					associatedValue[nSmall]=x;
				}

				nSmall++;
			}
			else
			{
				minimumAmongLarger=YsSmaller(minimumAmongLarger,var);
			}
		}

		if(nSmall!=0 && nSmall!=n)
		{
			// printf("Small=%d\n",nSmall);  // ####
			// for(i=0; i<nSmall; i++)
			// {
			// 	printf("%04d ",keyValue[i]);
			// 	if(i%8==7)
			// 	{
			// 		printf("\n");
			// 	}
			// }
			// printf("\n");
			// printf("Large=%d\n",n-nSmall);  // ####
			// for(i=0; i<n-nSmall; i++)
			// {
			// 	printf("%04d ",(keyValue+nSmall)[i]);
			// 	if(i%8==7)
			// 	{
			// 		printf("\n");
			// 	}
			// }
			// printf("\n");

			if(associatedValue!=NULL)
			{
				YsQuickSort<keyType,assocType>
				    (nSmall    ,keyValue       ,associatedValue       ,rangeMinimum,maximumAmongSmaller);
				YsQuickSort<keyType,assocType>
				    ((n-nSmall),keyValue+nSmall,associatedValue+nSmall,minimumAmongLarger,rangeMaximum);
			}
			else
			{
				YsQuickSort<keyType,assocType>
				    (nSmall    ,keyValue       ,NULL,rangeMinimum,maximumAmongSmaller);
				YsQuickSort<keyType,assocType>
				    ((n-nSmall),keyValue+nSmall,NULL,minimumAmongLarger,rangeMaximum);
			}
		}
		// >> This portion is added on 2002/06/09
		// Now nSmall==0 || nSmall==n.  It means either:
		//   all values are equal, or
		//   keyType is integer, and max-min=1
		else
		{
			int i;
			keyType min;
			min=keyValue[0];
			for(i=1; i<n; i++)
			{
				min=YsSmaller(min,keyValue[i]);
			}

			nSmall=0;
			// for(nSmall=0; keyValue[nSmall]==min && nSmall<n; nSmall++) // nSmall<n is added on 2004/10/06
			for(nSmall=0; nSmall<n && keyValue[nSmall]==min; nSmall++) // nSmall<n must be checked first (2004/12/07)
			{
			}

			for(i=n-1; i>nSmall; i--)
			{
				if(keyValue[i]==min)
				{
					YsSwapSomething <keyType> (keyValue[i],keyValue[nSmall]);
					if(associatedValue!=NULL)
					{
						YsSwapSomething <assocType> (associatedValue[i],associatedValue[nSmall]);
					}
					nSmall++;
				}
			}
		}
		// << This portion is added on 2002/06/09
	}
	return YSOK;
}

template <class keyType,class assocType>
YSRESULT YsQuickSort(int n,keyType keyValue[],assocType associatedValue[]=NULL)
{
	int i;
	keyType keyMin,keyMax;
	if(n>1)
	{
		keyMin=keyValue[0];
		keyMax=keyValue[0];
		for(i=1; i<n; i++)
		{
			keyMin=YsSmaller(keyMin,keyValue[i]);
			keyMax=YsGreater(keyMax,keyValue[i]);
		}
		return YsQuickSort(n,keyValue,associatedValue,keyMin,keyMax);
	}
	return YSOK;  // Don't have to sort.
}


// Quick Sort Any works with a type for which operator/ and operator- are not defined.
// keyValue can be anything for which compare operators are defined, like a pointer.
template <class keyType,class assocType>
YSRESULT YsQuickSortAny(int n,keyType keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	int i,dividePoint;
	keyType divider,nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];
	nonDivider=NULL;

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortSortAny %d\n",n);
	}

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		if(allEqual==YSTRUE && keyValue[i]!=divider)  // Remark: This check must come before swapping.
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}

		if(keyValue[i]<divider)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <keyType> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
	}

	if(allEqual==YSTRUE)
	{
		if(watch==YSTRUE)
		{
			printf("All Equal\n");
		}
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		if(watch==YSTRUE)
		{
			YsPrintf("YsQuickSortAny: Trying again %08x %08x\n",divider,nonDivider);
		}
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(watch==YSTRUE)
	{
		printf("Next: %d,%d\n",dividePoint,n-dividePoint);
	}

	if(assocValue!=NULL)
	{
		YsQuickSortAny <keyType,assocType> (dividePoint,keyValue,assocValue,watch);
		YsQuickSortAny <keyType,assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint,watch);
	}
	else
	{
		YsQuickSortAny <keyType,assocType> (dividePoint,keyValue,NULL,watch);
		YsQuickSortAny <keyType,assocType> ((n-dividePoint),keyValue+dividePoint,NULL,watch);
	}

	return YSOK;
}


/* } */
#endif
