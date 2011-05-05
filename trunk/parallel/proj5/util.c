#include <stdio.h>
#include <time.h>
#include "util.h"

// Prints the primes
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
			printf("%d, ", i * 2 + 3);
		}
	}
	if(unmarkedCount == 0)
		printf("all marked\n");
	else
		printf("(%ld unmarked)\n", unmarkedCount);
}

inline unsigned long getTimeNanoseconds()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC, &tp);
	return tp.tv_sec * 1000000000 + tp.tv_nsec;
}

unsigned long calculateRangeBegin(int threadID, int threadCount, unsigned long zeroRangeEnd, unsigned long maxNum)
{
	unsigned long begin;
	unsigned long coverage;
	
	coverage = maxNum - zeroRangeEnd;
	begin = coverage / threadCount * threadID + 1 + zeroRangeEnd;

	// Cover the leftover by assigning one extra to threads front to back
	// We do this by making the begin one extra larger for each thread _after_
	// the first one that will be computing more
	if(threadID + 1 > threadCount - coverage % threadCount)
	{
		begin++;
	}

	// Adjust to make it start on the odd
	if(begin % 2 == 0)
		begin++;
		
	return begin;
}

unsigned long calculateRangeEnd(int threadID, int threadCount, unsigned long zeroRangeEnd, unsigned long maxNum)
{
	// Normally go to 2 before the next range, but if we're the last thread
	// then we need to just go to the last element (that's actually odd)
	if(threadID < threadCount - 1)
		return calculateRangeBegin(threadID + 1, threadCount, zeroRangeEnd, maxNum) - 2;
	else if(maxNum % 2 == 0)
		return maxNum - 1;
	else
		return maxNum;
}

