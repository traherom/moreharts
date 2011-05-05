#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mod1.h"
#include "util.h"

unsigned long getPrimeCountNoEvens(unsigned long maxNum, int processorCount, int id)
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
	bool *marked;
	
	// Determine the number range we are processing
	rangeBegin = calculateRangeBegin(id, processorCount, maxNum);
	rangeEnd = calculateRangeEnd(id, processorCount, maxNum);
	
	// Truncate here, so that if we were processing, say, 0-5,
	// then we only have two array spots for the odd numbers that may/may not
	// be prime (3 and 5). 2 is always implied
	rangeSize = (rangeEnd - rangeBegin) / 2 + 1;
	
	// Allocate completely unmarked array
	marked = (bool*)malloc(sizeof(bool) * rangeSize);
	if(marked == NULL)
	{
		fprintf(stderr, "Cannot allocate memory on processor %d\n", id);
		MPI_Finalize();
		exit(1);
	}
	
	for(i = 0; i < rangeSize; i++)
		marked[i] = FALSE;
	
	// No point in checking beyond sqrt(maxNum).
	index = 0;
	currentPrime = 3; // 2 not saved, only implied for processor 0
	while(currentPrime * currentPrime <= maxNum)
	{
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
		
		if(id == 0)
		{
			// Find next unmarked index, which happens to be the index of
			// our next prime
			index++;
			while(marked[index] == TRUE)
				index++;
			
			// Convert to the actual prime from this index
			// Our array is half-size and starts at 3
			currentPrime = index * 2 + 3;
		}
		
		// Send/receive next prime to mark off all its multiples
		MPI_Bcast(&currentPrime, 1, MPI_INT, 0, MPI_COMM_WORLD);
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

unsigned long calculateRangeBegin(int id, int procs, unsigned long max)
{
	unsigned long begin;
	if(id == 0)
		begin = 3;
	else
		begin = id * max / procs + 1;
		
	// Adjust to make it start on the odd
	if(begin % 2 == 0)
		begin++;
		
	return begin;
}

unsigned long calculateRangeEnd(int id, int procs, unsigned long max)
{
	return calculateRangeBegin(id + 1, procs, max) - 2;
}

