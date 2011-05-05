#include <time.h>
#include <sys/time.h>

#ifndef PI_H
#define PI_H

double piWithSimpsons(int intervals, double x0, double xn);
struct timespec timeTaken(struct timespec start,  struct timespec end);

#endif

