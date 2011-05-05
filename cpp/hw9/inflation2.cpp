// Author: Ryan Morehart
// Date: Mon Oct 22 09:00:55 EDT 2007
// Purpose: HW9 4.5 -  Guages rate of inflation for the past year
//	and guesses the price over the next few years

#include <iostream>
using namespace std;

double calcInflation(double oldCost, double newCost);
double estimateCost(double currPrice, double inflation);

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
		double inflation = calcInflation(costYearAgo, costToday);
		cout << "Inflation of " << inflation << "%\n";

		// Tell cost in a few years
		double costFuture;
		costFuture = estimateCost(costToday, inflation);
		cout << "In 1 year a hamburger will cost " << costFuture;
		cout << ".\nIn 2 years it will cost ";
		cout << estimateCost(costFuture, inflation) << ".\n";

		// Run again?
		cout << "Calculate another inflation rate? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');

	return 0;
}

double calcInflation(double oldCost, double newCost)
{
	return (newCost - oldCost) / oldCost * 100;
}

double estimateCost(double currPrice, double inflation)
{
	// We have to convert the inflattion to real percentage
	return currPrice * (inflation / 100 + 1);
}

