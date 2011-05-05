// Author: Ryan Morehart
// Date: Mon Oct 22 09:00:55 EDT 2007
// Purpose: HW9 4.4 -  Guages rate of inflation for the past year

#include <iostream>
using namespace std;

double calcInflation(double oldCost, double newCost);

int main()
{
	double costYearAgo;
	double costToday;
	char runAgain;

	do
	{
		// Get costs
		cout << "What was the cost of a hamburger a year ago? ";
		cin >> costYearAgo;
		cout << "What does a hamburger cost today? ";
		cin >> costToday;

		// Tell inflation
		cout << "Inflation of " << calcInflation(costYearAgo,
			costToday);
		cout << "%\n";

		// Run again?
		cout << "Calculate another inflation? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');

	return 0;
}

double calcInflation(double oldCost, double newCost)
{
	return (newCost - oldCost) / oldCost * 100;
}

