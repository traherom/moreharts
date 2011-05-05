// Author: Ryan Morehart
// Date: Mon Oct 1 08:58:46 EDT 2007
// Purpose: HW7 - Sequence game where the user
//	must follow the pattern we create. 

#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

int main()
{
	const int ROUNDS = 20;
	const char VALID_LETTERS[4] = {'R', 'G', 'B', 'Y'};

	// Seed rand
	srand(time(0));

	// Sequence of characters
	char sequence[ROUNDS];

	for(int round = 0; round < ROUNDS; round++)
	{
		// Add random char
		sequence[round] = VALID_LETTERS[rand() % 4];

		// Clear screen and display sequence one character
		// at a time.
		cout << "\014Simon says: ";
		cout.flush();
		for(int i = 0; i <= round; i++)
		{
			cout << sequence[i];
			cout.flush();
			sleep(1);
		}

		// Erase
		for(int i = 0; i <= round; i++)
		{
			cout << "\010";
		}
		for(int i = 0; i <= round; i++)
		{
			cout << ".";
		}

		// Get user's try
		cout << "\nPlease enter " << round + 1;
		cout << " characters to match: ";

		for(int i = 0; i < round + 1; i++)
		{
			char chIn;
			cin >> chIn;
			if(chIn != sequence[i])
			{
				cout << "Sorry, that wasn't quite right.\n";
				cout << "The correct sequence was ";
				for(int j = 0; j <= round; j++)
				{
					cout << sequence[j];
				}
				cout << "\n";
				exit(0);
			}
		}
	}

	// They won, amazing
	cout << "\nCongratulation! You've beaten Simon.\n";

	return 0;
}

