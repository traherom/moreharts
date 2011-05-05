/**
 * Alex Laird
 * Homework #2
 * Problem #3.7
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main(int argc, char **argv)
{
    // good old C, making us declare variables up front
	int count;
	int max;
	int result;
	int step;
	int task;
	
	// ensure all necessary arguments are present
	if (argc == 3)
	{
	    // fill variables and calculate max
        count = atoi (argv[1]);
        task = atoi (argv[2]);
        max = (int) ceil (log (count) / log (2));
	        
	    // ensure arguments received are valid
	    if (count >= 1 &&
	        task >= 0 &&
	        count > task)
	    {
	        // loop through all steps
	        for(step = 0; step < max; ++step)
            {
                // calculate the result for the current step
                result = task ^ (1 << (max - step - 1));
                
                // if the result is less than the current task, we're sending and we're done
                if(result < task)
                {
                    printf("Message sent to task %d\n", result);
                    break;
                }
                // if the result is less than the count, we're receiving
                else if(result < count)
                {
                    printf("Message received from task %d\n", result);
                }
            }
	    }
	}
	
	return 0;
}

