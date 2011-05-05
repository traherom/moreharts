// Author:  Keith A. Shomper
// Date:    2/2/06
// Purpose: To demonstrate using a well-defined class

#include <iostream>
#include "complex.h"

using namespace std;

void printMe(complex_number p);

int main () {
	complex_number x(1, 2), y, z;
	complex_number sum, difference, product, quotient;

	y = "2+4i";

	z = 3;

	cout << "x is:  " << x << endl;
	cout << "y is:  " << y << endl;
	cout << "z is:  " << z << endl;

	sum        = x + y;
	difference = x - y;
	product    = x * y;
	quotient   = sum / z;
	
	cout << "The sum (x + y) is:  " << sum << endl;
	cout << "The difference (x - y) is:  " << difference<< endl;
	cout << "The product (x * y) is:  " << product << endl;
	cout << "The quotient (sum / z) is:  " << quotient << endl;

	cout << "Adding x with a real constant 2.5 is "; 
	printMe(x + 2.5);
	cout << endl;

	cout << "Please enter a value for x:  ";
	cin >> x;

	cout << "The value you entered is:  " << x << endl;

	return 0;
}

void printMe(complex_number p) {
	cout << "printing from printMe():  " << p << endl;
}

