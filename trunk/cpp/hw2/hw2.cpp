// hw2.cpp - CS1210 second homework program
// Author:      Ryan Morehart
// Date:        25 Aug 2007

#include <iostream>

using namespace std;

int main()
{
	string firstName;
	string lastName;

	// Get the user's name
	cout << "What is your first name: ";
	cin >> firstName;

	cout << "What is your last name: ";
	cin >> lastName;

	// Write out the greeting
	cout << "Hello ";
	cout << firstName << " " << lastName << "\n";

	return 0;
}
