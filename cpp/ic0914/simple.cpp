// Author: Ryan Morehart
// Date: Wed Sep 12 09:43:00 EDT 2007
// Purpose: IC0914 P1-3 - Looping exercises

#include <iostream>
using namespace std;

int main()
{
	// Part 1
	int x = 10;
	while(x > 0)
	{
		cout << x << "\n";
		x -= 3;
	}

	// Part 2
	x = 10;
	while(x < 0)
	{
		// This will never run
	}

	// Part 3
	x = -42;
	do
	{
		cout << x << "\n";
		x -= 3;
	} while(x > 0);

	return 0;
}

