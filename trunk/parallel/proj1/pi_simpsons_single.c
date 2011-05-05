#include <stdio.h>
#include "pi.h"

int main(int argc, char **argv)
{
	// How many intervals should we process?
	long numIntervals = 5000;
	if(argc > 1)
		numIntervals = atol(argv[1]);

	// Compute a small section of pi
	double pi;

	// Start timing
	struct timespec begTime;
	clock_gettime(CLOCK_REALTIME, &begTime);

	// We don't want this to computer the number of intervals given in
	// _every_ break up, just a
	pi = piWithSimpsons(numIntervals, 0.0, 1.0);

	// End timing
	struct timespec endTime;
	clock_gettime(CLOCK_REALTIME, &endTime);
	
	struct timespec totalTime;
	totalTime = timeTaken(begTime, endTime);

	printf("%ld, %f\n", numIntervals, (double)totalTime.tv_sec * 1000000000.0 + (double)totalTime.tv_nsec);
	//printf("Ran with %ld intervals\n", numIntervals);
	//printf("Approx PI = %1.50f\n", pi);
	//printf("Calculated in %d seconds, %ld nanoseconds\n", (int)totalTime.tv_sec, totalTime.tv_nsec);

	// Indicate we aren't the one with the result
	return 0;
}

