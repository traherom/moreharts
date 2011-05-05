// Author: Ryan Morehart
// Date: Mon Oct 22 09:00:55 EDT 2007
// Purpose: HW9 4.7 - Computes gravitational force

#include <iostream>
using namespace std;

// Universal gravitational constant
const double G = 6.637 * .00000001;

double calcGravity(double m1, double m2, double dist);

int main()
{
	double m1;
	double m2;
	double d;
	char runAgain;

	do
	{
		// Get needed info
		cout << "What is the mass (g) of the first object? ";
		cin >> m1;
		cout << "What is the mass (g) of the second object? ";
		cin >> m2;
		cout << "How far apart are they (cm)? ";
		cin >> d;

		// Display grav
		cout << "Gravity is " << calcGravity(m1, m2, d);
		cout << " dynes\n";

		// Run again?
		cout << "Calculate another gravitational force? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');

	return 0;
}

double calcGravity(double m1, double m2, double dist)
{
	return (G * m1 * m2) / (dist * dist);
}

