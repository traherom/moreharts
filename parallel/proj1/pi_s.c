#include <stdio.h>
#include <time.h>
#include <sys/time.h>

struct timespec timeTaken(struct timespec start,  struct timespec end);

int main(int argc, char **argv)
{
	double partialPI = 0.0;
	long i = 0;
	
	// How many intervals should we process at?
	long numIntervals = 5000;
	if(argc > 1)
		numIntervals = atol(argv[1]);
	
	// Width of each rectangl
	double width = 1.0 / numIntervals;

	// Start timing
	struct timespec begTime;
	clock_gettime(CLOCK_REALTIME, &begTime);
	
	for(i = 0; i < numIntervals; i++)
	{
		double xVal = (i + .5) * width;
		double height = 4.0 / (1.0+xVal*xVal);
		partialPI += height;
	}

	double pi = partialPI * width;
	
	// End timing. Wanted to include final multiply as it could potentially fall in the loop
	// if we were dumb.
	struct timespec endTime;
	clock_gettime(CLOCK_REALTIME, &endTime);
	
	struct timespec totalTime;
	totalTime = timeTaken(begTime, endTime);
	
	printf("%ld, %f\n", numIntervals, (double)totalTime.tv_sec * 1000000000.0 + (double)totalTime.tv_nsec);
	//printf("Ran with %ld intervals\n", numIntervals);
	//printf("Approx PI = %1.50f\n", pi);
	//printf("Calculated in %d seconds, %ld nanoseconds\n", (int)totalTime.tv_sec, totalTime.tv_nsec);
	
	return 0;
}

// Taken from http://www.guyrutenberg.com/2007/09/22/profiling-code-using-clock_gettime/
struct timespec timeTaken(struct timespec start,  struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0)
	{
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	}
	else
	{
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

