#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "mod1.h"
#include "util.h"

int main(int argc, char **argv)
{
	// Variables are all nice unsigned long nows because I didn't
	// like being limited to ranges of only 32,000/64,000 numbers.
	// Or whatever int is on this system. I think it might already
	// have been long, but I didn't want to check. This is safer
	// Settings
	bool csvOutput;
	unsigned long maxNum;
	int threadCount;
	
	// Results
	unsigned long startTime;
	unsigned long endTime;
	unsigned long elapsedTime;
	unsigned long globalCount;
	
	// Grab how many primes we're looking for and the format
	// we should output in (-t makes us do a CSV format)
	if(argc == 2)
	{
		threadCount = omp_get_max_threads();
		maxNum = atol(argv[1]);
		csvOutput = FALSE;
	}
	else if(argc == 3)
	{
		threadCount = atoi(argv[1]);
		omp_set_num_threads(threadCount);
		maxNum = atol(argv[2]);
		csvOutput = FALSE;
	}
	else if(argc == 4)
	{
		threadCount = atoi(argv[2]);
		omp_set_num_threads(threadCount);
		maxNum = atol(argv[3]);
		csvOutput = TRUE;
	}
	else
	{
		fprintf(stderr, "Usage: %s [-t] [<threads>] <max>\n", argv[0]);
		exit(-1);
	}

	// Start timer
	startTime = getTimeNanoseconds();
	
	// Calculate primes!
	globalCount = getPrimeCountNoEvens(maxNum);
	
	// Stop timer and output the result
	endTime = getTimeNanoseconds();
	elapsedTime = endTime - startTime;
	
	if(csvOutput)
	{
		printf("%ld, %d, %ld, %ld\n", maxNum, threadCount, elapsedTime, globalCount);
	}
	else
	{
		printf("%ld primes are less than or equal to %ld\n", globalCount, maxNum);
		printf("Total elapsed time around %ld ns\n", elapsedTime);
		printf("Threads used: %d\n", threadCount);
	}
	
	return 0;
}

