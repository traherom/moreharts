// Author:  Keith A. Shomper
// Date:    11/17/03
// Purpose: To deminstrate classes and attribute access
// Modified by: Ryan Morehart
// History: 11/14/07 - Added test for conjugate and modulus of complex_number

#include <iostream>
#include "complex_op.h"

using namespace std;

int main () {
	complex_number x(1, 2), y, z;
	complex_number sum, difference, product, quotient, conjugate;
	double modulus;

	y = complex_number(2, 4);

	z = complex_number(3, 0);

	cout << "x is:  "; print(x); cout << endl;
	cout << "y is:  "; print(y); cout << endl;
	cout << "z is:  "; print(z); cout << endl;

	sum        = x + y;
	difference = x - y;
	product    = x * y;
	quotient   = sum / z;
	conjugate  = !x;
	modulus    = ~x;
	
	cout << "The sum (x + y) is:  "; print(sum); cout << endl;
	cout << "The difference (x - y) is:  "; print(difference); cout << endl;
	cout << "The product (x * y) is:  "; print(x * y); cout << endl;
	cout << "The quotient (sum / z) is:  "; print(quotient); cout << endl;
	cout << "The conjugate (x) is: "; print(conjugate); cout << endl;
	cout << "The modulus (x) is: " << modulus << endl;


	return 0;
}
