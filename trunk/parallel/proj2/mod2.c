#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mod2.h"
#include "mod1.h"
#include "util.h"

unsigned long getPrimeCountLessComm(unsigned long maxNum, int processorCount, int id)
{
	// Results
	unsigned long count;
	unsigned long globalCount;
	
	// State tracking
	unsigned long first;
	unsigned long currentPrime;
	unsigned long index;
	unsigned long i;
	
	// Range for this process
	unsigned long rangeBegin;
	unsigned long rangeEnd;
	unsigned long rangeSize;
	unsigned long zeroBegin;
	unsigned long zeroEnd;
	unsigned long zeroSize;
	bool *zeroMarked;
	bool *marked;
	
	// Determine the number range we are processing
	rangeBegin = calculateRangeBegin(id, processorCount, maxNum);
	rangeEnd = calculateRangeEnd(id, processorCount, maxNum);
	
	// Truncate here, so that if we were processing, say, 0-5,
	// then we only have two array spots for the odd numbers that may/may not
	// be prime (3 and 5). 2 is always implied
	rangeSize = (rangeEnd - rangeBegin) / 2 + 1;

	// Declare the overall beginning and end regions if we are not processor zero
	if (id != 0)
	{
		zeroBegin = 2;
		zeroEnd = sqrt(maxNum);
		zeroSize = zeroEnd - zeroBegin + 1;
	}
	
	// Allocate completely unmarked array
	marked = (bool*)malloc(sizeof(bool) * rangeSize);
	if (id != 0)
	{
		zeroMarked = (bool*)malloc(sizeof(bool) * zeroSize);
	}
	else
	{
		// Avoids yet more special casing for processor 0
		zeroMarked = marked;
	}

	if(marked == NULL)
	{
		fprintf(stderr, "Cannot allocate memory on processor %d\n", id);
		MPI_Finalize();
		exit(1);
	}
	if(id != 0 && zeroMarked == NULL)
	{
		fprintf(stderr, "Cannot allocate memory on processor %d\n", id);
		MPI_Finalize();
		exit(1);
	}
	
	for(i = 0; i < rangeSize; i++)
		marked[i] = FALSE;
	if(id != 0)
		for(i = 0; i < zeroSize; i++)
			zeroMarked[i] = FALSE;
	
	// No point in checking beyond sqrt(maxNum).
	index = 0;
	currentPrime = 3; // 2 not saved, only implied for processor 0
	while(currentPrime * currentPrime <= maxNum)
	{
		if (id != 0)
		{
			// Find where this currentPrime starts within the range we are responsible for
			first = (currentPrime * currentPrime - zeroBegin) / 2;
			
			// Mark every other multiple of this prime in the zeroth array
			for(i = first; i < zeroSize; i += currentPrime)
			{
				zeroMarked[i] = TRUE;
			}
		}

		// Find where the first multiple of the current prime is within the range we are responsible for
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
					// The number we found isn't actually in our range (it's even), but the next instance is
					first += currentPrime;
				}
				
				// And shift down to match our indexes
				first = first / 2;
			}
		}
		
		// Mark every multiple of this prime
		for(i = first; i < rangeSize; i += currentPrime)
		{
			marked[i] = TRUE;
		}
		
		// Find next unmarked index, which happens to be the index of
		// our next prime
		index++;
		while(zeroMarked[index] == TRUE)
			index++;
		
		// Convert to the actual prime from this index
		// Our array is half-size and starts at 3
		currentPrime = index * 2 + 3;
	}
		
	// Count the number of primes we found
	count = 0;
	for(i = 0; i < rangeSize; i++)
	{
		if(!marked[i])
			count++;
	}
	
	// Processor 0 also "found" 2
	if(id == 0)
		count++;
	
	// Sum up how many primes everyone found
	MPI_Reduce(&count, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	return globalCount;
}


