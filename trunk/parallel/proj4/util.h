#ifndef UTIL_H
#define UTIL_H

#include "mpi.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef char bool;
#define TRUE 1
#define FALSE 0

#define MAX_OUTPUT_SIZE 50
#define MSG_INIT_ARRAY 4
#define MSG_PRODUCT_ARRAY 8
#define MSG_INTERVAL_ARRAY 16

// Generate an array filled with either 0s (seed=0) or an array filled with random numbers between 0 and 999 (seed=1) and return the matrix
int** generateArray(int randSeed,
                    int seed,
                    int height,
                    int width);

// Handles the common communication for all procs in a multiply
void multiProcMultiply(int id,
                       int processorCount,
                       int** firstArrayPtr,
                       int** secondArrayPtr,
                       int** productArrayPtr,
                       unsigned long dimension,
                       MPI_Status status,
                       unsigned long cacheSize,
                       int length);

// Output a given number of array rows and columns to the console
void printArray(int** array,
                char* message,
                int id,
                unsigned long height,
                unsigned long width);

// The root processor's functionality, using blocking 
void rootCache(int id,
               int processorCount,
               int** firstArrayPtr,
               int** secondArrayPtr,
               int** productArrayPtr,
               unsigned long dimension,
               MPI_Status status,
               unsigned long cacheSize,
               int length);

// Perform an array multiplication using a single processor and cache blocking
void singleCache(int** firstArrayPtr,
                 int** secondArrayPtr, int** productArrayPtr, unsigned long cacheSize,
                 unsigned long aRow,
                 unsigned long aCol,
                 unsigned long bRow,
                 unsigned long bCol,
                 unsigned long resultRow,
                 unsigned long resultCol,
                 unsigned long l,
                 unsigned long m,
                 unsigned long n,
                 int depth);
                
// Perform an array multiplication using only a single processor
void singleNoCache(unsigned long dimension,
                   int** firstArrayPtr,
                   int** secondArrayPtr,
                   int** productArrayPtr);

// A worker processor's functionality, using blocking
void workerCache(int id,
                 int processorCount,
                 int** firstArrayPtr,
                 int** secondArrayPtr,
                 int** productArrayPtr,
                 unsigned long dimension,
                 MPI_Status status,
                 unsigned long cacheSize,
                 int length);

#endif

