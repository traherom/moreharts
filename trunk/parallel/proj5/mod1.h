#ifndef NO_CHANGES_H
#define NO_CHANGES_H

#include "util.h"

unsigned long getPrimeCountNoEvens(unsigned long maxNum);
void seiveRange(bool *marked, unsigned long zeroRangeEnd, unsigned long rangeBegin, unsigned long rangeEnd, unsigned long rangeBeginIndex);

#endif

