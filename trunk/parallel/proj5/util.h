#ifndef UTIL_H
#define UTIL_H

typedef char bool;
#define TRUE 1
#define FALSE 0

void printUnmarked(bool *array, int length);
unsigned long getTimeNanoseconds();

unsigned long calculateRangeBegin(int threadID, int threadCount, unsigned long zeroRangeEnd, unsigned long maxNum);
unsigned long calculateRangeEnd(int threadID, int threadCount, unsigned long zeroRangeEnd, unsigned long maxNum);

#endif

