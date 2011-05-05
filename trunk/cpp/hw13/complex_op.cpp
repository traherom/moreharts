// Author:  Keith A. Shomper
// Date:    11/17/03
// Purpose: To demonstrate a class
// Modified by: Ryan Morehart
// History: 11/14/07 - Added conjugate and modulus to complex_number

#include <iostream>
#include <cmath>
#include "complex_op.h"

using namespace std;

complex_number operator+ (const complex_number &a, const complex_number &b) {
	complex_number result;

	result.r = a.r + b.r;
	result.i = a.i + b.i;

	return result;
}

complex_number operator- (const complex_number &a, const complex_number &b) {
	complex_number result;

	result.r = a.r - b.r;
	result.i = a.i - b.i;

	return result;
}

complex_number operator* (const complex_number &a, const complex_number &b) {
	complex_number result;

	result.r = (a.r * b.r - a.i * b.i);
	result.i = (a.r * b.i + a.i * b.r);

	return result;
}

complex_number operator/ (const complex_number &a, const complex_number &b) {
	complex_number result;

	result.r = (a.r * b.r + a.i * b.i) / (b.r * b.r + b.i * b.i);
	result.i = (a.i * b.r - a.r * b.i) / (b.r * b.r + b.i * b.i);

	return result;
}

complex_number operator! (const complex_number &a) {
	complex_number result;

	result.r = a.r;
	result.i = -1 * a.i;

	return result;
}

double operator~ (const complex_number &a) {
	return sqrt(a.r * a.r + a.i * a.i);
}

void print (const complex_number &a) {
	cout << a.r << "+" << a.i << "i";
}
