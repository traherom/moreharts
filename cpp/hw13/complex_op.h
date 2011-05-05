// Author:  Keith A. Shomper
// Date:    11/17/03
// Purpose: To demonstrate a class
// Modified by: Ryan Morehart
// History: 11/14/07 - Added conjugate and modulus to complex_number

class complex_number {
 public:
   complex_number(float real = 0.0, float imag = 0.0) {r = real; i = imag;}
	float r;
	float i;
};

complex_number operator+ (const complex_number &a, const complex_number &b);
complex_number operator- (const complex_number &a, const complex_number &b);
complex_number operator* (const complex_number &a, const complex_number &b);
complex_number operator/ (const complex_number &a, const complex_number &b);
complex_number operator! (const complex_number &a);
double operator~ (const complex_number &a);

void           print     (const complex_number &a);
