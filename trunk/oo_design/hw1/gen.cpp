// Author: Ryan Morehart
// Date: Tue Jan 8 16:49:01 EST 2008
// Purpose: HW1 - Generate list of random numbers and write
//	to file

#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
using namespace std;

int randRange(int min, int max);

int main()
{
	int num;
	int min;
	int max;

	// Seed rand
	srand((unsigned int)time(0));

	// Get number of ints and bounds
	cout << "Number of ints: ";
	cin >> num;
	cout << "Minimum value: ";
	cin >> min;
	cout << "Maximum value: ";
	cin >> max;

	// Quick check on values
	if(num < 1)
	{
		cerr << "There's no point if we generate no values, is there?\n";
		return 0;
	}
	if(max <= min)
	{
		cerr << "Maximum must be greater than minimum\n";
		return 0;
	}

	// Where should we write to?
	string outName;
	cout << "Output file: ";
	cin >> outName;

	// Open file for writing
	ofstream numFile(outName.c_str());
	if(numFile.fail())
	{
		cerr << "Unable to open file\n";
		return 1;
	}

	// Put in number of ints we will write
	numFile << num << "\n";

	// List
	for(int i = 0; i < num; i++)
	{
		numFile << randRange(min, max) << "\n";
	}

	// Close file
	numFile.close();

	return 0;
}

int randRange(int min, int max)
{
	return min + (rand() % (max - min + 1));
}
