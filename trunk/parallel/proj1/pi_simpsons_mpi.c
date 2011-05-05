#include <stdio.h>
#include <mpi.h>
#include "pi.h"

int main(int argc, char** argv)
{
	int size, rank, i;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	
	// How many intervals should we process?
	long numIntervals = 5000;
	if(argc > 1)
		numIntervals = atol(argv[1]);

	double beginTime = MPI_Wtime();

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(rank == 0)
	{
		// Rank 0 is in charge of recombining all the subparts of PI together
		double globalPi = 0.0;
		double bufferPi;

		for(i = 1; i < size; i++)
		{
			MPI_Recv(&bufferPi, 1, MPI_DOUBLE, i, 1, MPI_COMM_WORLD, &status);
			globalPi += bufferPi;
		}

		double endTime = MPI_Wtime();
		MPI_Finalize();

		double totalTime = endTime - beginTime;
		printf("%ld, %f\n", numIntervals, totalTime * 1000000000.0);
		//printf("Ran with %ld intervals\n", numIntervals);
		//printf("Approx PI = %1.50f\n", globalPi);
		//printf("Calculated in %ld seconds\n", endTime - beginTime);
		return globalPi;
	}
	else
	{  
		// Compute a small section of pi
		double localPi;
		double x0 = (rank-1)/(size-1);
		double xn = (rank)/(size-1);

		// We don't want this to computer the number of intervals given in
		// _every_ break up, just a
		localPi = piWithSimpsons(numIntervals / size, x0, xn);
		MPI_Send(&localPi, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Finalize();

		// Indicate we aren't the one with the result
		return 0;
	}
}

