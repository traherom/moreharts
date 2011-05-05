#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "mod0.h"
#include "mod1.h"
#include "mod2.h"
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
	int calcMode;
	
	// Results
	double elapsedTime;
	unsigned long globalCount;
	
	// Range for this process
	unsigned long proc0_size;
	
	// MPI stuff
	int processorCount;
	int id;
	
	// Make MPI strip off any special args the user gave it on the command line
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &processorCount);
	
	// Grab how many primes we're looking for and the format
	// we should output in (-t makes us do a CSV format)
	if(argc == 2)
	{
		maxNum = atoi(argv[2]);
		calcMode = 2;
		csvOutput = FALSE;
	}
	else if(argc == 3)
	{
		maxNum = atoi(argv[2]);
		calcMode = atoi(argv[1]);
		csvOutput = FALSE;
	}
	else if(argc == 4)
	{
		maxNum = atol(argv[3]);
		calcMode = atoi(argv[2]);
		csvOutput = TRUE;
	}
	else
	{
		if(id == 0)
			fprintf(stderr, "Usage: %s [-t] [mod #] <max>\n", argv[0]);
		MPI_Finalize();
		exit(-1);
	}
	
	if(calcMode < 0 || calcMode > 2)
	{
		if(id == 0)
			fprintf(stderr, "mod # must be between 0-2, where it represents the modification desired\n");
		MPI_Finalize();
		exit(-1);
	}
	
	// Check that some condition the algorithm requires is met.
	// Honestly I don't get what this actually does. Need to ask G
	proc0_size = (maxNum - 1) / processorCount;
	if((2 + proc0_size) < (int)sqrt((double) maxNum))
	{
		if(id == 0)
			fprintf(stderr, "Too many parameters");
			
		MPI_Finalize();
		exit(-1);
	}
	
	// Start timer _after_ everyone initilizes. We do
	// this as we are testing the algorithm's efficiency
	// compared to optimizations, rather than parallel vs. serial
	MPI_Barrier(MPI_COMM_WORLD);
	elapsedTime = -MPI_Wtime();
	
	// Calculate primes!
	if(calcMode == 0)
		globalCount = getPrimeCountPlain(maxNum, processorCount, id);
	else if(calcMode == 1)
		globalCount = getPrimeCountNoEvens(maxNum, processorCount, id);
	else
		globalCount = getPrimeCountLessComm(maxNum, processorCount, id);
	
	// Stop timer and output the result
	elapsedTime += MPI_Wtime();
	if(id == 0)
	{
		if(csvOutput)
		{
			printf("%ld, %d, %ld, %d, %10.15f\n", maxNum, calcMode, globalCount, processorCount, elapsedTime);
		}
		else
		{
			printf("Modification level: %d\n", calcMode);
			printf("%ld primes are less than or equal to %ld\n", globalCount, maxNum);
			printf("Total elapsed time: %10.15f\n", elapsedTime);
			printf("Processors used: %d\n", processorCount);
		}
	}
	
	MPI_Finalize();
	return 0;
}

