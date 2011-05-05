// Author: Ryan Morehart
// Date: Sat Sep 1 11:55:01 EDT 2007
// Purpose: HW4 (Ex. 7, p. 106) - Calculate the taxes and net pay for a worker

#include <iostream>
#include <iomanip>
using namespace std;

int main()
{
	const double hourlyPay = 16.78;

	// Get hours worked
	double hours;
	cout << "How many hours did you work? ";
	cin >> hours;

	// Calculate gross pay
	double gross = hourlyPay * hours;
	
	// Calculate withholdings
	double ssTax = gross * .06;
	double fedIncTax = gross * .14;
	double stateIncTax = gross * .05;
	double unionDues = 10; // Union dues per full week
	
	// Calc net pay
	double net = gross - ssTax - fedIncTax - stateIncTax - unionDues;
	
	// Display
	cout << std::fixed;
	cout.precision(2);
	
	cout << "Gross pay: $" << gross << "\n";
	cout << "Withholdings\n";
	cout << "   S.S.: $" << ssTax << "\n";
	cout << "   Federal: $" << fedIncTax << "\n";
	cout << "   State: $" << stateIncTax << "\n";
	cout << "   Union dues: $" << unionDues << "\n";
	cout << "Net: $" << net << "\n";
	
	return 0;
}

