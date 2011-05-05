// Author: Ryan Morehart
// Date: Thu Aug 30 16:36:27 EDT 2007
// Purpose: HW3 (Ex. 14, p. 59) - displays small square root table

#include <iostream>
using namespace std;

int main()
{
	// An array would be way nicer, but following what I "know:"
	double one = 1.0;
	double two = 1.414;
	double three = 1.732;
	double four = 2;
	double five = 2.236;

	// I swear this is the dumbest way to print a table ever
	cout << "N\t\tSquare Root\n";
	cout << "1\t\t" << one << "\n";
	cout << "2\t\t" << two << "\n";
	cout << "3\t\t" << three << "\n";
	cout << "4\t\t" << four << "\n";
	cout << "5\t\t" << five << "\n";

	return 0;
}

