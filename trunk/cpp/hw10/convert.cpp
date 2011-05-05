// Author: Ryan Morehart
// Date: Mon Oct 29 09:01:01 EDT 2007
// Purpose: HW10 5.6 - Converts ft/in to m/cm

#include <iostream>
using namespace std;

void getEnglish(int &inches);
void convertToMetric(int in, int &cm);
void displayMetric(int cm);

int main()
{
	char runAgain;

	int inches;
	int centimeters;
	
	do
	{
		getEnglish(inches);
		convertToMetric(inches, centimeters);
		displayMetric(centimeters);
	
		cout << "Run again? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');
	
	return 0;
}

// Get number of inches (convert feet to inches)
void getEnglish(int &inches)
{
	int feet;
	int in;
	
	cout << "Feet? ";
	cin >> feet;
	
	cout << "Inches? ";
	cin >> in;
	
	inches = feet * 12 + in;
}

void convertToMetric(int in, int &cm)
{
	// Note we keep only whole cms
	cm = (int)(in * 2.54);
}

void displayMetric(int cm)
{
	int m;
	
	// Cut out full meters
	m = cm / 100;
	cm -= m * 100;
	
	// Display
	cout << m << " meters, " << cm << " cm\n";
}


