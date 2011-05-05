// Author:  Keith A. Shomper
// Date:    2/2/06
// Purpose: To demonstrate a well-defined class
// Modified by: Ryan Morehart

#include <string>
#include <iostream>
using namespace std;

#include "complex.h"

complex_number::complex_number(const char *cStr)
{
	// I really wish that there was a default regex library for C++...

	string num = cStr;
	string rPart; // Temporary real part
	string iPart; // Temporary imaginary part
	
	// Add a positive sign at the beginning if there isn't a sign already
	if(num[0] != '-' && num[0] != '+')
	{
		num.insert(0, "+");
	}
	
	// Break into two halves/if there is only one piece decide what it is
	int part2Beg = num.find_first_of("+-", 1);
	if(part2Beg != string::npos)
	{
		// Two parts
		rPart = num.substr(0, part2Beg);
		iPart = num.substr(part2Beg);
	}
	else
	{
		// There's only one part, which one is it?
		if(num[num.length() - 1] == 'i')
		{
			// Imaginary
			iPart = num;
		}
		else
		{
			// Real
			rPart = num;
		}
	}
	
	// Handle i with no numerical part, meaning it has an implicit 1
	if(iPart.length() == 2)
	{
		iPart = iPart.insert(1, "1");
	}
	
	// Convert to numbers
	r = atof(rPart.c_str());
	i = atof(iPart.substr(0, iPart.length() - 1).c_str());
}

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

ostream& operator<< (ostream &out, const complex_number &b) {
	out << b.r << "+" << b.i << "i";
	return out;
}

istream& operator>> (istream &in, complex_number &b) {
	string str;
	in >> str;
	
	b = str.c_str();
	
	return in;
}

