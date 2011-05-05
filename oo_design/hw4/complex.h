// Author:  Keith A. Shomper
// Date:    2/2/06
// Purpose: To demonstrate a well-defined C++ class
// Modified by: Ryan Morehart

#include <iostream>

using std::ostream;
using std::istream;

class complex_number {
 public:
   complex_number(float real = 0.0, float imag = 0.0) {r = real; i = imag;}
   complex_number(const char *cStr);

   friend complex_number operator+ (const complex_number &a, const complex_number &b);
   friend complex_number operator- (const complex_number &a, const complex_number &b);
   friend complex_number operator* (const complex_number &a, const complex_number &b);
   friend complex_number operator/ (const complex_number &a, const complex_number &b);

   friend ostream&       operator<<(ostream &out,            const complex_number &b);
   friend istream&       operator>>(istream &in,             complex_number &b);

 private:
	float r;
	float i;
};




