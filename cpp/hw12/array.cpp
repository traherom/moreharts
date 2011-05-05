// Author: Ryan Morehart
// Date: Wed Nov 7 08:35:33 EST 2007
// Purpose: HW12 p1 - Dynamic allocation practice

#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;

int main()
{
	int size;
	int *array = 0;

	// Seed
	srand(time(0));

	// Ask for size
	cout << "How many elements do you want in the array? ";
	cin >> size;

	// Allocate and assign
	array = new int[size];
	for(int i = 0; i < size; i++)
	{
		array[i] = rand() % 100 + 1;
	}

	// Display with newlines every 20 elements
	for(int i = 0; i < size; i++)
	{
		cout << setw(4) << array[i];

		if((i + 1) % 20 == 0)
		{
			cout << "\n";
		}
	}

	cout << "\n";

	// Free array
	delete[] array;
	array = 0;

	return 0;
}

