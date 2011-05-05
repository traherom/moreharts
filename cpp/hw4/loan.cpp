// Author: Ryan Morehart
// Date: Sat Sep 1 09:46:14 EDT 2007
// Purpose: HW4 (Ex. 5, p. 105) - Calculates the needed loan size
//	to get the desired amount of monthly and tells what the payment
//	will be for that face value.

#include <iostream>
using namespace std;

int main()
{
	// Get amount needed for loan
	double amountNeeded;
	cout << "How much money do you need? $";
	cin >> amountNeeded;

	// Get the interest rate from the user as 15.4% and
	// conver to .154
	double rate;
	cout << "What is the interest rate? ";
	cin >> rate;

	rate /= 100;

	// Get how long the user will have this loan
	double time;
	cout << "How many months will you have this loan? ";
	cin >> time;

	// Calculate face value
	// Note we divide the time by 12 because we need the time in years
	double faceValue;
	faceValue = amountNeeded / (1 - rate * (time / 12));

	// Calculate monthly payment
	double payment;
	payment = faceValue / time;

	// Tell consumer, display only cents, not fractions	
	cout << std::fixed;
	cout.precision(2);
	
	cout << "You will need to borrow $" << faceValue << ", which means you will pay $";
	cout << payment << " per month.\n";

	return 0;
}
