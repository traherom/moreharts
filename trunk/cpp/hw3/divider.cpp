// Author: Ryan Morehart
// Date: Thu Aug 30 16:47:03 EDT 2007
// Purpose: HW3 (Ex. 18, p. 73) - Divides two numbers, showing the remainder

#include <iostream>
using namespace std;

int main()
{
	int divisor; // Denominator
	int dividend; // Numerator

	// Get user stuff
	cout << "Divide: ";
	cin >> dividend;

	cout << "By: ";
	cin >> divisor;

	// Display answer like "9 / 4 = 2 R1"
	cout << dividend << " / " << divisor << " = ";
	cout << dividend / divisor << " R" << dividend % divisor << "\n";

	return 0;
}

