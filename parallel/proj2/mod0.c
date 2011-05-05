#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mod0.h"
#include "util.h"

unsigned long getPrimeCountPlain(unsigned long maxNum, int processorCount, int id)
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
	rangeBegin = 2 + id * (maxNum - 1) / processorCount;
	rangeEnd = 2 + (id + 1) * (maxNum - 1) / processorCount - 1;
	rangeSize = rangeEnd - rangeBegin + 1;
	
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
	
	// For task 0 only really
	index = 0;
	
	// No point in checking beyond sqrt(maxNum).
	currentPrime = 2;
	while(currentPrime * currentPrime <= maxNum)
	{
		// Find where this currentPrime starts within the range we are responsible for
		if(currentPrime * currentPrime > rangeBegin)
		{
			first = currentPrime * currentPrime - rangeBegin;
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
			}
		}
		
		// Mark every multiple of this prime
		for(i = first; i < rangeSize; i += currentPrime)
		{
			marked[i] = TRUE;
		}
		
		// Find next unmarked number, which happens to be prime
		if(id == 0)
		{
			index++;
			while(marked[index] == TRUE)
				index++;
			currentPrime = index + 2;
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
	
	// Sum up how many primes everyone found
	MPI_Reduce(&count, &globalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	return globalCount;
}

