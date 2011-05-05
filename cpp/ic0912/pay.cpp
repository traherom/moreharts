// Author: Ryan Morehart
// Date: Sat Sep 1 11:55:01 EDT 2007
// Purpose: IC0912 P4 (Ex. 7, p. 106) - Rewrite of HW4
//	Calculate the taxes and net pay for a worker

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
	double gross;
	if(hours <= 40)
	{
		// Normal rate
		gross = hourlyPay * hours;
	}
	else
	{
		// Overtime!
		gross = hourlyPay * 40 + hourlyPay * 1.5 * (hours - 40);
	}

	// Get number of dependents
	int dependents;
	cout << "How many dependents do you have? ";
	cin >> dependents;
	
	// Calculate withholdings
	double ssTax = gross * .06;
	double fedIncTax = gross * .14;
	double stateIncTax = gross * .05;
	double unionDues = 10; // Union dues per full week, not patial
	double dependentInsurance = 0;

	if(dependents >= 3)
	{
		dependentInsurance = 35;
	}

	// Calc net pay
	double net = gross - ssTax - fedIncTax - stateIncTax;
	net -= unionDues - dependentInsurance;
	
	// Display
	cout << std::fixed;
	cout.precision(2);
	
	cout << "Gross pay: $" << gross << "\n";
	cout << "Withholdings\n";
	cout << "   S.S.: $" << ssTax << "\n";
	cout << "   Federal: $" << fedIncTax << "\n";
	cout << "   State: $" << stateIncTax << "\n";
	cout << "   Union dues: $" << unionDues << "\n";

	// Only show if there actually was a deduction
	if(dependentInsurance != 0)
	{
		cout << "   Dependents: $" << dependentInsurance << "\n";
	}

	cout << "Net: $" << net << "\n";
	
	return 0;
}

