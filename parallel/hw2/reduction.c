/************************************
Parallel Computing
Homework 2, problem 3.7
Author: Ryan Morehart
Purpose: Simulate a gather operation for a given task in
	a parallel system, showing the sends and receives
	that occur.

Example output for all tasks in a 6-task setup:
./reduction 6 0
Simulating task 0 of 6
Step 0: Message received from task 4
Step 1: Message received from task 2
Step 2: Message received from task 1

./reduction 6 1
Simulating task 1 of 6
Step 0: Message received from task 5
Step 1: Message received from task 3
Step 2: Message sent to task 0

./reduction 6 2
Simulating task 2 of 6
Step 0: ---
Step 1: Message sent to task 0

./reduction 6 3
Simulating task 3 of 6
Step 0: ---
Step 1: Message sent to task 1

./reduction 6 4
Simulating task 4 of 6
Step 0: Message sent to task 0

./reduction 6 5
Simulating task 5 of 6
Step 0: Message sent to task 1
************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	int taskCount; // Number of tasks
	int currentTask; // Task # of the task we are simulating (zero-based)
	int testTask; // Task we are sending/receiving to/from
	int timestep; // Current timestep we are looking at
	int maxSteps; // Maximum number of timesteps it will take to simulate
	
	// Get parameters
	if(argc != 3)
	{
		printf("Usage: reduction <task count> <this task>\n");
		exit(0);
	}
	
	taskCount = atoi(argv[1]);
	currentTask = atoi(argv[2]);
	
	if(taskCount < 1)
	{
		printf("There must be at least one task to simulate anything\n");
		exit(0);
	}

	if(currentTask < 0 || taskCount <= currentTask)
	{
		printf("Task to simulate out of range of tasks assigned (must be 0 to %d)\n", taskCount);
		exit(0);
	}
	
	// The algorithm will take lg(n) steps to completely reduce (technically
	// this task might have nothing left to do sooner though)
	maxSteps = (int)ceil(log(taskCount) / log(2));
	
	printf("Simulating task %d, with %d total tasks\n", currentTask, taskCount);
	
	// We flip a single bit in our number with each timestep. We begin working
	// on the maxSteps bit and go down from there (IE, if maxSteps is 3, then
	// we first flip bit 2, then 1, then 0). At each step, if the flipped number
	// is greater AND within the number of tasks that we have, then we would be
	// receiving from them. If less, then we would send to them. We will never send
	// more than once, so we break after we manage to once.
	for(timestep = 0; timestep < maxSteps; timestep++)
	{
		testTask = currentTask ^ (1 << (maxSteps - timestep - 1));
		
		if(testTask < currentTask)
		{
			printf("Step %d: Message sent to task %d\n", timestep, testTask);
			break;
		}
		else if(testTask < taskCount)
		{
			printf("Step %d: Message received from task %d\n", timestep, testTask);
		}
		else
		{
			printf("Step %d: ---\n", timestep);
		}
	}

	// Finish out empty timesteps to show how long the reduce actually takes
	// for those tasks which finished early
	for(++timestep; timestep < maxSteps; timestep++)
		printf("Step %d: ---\n", timestep);
	
	return 0;
}

