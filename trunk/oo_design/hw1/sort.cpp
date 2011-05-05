// Author: Ryan Morehart
// Date: Tue Jan 8 16:49:01 EST 2008
// Purpose: HW1 - Read in list of numbers from file and sort

#include <iostream>
#include <fstream>
#include <list>
#include <string>
using namespace std;

int main()
{
	int cnt;
	list<int> nums;
	int sum = 0;

	// Get file we should write the sorted values to
	string outName;
	cout << "Where would you like the sorted numbers saved? ";
	cin >> outName;
	
	// Where should we read from?
	string inName;
	cout << "Where should we read from? ";
	cin >> inName;

	// Open file for reading
	ifstream numFile(inName.c_str());
	if(numFile.fail())
	{
		cerr << "Unable to open input file\n";
		return 1;
	}

	// Open file for writing
	ofstream sortedFile(outName.c_str());
	if(sortedFile.fail())
	{
		cerr << "Unable to open " << outName << " for writing\n";
		return 1;
	}

	// Get number of ints
	numFile >> cnt;

	// Read in each, totaling as we go
	for(int i = 0; i < cnt; i++)
	{
		int newNum;
		numFile >> newNum;
		nums.push_back(newNum);
		sum += newNum;
	}

	// Display mean
	cout << "Mean: " << (float)sum / cnt << "\n";

	// Sort
	nums.sort();

	// Write out
	while(nums.size() > 0)
	{
		sortedFile << nums.front() << "\n";
		nums.pop_front();
	}

	// Close files
	numFile.close();
	sortedFile.close();

	return 0;
}

