// Author: Ryan Morehart
// Date: Thu Jan 10 08:29:30 EST 2008
// Purpose: Plays the game red-yellow-green 

#include <iostream>
#include <cstdlib>
#include <sstream>
using namespace std;

int main()
{
	bool hasWon = false;
	
	// Get random number
	srand(time(0));
	int sNum = rand() % 1000;
	
	// Convert to left-padded (with 0) string
	string sStr;
	stringstream out;
	out << sNum;
	sStr = out.str();
	while(sStr.length() < 3)
	{
		sStr = "0" + sStr;
	}

	//cout << "Secret: " <<  sStr << "\n";

	// Declare/init array to make sure we don't reuse a secret char
	// for multiple guess characters
	bool *alreadyUsed = new bool[sStr.length()];
	if(alreadyUsed == NULL)
	{
		cerr << "Unable to allocate 3ish bytes. Your computer sucks hardcore.\n";
		return 1;
	}

	// Declare/init array to make sure we don't reuse a secret char
	// Take up to XX guesses or when they get it all correct
	for(int guessNum = 0; guessNum < 10; guessNum++)
	{
		// Get guess
		string guess;
		do
		{
			cout << "Guess " << guessNum + 1 << ": ";
			cin >> guess;
		}
		while(guess.length() != sStr.length());

		// Reset secret char use array
		for(int i = 0; i < sStr.length(); i++)
		{
			alreadyUsed[i] = false;
		}

		// Counts for each type
		int red = 0;
		int yellow = 0;
		int green = 0;

		// Check each digit
		for(int i = 0; i < sStr.length(); i++)
		{
			// Same character, same place?
			if(guess[i] == sStr[i])
			{
				// If someone else alread used this character, they must have become yellow,
				// so kill that off and make it red.. Doing it this way prevents us from having
				// multiple loops for checking green first, then yellow
				if(alreadyUsed[i])
				{
					yellow--;
					red++;
				}

				// Correct
				alreadyUsed[i] = true;
				green++;
			}
			else 
			{
				// Same character (that isn't used for something else) anywhere in the array?
				for(int j = 0; j < sStr.length(); j++)
				{
					if(!alreadyUsed[j] && guess[i] == sStr[j])
					{
						alreadyUsed[j] = true;
						yellow++;
						red--; // Cancel out inevitable increment
						
						// Prevent it from matching something else as well
						break;
					}
				}

				// Assume it was incorrect
				// If it was really yellow there will have a been a decrement
				// to cancel this out
				red++;
			}

			// DEBUGING
			/*
			cout << "Used: ";
			for(int j = 0; j < sStr.length(); j++)
			{
				cout << (alreadyUsed[j] ? "1" : "0");
			}
			cout << "\n";
			*/
		}

		// FTW?
		if(green == sStr.length())
		{
			cout << "You guessed correctly in " << guessNum + 1 << " tries.\n";
			hasWon = true;
			break;
		}

		// Output result
		if(red > 0)
		{
			cout << "R" << red << " ";
		}
		if(yellow > 0)
		{
			cout << "Y" << yellow << " ";
		}
		if(green > 0)
		{
			cout << "G" << green << " ";
		}
		cout << "\n";

		// Sanity check, put in place to catch a bug I noticed once but had trouble reproducing
		// Left in as a virtually free debug check
		if(red + yellow + green != sStr.length())
		{
			cerr << "\n***********\nCount mismatch\n***********\n\n";
		}
	}

	// If the loop exited because the user ran out of guess, not because they got it
	// completely (all green) correct
	if(!hasWon)
	{
		cout << "Too bad. The correct answer was " << sStr << "\n";
	}

	return 0;
}
