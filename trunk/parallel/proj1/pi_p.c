#include <stdio.h>
#include <mpi.h>
#include <sys/time.h>

int main(int argc, char **argv)
{
	int i = 0;
	double PI = 0.0; double partialPI = 0.0;
	int myRank, numProcs;

	long numIntervals = 5000;
	if(argc > 1)
		numIntervals = atol(argv[1]);

	double width = 1.0 / numIntervals;

	MPI_Init (&argc, &argv);
	
	double beginTime = MPI_Wtime();

	MPI_Comm_rank (MPI_COMM_WORLD, &myRank);
	MPI_Comm_size (MPI_COMM_WORLD, &numProcs); 

	for (i=myRank; i<numIntervals; i+=numProcs)
	{
		double xVal = (i + .5) * width;
		double height = 4.0 / (1.0 + xVal * xVal);
		partialPI += height;
	}
	
	MPI_Reduce(&partialPI, &PI, 1, MPI_DOUBLE,
	MPI_SUM, 0, MPI_COMM_WORLD);
	
	double endTime = MPI_Wtime();
	if (myRank == 0)
	{
		double finalPI = PI * width;
		double totalTime = endTime - beginTime;
		printf("%ld, %f\n", numIntervals, totalTime * 1000000000.0);
		//printf("Ran with %ld intervals\n", numIntervals);
		//printf("Approx PI = %1.50f\n", finalPI);
		//printf("Calculated in %ld seconds\n", endTime - beginTime);
	}
	
	MPI_Finalize();
	
	return (0);
}

