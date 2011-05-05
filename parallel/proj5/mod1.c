#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "mod1.h"
#include "util.h"

unsigned long getPrimeCountNoEvens(unsigned long maxNum)
{
	// State tracking
	unsigned long i;
	bool *marked = 0;
	unsigned long count = 0;
	
	int threadCount;
	int threadID;
	
	// Range for this process
	unsigned long rangeBegin;
	unsigned long rangeBeginIndex;
	unsigned long rangeEnd;
	unsigned long rangeSize;
	
	unsigned long zeroRangeBegin;
	unsigned long zeroRangeEnd;
	unsigned long zeroRangeSize;
	
	unsigned long fullSize;
	
	// Allocate completely unmarked array
	fullSize = (maxNum - 3) / 2 + 1;
	marked = (bool*)calloc(fullSize, sizeof(bool));
	if(marked == NULL)
	{
		fprintf(stderr, "Cannot allocate memory for marking array\n");
		exit(1);
	}
	
	// Precompute up to sqrt(n) and adjust the rest of the calculation range to not
	// redo work. Actually, if the number is super small, just calculate it all
	zeroRangeBegin = 3;
	zeroRangeEnd = sqrt(maxNum);
	zeroRangeSize = (zeroRangeEnd - zeroRangeBegin) / 2 + 1;
	seiveRange(marked, zeroRangeEnd, zeroRangeBegin, zeroRangeEnd, 0);

	#pragma omp parallel private(i, threadCount, threadID, rangeBegin, rangeEnd, rangeSize, rangeBeginIndex) reduction(+:count)
	{
		// Now have each thread do a separate bit for everything past sqrt(n)
		threadCount = omp_get_num_threads();
		threadID = omp_get_thread_num();
	
		rangeBegin = calculateRangeBegin(threadID, threadCount, zeroRangeEnd, maxNum);
		rangeBeginIndex = (rangeBegin - 3) / 2;
		rangeEnd = calculateRangeEnd(threadID, threadCount, zeroRangeEnd, maxNum);
		rangeSize = (rangeEnd - rangeBegin) / 2 + 1;

		// Compute our range
		seiveRange(marked, zeroRangeEnd, rangeBegin, rangeEnd, rangeBeginIndex);

		// Count the number of primes we found
		count = 0;
		for(i = 0; i < rangeSize; i++)
		{
			if(!marked[rangeBeginIndex + i])
				count++;
		}

		// One person counts the number of primes in the precomputed section
		#pragma omp single nowait
		{
			// Extra one for the implicit "2"
			count++;

			for(i = 0; i < zeroRangeSize; i++)
			{
				if(!marked[i])
					count++;
			}
		}
		
		// Close off parallel section of code
	}

	return count;
}

void seiveRange(bool *marked, unsigned long zeroRangeEnd,
	unsigned long rangeBegin, unsigned long rangeEnd, unsigned long rangeBeginIndex)
{
	unsigned long currentPrime;
	unsigned long index;
	unsigned long rangeSize;
	unsigned long i;
	unsigned long first;

	// How big is the actual array section we're working with?
	rangeSize = (rangeEnd - rangeBegin) / 2 + 1;

	// Only have to mark off the primes that were up to sqrt(n)
	currentPrime = 3;
	index = 0;
	while(currentPrime <= zeroRangeEnd)
	{
		// Find where the first multiple of the current prime is within the
		// range we are responsible for
		if(currentPrime * currentPrime > rangeBegin)
		{
			first = (currentPrime * currentPrime - rangeBegin) / 2;
		}
		else
		{
			if(rangeBegin % currentPrime == 0)
			{
				// The prime falls right on the edge of our range
				first = 0;
			}
			else
			{
				first = currentPrime - (rangeBegin % currentPrime);
				if(first % 2 != 0)
				{
					// The number we found isn't actually in our range (it's even), but
					// the next instance is
					first += currentPrime;
				}
			
				// And shift down to match our indexes
				first = first / 2;
			}
		}
		
		// Mark every multiple of this prime
		for(i = first; i < rangeSize; i += currentPrime)
		{
			marked[rangeBeginIndex + i] = TRUE;
		}

		// Find next unmarked index, which happens to be the index of
		// our next prime.
		index++;
		while(marked[index] == TRUE)
			index++;
		
		// Convert to the actual prime from this index
		// Our array is half-size and starts at 3
		currentPrime = index * 2 + 3;
	}
}

