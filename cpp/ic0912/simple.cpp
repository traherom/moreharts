// Author: Ryan Morehart
// Date: Wed Sep 12 09:15:27 EDT 2007
// Purpose: IC0912 - Selection exercises

#include <iostream>
using namespace std;

int main()
{
	// Part 1
	int score;
	cout << "Score: ";
	cin >> score;

	if(score > 100)
	{
		cout << "High\n";
	}
	else
	{
		cout << "Low\n";
	}

	// Part 2
	int exam;
	int programsDone;

	cout << "Exam grade: ";
	cin >> exam;

	cout << "Programs done: ";
	cin >> programsDone;

	if(exam >= 60 && programsDone >= 10)
	{
		cout << "Passed\n";
	}
	else
	{
		cout << "Failed\n";
	}

	// Part 3
	if(0)
		cout << "0 is true\n";
	else
		cout << "0 is false\n";

	if(1)
		cout << "1 is true\n";
	else
		cout << "1 is false\n";

	if(-1)
		cout << "-1 is true\n";
	else
		cout << "-1 is false\n";

	return 0;
}

