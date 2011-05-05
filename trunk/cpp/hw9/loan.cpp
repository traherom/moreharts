// Author: Ryan Morehart
// Date: Mon Oct 22 09:00:55 EDT 2007
// Purpose: HW9 4.8 - Computes after-tax costn of house

#include <iostream>
using namespace std;

double cost(double price, double down);
double initialBalance(double price, double down);
double taxSavings(double price, double down);
double annualPayment(double price, double down);
double interestPayment(double price, double down);

int main()
{
	char runAgain;
	double housePrice;
	double downPayment;

	do
	{
		// Get house cost and payment
		cout << "House price? ";
		cin >> housePrice;
		cout << "Down payment? ";
		cin >> downPayment;

		// Calculate
		cout << "Annual after-tax cost will be $";
		cout << cost(housePrice, downPayment) << "\n";

		// Run again?
		cout << "Run again? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');

	return 0;
}

double cost(double price, double down)
{
	return annualPayment(price, down) - taxSavings(price, down);
}

double annualPayment(double price, double down)
{
	return .03 * initialBalance(price, down)
		+ interestPayment(price, down);
}

double taxSavings(double price, double down)
{
	return .35 * interestPayment(price, down);
}

double interestPayment(double price, double down)
{
	return .06 * initialBalance(price, down);
}

double initialBalance(double price, double down)
{
	return price - down;
}

