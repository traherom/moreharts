#include "util.h"
#include <math.h>

int** generateArray(int randSeed,
                    int seed,
                    int height,
                    int width)
{
    unsigned long i;
    unsigned long j;
    int** matrix;
	
    matrix = calloc (height, sizeof (int*));
    matrix[0] = calloc (height * width, sizeof (int*));
    for (i = 0; i < height; ++i)
    {
        matrix[i] = matrix[0] + i * width;
		
        // Initialze values for the row created
        for (j = 0; j < width; ++j)
        {
            // Set to particular values
            if (seed == 1)
            {
                // Generate random matrix values
                if (randSeed == 1)
                {
                    matrix[i][j] = (rand () * time (NULL)) % 999;
                }
                // Set matrix values for testing purposes
                else
                {
                    matrix[i][j] = i + j;
                }
            }
            // Initialize values to zero
            else if (seed == 0)
            {
                matrix[i][j] = 0;
            }
            else
            {
                fprintf (stderr, "Invalid seed: %d\n", seed);
            }
        }
    }
	
    return matrix;
}

void multiProcMultiply(int id,
                       int processorCount,
                       int** firstArrayPtr,
                       int** secondArrayPtr,
                       int** productArrayPtr,
                       unsigned long dimension,
                       MPI_Status status,
                       unsigned long cacheSize,
                       int length)
{
    int i;
    int sendTo;
    int receiveFrom;
    MPI_Request request;
    MPI_Request sendReq;
    int** bufferArrayPtr;
    int** tempArrayPtr;
    
    sendTo = (id + processorCount - 1) % processorCount;
    receiveFrom = (id + 1) % processorCount;
	
    // Buffer to non-blocking receive into
    bufferArrayPtr = generateArray (0, 0, length, dimension);
		
    // We have processorCount number of rotations we have to do to get all
    // of the intervals in the second array
    for(i = 1; i < processorCount; ++i)
    {
        // Send the second matrix interval we were working on and receive from above
        // into a temporary buffer. We will still be working with our current data
        MPI_Irecv(bufferArrayPtr[0], length * dimension, MPI_INT,
                  receiveFrom, MSG_INTERVAL_ARRAY, MPI_COMM_WORLD,
                  &request);
        MPI_Isend(secondArrayPtr[0], length * dimension, MPI_INT,
                  sendTo, MSG_INTERVAL_ARRAY, MPI_COMM_WORLD, &sendReq);
				 
        // Calculate using the values we currently have
        singleCache(firstArrayPtr, secondArrayPtr, productArrayPtr, cacheSize,
                    0, ((id + i - 1) % processorCount) * length,
                    0, 0, 0, 0, length, length, dimension, 0);
	    
        // Wait for the new values and then swap them into our current working matrix
        MPI_Wait(&request, &status);
        tempArrayPtr = bufferArrayPtr;
        bufferArrayPtr = secondArrayPtr;
        secondArrayPtr = tempArrayPtr;
	}
	
    // Calculate using the last values we received
    singleCache(firstArrayPtr, secondArrayPtr, productArrayPtr, cacheSize,
                0, ((id + processorCount - 1) % processorCount) * length,
                0, 0, 0, 0, length, length, dimension, 0);
}

void printArray(int** array,
                char* message,
                int id,
                unsigned long height,
                unsigned long width)
{
    unsigned long i;
    unsigned long j;
	
    printf ("Proc %d (%ld x %ld): %s\nProc %d: ", id, height, width, message, id);
    for (i = 0; i < height; ++i)
    {
        for (j = 0; j < width; ++j)
        {
            printf ("%d ", array[i][j]);
        }
        printf ("\nProc %d: ", id);
    }
    printf ("END ARRAY\n");
}

