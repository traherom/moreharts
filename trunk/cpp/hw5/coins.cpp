// Author: Ryan Morehart
// Date: Wed Sep 12 18:17:31 EDT 2007
// Purpose: HW5 - Calculate the minimum bills/coins needed to add up to
//	a particular dollar amount

#include <iostream>
using namespace std;

int main()
{
	// Set up arrays which will be looped through in calculations
	// The first is just the dollar amounts of each type of bill/coin
	// The second (tenderUsed) is where we store the actual count
	const int tenderTypeCount = 10;
	const double tenderType[tenderTypeCount] = {
		100, 50, 20, 10, 5, 1, .25, .10, .05, .01
		};
	int tenderUsed[tenderTypeCount];

	// Get the amount we are trying to add to from the user
	double amount;
	cout << "Amount? $";
	cin >> amount;

	// Header
	cout << "For $" << amount << ":\n";

	// Move through each tenderType and take as much out of the given
	// amount as possible
	for(int i = 0; i < tenderTypeCount; i++)
	{
		// How many times does it go in?
		// The .001 is added to get around floating point inaccuracies
		// It's a hack I'll grant you, but it works and is far simpler
		// than finding the whole and decimal parts and doing each seperately.
		// The other simple solution would have been to multiply everthing by
		// 100, but it's too late now :)
		tenderUsed[i] = (int)(amount / tenderType[i] + .001);

		// Remove that much from the remaing amount
		amount -= tenderUsed[i] * tenderType[i];
	}

	// Display results
	// This could have been combined with the previous loop, but...
	// well, I just didn't feel like it. I guess it would have saved me
	// the second array.
	int totalUsed = 0;
	for(int i = 0; i < tenderTypeCount; i++)
	{
		// Don't show a type if we didn't use any
		if(tenderUsed[i] > 0)
		{
			totalUsed += tenderUsed[i];
			cout << "   # of $" << tenderType[i] << " used = ";
			cout << tenderUsed[i] << "\n";
		}
	}

	cout << "Minimum bills/coins required = " << totalUsed << "\n";

	return 0;
}

