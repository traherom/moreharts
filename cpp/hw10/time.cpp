// Author: Ryan Morehart
// Date: Mon Oct 29 09:01:01 EDT 2007
// Purpose: HW10 p5.1 - 24 to 12 hour time conversion

#include <iostream>
#include <cstdlib>
#include <iomanip>
using namespace std;

void get24HourTime(int &hours, int &minutes);
void convertTime(int &hours, int &minutes, bool &isPM);
void display12HourTime(int &hours, int &minutes, bool &isPM);

int main()
{
	char runAgain;
	int hours;
	int minutes;
	bool isPM;

	do
	{
		get24HourTime(hours, minutes);
		convertTime(hours, minutes, isPM);
		display12HourTime(hours, minutes, isPM);

		// Run again?
		cout << "Run again? (y/n) ";
		cin >> runAgain;
	}
	while(runAgain == 'y' || runAgain == 'Y');

	return 0;
}

void get24HourTime(int &hours, int &minutes)
{
	string time;

	// Read in time as string then break apart
	cout << "What is the 24 hour time? ";
	cin >> time;

	int colPlace = time.find(":");
	hours = atoi(time.substr(0, colPlace).c_str());
	minutes = atoi(time.substr(colPlace + 1).c_str());
}

void convertTime(int &hours, int &minutes, bool &isPM)
{
	// Check AM/PM
	isPM = (hours >= 12);

	// Move down hours if 13:00 or later
	if(hours > 12)
	{
		hours -= 12;
	}
}

void display12HourTime(int &hours, int &minutes, bool &isPM)
{
	cout << "12 hour time is " << hours << ":";
	cout << setw(2) << setfill('0') << minutes;
	cout << " " << (isPM ? "PM" : "AM") << "\n";
}

