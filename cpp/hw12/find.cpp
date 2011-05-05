// Author: Ryan Morehart
// Date: Wed Nov 7 09:10:59 EST 2007
// Purpose: HW12 p2 - Finds all occurences of a character in a string

#include <iostream>
#include <string>
using namespace std;

int *find(string str, char toFind);

int main()
{
	string str;
	char c;

	// Get string to search and the character to find
	cout << "String to search: ";
	getline(cin, str);

	cout << "Character to find: ";
	cin >> c;

	int *occ = find(str, c);

	// Display occurences
	int i = 0;
	while(occ[i] != -1)
	{
		cout << occ[i] << "\n";
		i++;
	}

	// Just to be clean delete array
	delete[] occ;
	occ = 0;

	return 0;
}

// Str is the str to search
// toFind is the character to find
int *find(string str, char toFind)
{
	// The most times the character could occur is the size of
	// the string (IE, every character in str is toFind). We
	// need an extra element for -1
	int *occurences = new int[str.length() + 1];
	int hits = 0;

	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] == toFind)
		{
			// Yeah, a hit, save it
			occurences[hits] = i;
			hits++;
		}
	}

	// Mark the end with -1
	occurences[hits] = -1;

	return occurences;
}

