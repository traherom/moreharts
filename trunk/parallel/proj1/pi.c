#include <time.h>
#include <sys/time.h>
#include "pi.h"

double piWithSimpsons(int intervals, double x0, double xn)
{
	// Original code from http://netindonesia.net/blogs/norman/archive/2008/06/13/parallel-implementation-of-simpson-s-rule-for-numerical-integration-to-approximate-the-value-of-pi.aspx
	double h = (1.0 - 0.0) / intervals; // Width of the tops of the parabollas we'll consider
	int i = 0;
	double x = x0;
	double result = 0.0;

	while(x <= xn)
	{
		if(i != 0 | i != intervals - 1)
		{
			if(i % 2 == 0)
			{
				result += 8 / (1 + (x * x));
			}
			else
			{
				result += 16 / (1 + (x * x));
			}
		}
		else
		{
			result += 4 / (1 + (x * x));
		}

		i++;
		x += h;
	}

	result = result / (3 * intervals);

	return result;
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


