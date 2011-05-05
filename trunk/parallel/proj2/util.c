#include <stdio.h>
#include "util.h"

// Prints the indexes which are unmarked in the array
// Purely for debugging purposes
void printUnmarked(bool *array, int length)
{
	int i;
	unsigned long unmarkedCount = 0;
	for(i = 0; i < length; i++)
	{
		if(!array[i])
		{
			unmarkedCount++;
			printf("%d, ", i);
		}
	}
	if(unmarkedCount == 0)
		printf("all marked\n");
	else
		printf("(%ld unmarked)\n", unmarkedCount);
}

