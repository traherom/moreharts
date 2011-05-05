// Author: Ryan Morehart
// Date: Mon Oct 8 09:16:43 EDT 2007
// Purpose: HW8 - Magic Square checker 

#include <iostream>
using namespace std;

class NotMagicException
{
public:
	NotMagicException() {}
};

int main()
{
	// Used for saving square of numbers
	typedef int* intPtr;
	int **square = 0;
	unsigned size = 0;
	
	// Assume it is magic until proven otherwise
	bool isMagic = true;
	
	// The sum of the first thing checked. All other things must match
	// to be magic
	int totalGoal = -1;
	
	// The total of the current thing (row, col, diag)
	int total = 0;
	int row = 0;
	int col = 0;
	
	// Get the size of the square and allocate space
	cout << "What is the order of the square to be tested: ";
	cin >> size;
	try
	{
		square = new intPtr[size];
		for(int i = 0; i < size; i++)
		{
			square[i] = new int[size];
		}
	}
	catch(...)
	{
		cerr << "Out of memory.";
		exit(1);
	}
	
	// Read in square
	cout << "Please enter the " << size * size;
	cout << " values for the magic square in row major order: ";
	for(row = 0; row < size; row++)
	{
		for(col = 0; col < size; col++)
		{
			int num = 0;
			cin >> num;
			square[row][col] = num;
		}
	}

	try
	{
		// Check that all numbers exist
		// Do this by totaling all the numbers in the array, they should total to:
		// (size^2 + 1)(size^2 / 2) -- Yes, that took a little work to figure out
		// Also make sure that no numbers are out of
		// the range valid range for this size: 1 to size^2
		total = 0;
		for(row = 0; row < size; row++)
		{
			for(col = 0; col < size; col++)
			{
				if(square[row][col] < 1 || square[row][col] > size * size)
				{
					throw new NotMagicException();
				}
				
				total += square[row][col];
			}
		}
		
		// The check itself. Note that we use a floating division to handle
		// odd-sized squares (IE, 3x3)
		if(total != (int)((size * size + 1) * (size * size / 2.0)))
		{
			cout << "All numbers not present.\n";
			throw new NotMagicException();
		}
		else
		{
			// Output that all numbers were there because that's what
			// Shomper's does :)
			cout << "All numbers present.\n";
		}

		// Use the total to determine how much each row should add to
		// Do the math if you don't believe this works
		totalGoal = total / size;

		// Check rows
		for(row = 0; row < size; row++)
		{
			total = 0;
			for(col = 0; col < size; col++)
			{
				total += square[row][col];
			}
		
			// Display so the user knows what failed
			cout << "Row " << row << " sum: " << total << "\n";
		
			// Does this row not match the goal?
			if(total != totalGoal)
			{
				throw new NotMagicException();
			}
		}
		
		// Check cols
		for(col = 0; col < size; col++)
		{
			total = 0;
			for(row = 0; row < size; row++)
			{
				total += square[row][col];
			}
		
			// Display so the user knows what failed
			cout << "Column " << col << " sum: " << total << "\n";
				
			// Does this column not match the goal?
			if(total != totalGoal)
			{
				throw new NotMagicException();
			}
		}
		
		// Check diagonals
		total = 0;
		for(int i = 0; i < size; i++)
		{
			total += square[i][i];
		}
		
		// Display so the user knows what failed
		cout << "Left to right diagonal sum: " << total << "\n";
		
		// Does this row not match the goal?
		if(total != totalGoal)
		{
			throw new NotMagicException();
		}
		
		total = 0;
		for(row = 0, col = size - 1; row < size; row++, col--)
		{
			total += square[row][col];
		}
		
		// Display so the user knows what failed
		cout << "Right to left diagonal sum: " << total << "\n";
		
		// Does this row not match the goal?
		if(total != totalGoal)
		{
			throw new NotMagicException();
		}
	}
	catch(NotMagicException *e)
	{
		isMagic = false;
	}
	
	// Display result
	cout << "The square:\n";
	for(int row = 0; row < size; row++)
	{
		for(int col = 0; col < size; col++)
		{
			cout.width(5);
			cout.setf(ios::right);
			cout << square[row][col] << " ";
		}
		cout << "\n";
	}
	cout.setf(ios::left);
	cout.width(0);
	cout << "is" << (isMagic ? " " : " not ") << "a magic square.\n";

	// Delete the array
	for(int i = 0; i < size; i++)
	{
		delete[] square[i];
	}
	delete[] square;

	return 0;
}