void rootCache(int id,
               int processorCount,
               int** firstArrayPtr,
               int** secondArrayPtr,
               int** productArrayPtr,
               unsigned long dimension,
               MPI_Status status,
               unsigned long cacheSize,
               int length)
{
    unsigned long i;
	
    // Send the first matrix interval that a processor is assigned to and the first
    // interval in the second array that it will work with initially
    for (i = 1; i < processorCount; ++i)
    {
        // Matrix 1
        MPI_Send (firstArrayPtr[length * i], length * dimension, MPI_INT, i,
                  MSG_INIT_ARRAY, MPI_COMM_WORLD);
		
		// Matrix 2
		MPI_Send (secondArrayPtr[length * i], length * dimension, MPI_INT, i,
                  MSG_INIT_ARRAY, MPI_COMM_WORLD);
    }
	
    // Do actual calculations
    multiProcMultiply (id, processorCount, firstArrayPtr, secondArrayPtr,
                       productArrayPtr, dimension, status, cacheSize, length);

    // Now receive results from the other processes
    for(i = 1; i < processorCount; ++i)
    {
        MPI_Recv (productArrayPtr[i * length], length * dimension, MPI_INT, i,
                  MSG_PRODUCT_ARRAY, MPI_COMM_WORLD, &status);
    }
}

void singleCache(int** firstArrayPtr,
                 int** secondArrayPtr,
                 int** productArrayPtr,
                 unsigned long cacheSize,
                 unsigned long aRow,
                 unsigned long aCol,
                 unsigned long bRow,
                 unsigned long bCol,
                 unsigned long resultRow,
                 unsigned long resultCol,
                 unsigned long l,
                 unsigned long m,
                 unsigned long n,
                 int depth)
{
    unsigned long i;
    unsigned long j;
    unsigned long k;
	
    unsigned long lHalf[3];
    unsigned long mHalf[3];
    unsigned long nHalf[3];

    if (m * n * sizeof (int) < cacheSize)
    {
        // Do real work. A man's work, like what Dr. G does.
        for (i = 0; i < l; ++i)
        {
            for (j = 0; j < n; ++j)
            {
                for (k = 0; k < m; ++k)
                {
                    productArrayPtr[resultRow + i][resultCol + j] +=
                        (firstArrayPtr[aRow + i][aCol + k] * secondArrayPtr[bRow + k][bCol + j]);
                }
            }
        }
    }
    else
    {
        // Calculate locations and sizes of breaks
        lHalf[0] = 0;
        mHalf[0] = 0;
        nHalf[0] = 0;
		
        lHalf[1] = l/2;
        mHalf[1] = m/2;
        nHalf[1] = n/2;
		
        lHalf[2] = l - lHalf[1];
        mHalf[2] = m - mHalf[1];
        nHalf[2] = n - nHalf[1];
		
        // Break down more
        for(i = 0; i < 2; ++i)
        {
            for(j = 0; j < 2; ++j)
            {
                for(k = 0; k < 2; ++k)
                {
                    singleCache(firstArrayPtr, secondArrayPtr, productArrayPtr, cacheSize,
                                aRow + lHalf[i], aCol + nHalf[k], bRow + mHalf[k],
                                bCol + nHalf[j], resultRow + lHalf[i],
                                resultCol + nHalf[j], lHalf[i + 1], mHalf[k + 1], nHalf[j + 1],
                                depth + 1);
                }
            }
        }
    }
}

void singleNoCache(unsigned long dimension,
                   int** firstArrayPtr,
                   int** secondArrayPtr,
                   int** productArrayPtr)
{
    unsigned long i;
    unsigned long j;
    unsigned long k;
	
    for (i = 0; i < dimension; ++i)
    {
        for (j = 0; j < dimension; ++j)
        {
            for (k = 0; k < dimension; ++k)
            {
                productArrayPtr[i][j] +=
                    (firstArrayPtr[i][k] * secondArrayPtr[k][j]);
            }
        }
    }
}

void workerCache(int id,
                 int processorCount,
                 int** firstArrayPtr,
                 int** secondArrayPtr,
                 int** productArrayPtr,
                 unsigned long dimension,
                 MPI_Status status,
                 unsigned long cacheSize,
                 int length)
{
    // Receive for our interval
    MPI_Recv (firstArrayPtr[0], length * dimension, MPI_INT, 0, MSG_INIT_ARRAY,
              MPI_COMM_WORLD, &status);
    MPI_Recv (secondArrayPtr[0], length * dimension, MPI_INT, 0, MSG_INIT_ARRAY,
              MPI_COMM_WORLD, &status);

    // Do actual calculations
    multiProcMultiply (id, processorCount, firstArrayPtr, secondArrayPtr, productArrayPtr,
                       dimension, status, cacheSize, length);

    // Send the values for our interval out
    MPI_Send (productArrayPtr[0], length * dimension, MPI_INT, 0, MSG_PRODUCT_ARRAY,
              MPI_COMM_WORLD);
}
