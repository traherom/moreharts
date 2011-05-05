// Author: Ryan Morehart
// Date: Mon Nov 5 09:06:04 EST 2007
// Purpose: HW11 - Reverses a setion of a tring of characters

#include <iostream>
#include <iomanip>
#include <cstring>
using namespace std;

void reverse(char str[], int lower, int upper);
void reverseAll(char str[]);

int main()
{
	char str[80] = "";
	int lower = 0;
	int upper = 0;

	// Get 79 (leave room for null) characters from input, up till newline
	cout << ">> ";
	cin.get(str, 79);
	
	// Bounds for reversal (no checks are in place)
	cout << "Reverse from: ";
	cin >> lower;
	
	cout << "To: ";
	cin >> upper;

	// Reverse in bounds and the whole thing
	// We need to make a copy to allow us to do both
	char strCp[80] = "";
	strcpy(strCp, str);
	reverse(strCp, lower, upper);
	cout << "Substring reversed: " << strCp << "\n";

	reverseAll(str);
	cout << "All reversed: " << str << "\n";

	return 0;
}

void reverseAll(char str[])
{
	// Reverse from beginning to end
	reverse(str, 0, strlen(str) - 1);
}

// Swaps the upper and lower characters and passes the string on with
// the bounds moved in one
// Stops recursion when the bounds are either the same or 1 apart
void reverse(char str[], int lower, int upper)
{
	// Is this _not_ the base case? (only 1 or 2 characters)
	if(upper - lower > 1)
	{
		// Call again with even closer bounds
		reverse(str, lower + 1, upper - 1);
	}

	// Swap chars
	char temp = str[upper];
	str[upper] = str[lower];
	str[lower] = temp;
}

