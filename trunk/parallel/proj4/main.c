#include "util.h"

int main(int argc,
         char** argv)
{
    // MPI Stuff
    MPI_Status status;
    int processorCount;
    int id;
    
    // Define pointers to the arrays that are to be multiplied
    int** firstArrayPtr;
    int** secondArrayPtr;
    int** productArrayPtr;
    
    // Settings
    unsigned long dimension;
    unsigned long cacheSize;
    int randSeed;
    int blocking;

    // Matrices
    int length;
	
    // Results
    double elapsedTime;
	
    // Initialize cache size
    cacheSize = -1;
	
    // Make MPI strip off any special args the user gave it on the command line
    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &processorCount);
	
    // Parse through arguments and set their respective variables
    if (argc >= 4)
    {
        dimension = atol (argv[1]);
        randSeed = atoi (argv[2]);
        blocking = atoi (argv[3]);
        if (argc == 5)
        {
            cacheSize = atoi (argv[4]);
        }
        
        if (randSeed != 0 && randSeed != 1)
        {
            fprintf (stderr, "The randSeed given must be 0 (seed for standard matrix) or\n1 (seed for random matrix).\nGiven: randSeed=%d\n", randSeed);
            MPI_Finalize ();
            exit (2);
        }
        if (blocking != 0 && blocking != 1)
        {
            if (randSeed != 0 && randSeed != 1)
            {
                fprintf (stderr, "The blocking given must be 0 (no blocking in parallel) or\n1 (use blocking in parallel).\nGiven: blocking=%d\n", blocking);
                MPI_Finalize ();
                exit (3);
            }
        }
        if(dimension % processorCount != 0)
        {
            fprintf (stderr, "The dimensions of the matrix must be a multiple of the number of processors being used\nProcessors: %d\nDimension: %ld\n", processorCount, dimension);
            MPI_Finalize ();
            exit (4);
        }
        if(blocking && cacheSize < 4)
        {
            fprintf (stderr, "A cache size greater than 4 bytes must be specified.\n");
            MPI_Finalize ();
            exit (5);
        }
    }
    else
    {
        if (id == 0)
        {
            fprintf (stderr, "Usage: %s <dimension> <seed> <blocking> <cache size>\n", argv[0]);
            fprintf (stderr, "Only square-matrices are allowed.\n");
            fprintf (stderr, "Dimension is a single integer.\n");
            fprintf (stderr, "Blocking may be 1 to use cache blocking or 0 to not\n");
            fprintf (stderr, "Cache size should be the size of the processor cache in bytes.\n");
        }
        fprintf (stderr, "argc: %d\n", argc);
        MPI_Finalize ();
        exit (1);
    }
	
    // Determine the length of each interval and how much remainder we will have
    // that the final processor needs to cover
    length = dimension / processorCount;

    if (id == 0)
    {
        // Allocate the arrays in memory
        firstArrayPtr  = generateArray (randSeed, 1, dimension, dimension);
        secondArrayPtr = generateArray (randSeed, 1, dimension, dimension);
        productArrayPtr = generateArray (randSeed, 0, dimension, dimension);

        // For verification purposes, we need to output, but for the sake of our output not being ridiculous,
        // we're only going to output if it's smaller than the MAX_OUTPUT_SIZE of a matrix
        if (dimension < MAX_OUTPUT_SIZE)
        {
            // Output initial arrays
            printArray (firstArrayPtr, "First Matrix:", id, dimension, dimension);
            printArray (secondArrayPtr, "Second Matrix:", id, dimension, dimension);
        }
    }
	
    // Start timer after everyone initializes.
    MPI_Barrier (MPI_COMM_WORLD);
	
    if (id == 0)
    {
        elapsedTime = -MPI_Wtime();
        
        // If this operation is only being perform on a single core
        if (blocking == 0 && processorCount == 1)
        {
            singleNoCache (dimension, firstArrayPtr, secondArrayPtr, productArrayPtr);
        }
        // Multiple processors are being used and we're blocking
        else if (processorCount == 1)
        {
            singleCache (firstArrayPtr, secondArrayPtr, productArrayPtr, cacheSize, 0, 0, 0, 0, 0, 0, dimension, dimension, dimension, 0);
        }
        // Multiple processors are being used and we're blocking
        else
        {
            rootCache (id, processorCount, firstArrayPtr, secondArrayPtr, productArrayPtr, dimension, status, cacheSize, length);
        }
		
        // Get time spent to do work
        elapsedTime += MPI_Wtime();

        // Output the completed matrix if the dimensions are less than 50x50
        if (dimension < MAX_OUTPUT_SIZE)
        {
            printArray (productArrayPtr, "Product Matrix:", id, dimension, dimension);
        }        

        // Output benchmark results
        printf ("Total elapsed time: %10.15f\n", elapsedTime);
        printf ("Processors used: %d\n", processorCount);
    }
    else
    {
        // Allocate empty array space in memory
        firstArrayPtr  = generateArray (randSeed, 0, length, dimension);
        secondArrayPtr = generateArray (randSeed, 0, length, dimension);
        productArrayPtr = generateArray (randSeed, 0, length, dimension);

        workerCache (id, processorCount, firstArrayPtr, secondArrayPtr, productArrayPtr, dimension, status, cacheSize, length);
    }
	
    MPI_Finalize();
    return 0;
}

