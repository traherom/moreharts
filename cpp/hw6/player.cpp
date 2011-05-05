// ********************************************************
// player.cpp
//
// Summary: HW6 - This file includes the implementation for
//		the Player class and all its children.
//
// Author: Ryan Morehart
// Created: Sep 2007
// *******************************************************

#include "player.h"

#include <iostream>
using namespace std;

/////////////////////////////////
// Parent Player class
/////////////////////////////////
int Player::takeTurn()
{
	// Dice for this turn are saved here and passed everywhere
	int dice[NUM_DICE];
	
	// Tell them what round this is and who's going
	cout << "\n\n------------- Round " << roundNum << " --------------\n";
	
	// Roll dice
	roll(dice);

	// Score the dice roll
	score(dice);

	// Now that they've taken a turn, return the number of
	// rounds they have _left_ and bump what round they're on up.
	return ROUNDS_IN_GAME - roundNum++;
}

void Player::roll(int dice[])
{
	// First roll, fill all with 1-6
	for(int i = 0; i < NUM_DICE; i++)
	{
		dice[i] = rand() % 6 + 1;
	}

	// Prompt to reroll twice
	// Stop prompting if they reroll nothing
	bool noRerolls = false;
	bool reroll[NUM_DICE];
	for(int roll = 0; roll < NUM_REROLLS && !noRerolls; roll++)
	{
		// Get interaction from player, regardless of type
		playRoll(dice, reroll);
		
		// Don't ask them again if they don't reroll anything
		noRerolls = true;
		
		// Reroll ones the person request us to
		for(int diceNum = 0; diceNum < NUM_DICE; diceNum++)
		{
			if(reroll[diceNum] == true)
			{
				// Reroll dice and mark as having changed something
				noRerolls = false;
				dice[diceNum] = rand() % 6 + 1;
			}
		}
	}
}

void Player::score(int dice[])
{
	// Where score will be saved
	int saveLoc;
	
	// Save if an error with the selection was encountered
	int error = NO_ERROR;
	
	// Misc. variables used for checking validity of using a roll
	int countMostDice = 0;
	bool foundMatch;
	bool oneEach;
	bool seenTwo;
	bool seenThree;
	int straightCount;
	
	// Get counts of each dice type and total while we're at it,
	// as a few things use the total
	int counts[6] = {0, 0, 0, 0, 0, 0};
	int total = 0;
	for(int i = 0; i < NUM_DICE; i++)
	{
		// Add one to this die's spot
		counts[dice[i] - 1]++;
		
		// Add die to total
		total += dice[i];
	}
		
	// Loop until the user selects a valid location
	bool scoreRecorded = false;
	while(scoreRecorded == false)
	{
		// Get where to score from child class
		saveLoc = selectScore(dice, error);
		
		// Uh, it is in the range, right?
		if(saveLoc < ONES || saveLoc > CHANCE)
		{
			error = NOT_VALID;
			continue;
		}
		
		// Already filled? (and not yahtzee)
		if(scorecard[saveLoc] >= 0 && saveLoc != YAHTZEE)
		{
			error = ALREADY_SCORED;
			continue;
		}
		
		switch(saveLoc)
		{
		case ONES:		// For all number categories
		case TWOS:		// Just used dice of that cat *
		case THREES:	// number on dice
		case FOURS:		// IE: Dice are 2 3 3 4 3;
		case FIVES:		// 	Threes = 9
		case SIXES:
			scorecard[saveLoc] = counts[saveLoc] * (saveLoc + 1);
			scoreRecorded = true;
			break;

		case FOUR_OF_A_KIND:	// total all dice
		case THREE_OF_A_KIND:	// total all dice
		case CHANCE:			// total all dice
			// Find how many of the most dice there are
			// Only used if this is three/four of a kind, but it's fast
			// so who cares
			countMostDice = 0;
			for(int i = 0; i < 6; i++)
			{
				if(counts[i] > countMostDice)
				{
					countMostDice = counts[i];
				}
			}
		
			// Only save if this is chance or a valid three/four of a kind
			// Otherwise the user is scratching here
			if(saveLoc == CHANCE
				|| (saveLoc == THREE_OF_A_KIND && countMostDice >= 3)
				|| (saveLoc == FOUR_OF_A_KIND && countMostDice >= 4))
			{
				scorecard[saveLoc] = total;
			}
			else
			{
				scorecard[saveLoc] = 0;
			}
			
			scoreRecorded = true;
			break;

		case FULL_HOUSE:	// 25 pts
			// Look for two of one and three of another
			seenTwo = false;
			seenThree = false;
			for(int i = 0; i < 6; i++)
			{
				if(counts[i] == 2)
				{
					seenTwo = true;
				}
				else if(counts[i] == 3)
				{
					seenThree = true;
				}
			}
		
			// Is this really a full house or did the user scratch?
			if(seenTwo && seenThree)
			{
				scorecard[saveLoc] = 25;
			}
			else
			{
				scorecard[saveLoc] = 0;
			}
			
			scoreRecorded = true;
			break;

		case SMALL_STRAIGHT:	// 30 pts
			// Must be a row of 3
			straightCount = 0;
			for(int i = 0; i < 6; i++)
			{
				if(counts[i] != 0)
				{
					straightCount++;
					
					// Is this the fourth in a row?
					if(straightCount >= 4)
					{
						break;
					}
				}
				else
				{
					// Reset counter
					straightCount = 0;
				}
			}
			
			// Valid (4 consecutive numbers) or scratch?
			if(straightCount >= 4)
			{
				scorecard[saveLoc] = 30;
			}
			else
			{
				scorecard[saveLoc] = 0;
			}
			
			scoreRecorded = true;
			break;

		case LARGE_STRAIGHT:	// 40 pts
			// Must be a row of 4
			straightCount = 0;
			for(int i = 0; i < 6; i++)
			{
				if(counts[i] != 0)
				{
					straightCount++;
					
					// Is this the fifth in a row?
					if(straightCount >= 5)
					{
						break;
					}
				}
				else
				{
					// Reset counter
					straightCount = 0;
				}
			}
			
			// Valid (5 consecutive numbers) or scratch?
			if(straightCount >= 5)
			{
				scorecard[saveLoc] = 40;
			}
			else
			{
				scorecard[saveLoc] = 0;
			}
			
			scoreRecorded = true;
			break;

		case YAHTZEE:		// 50 pts first one, 100 after
			// Can't be used in the user scratched in here (took a 0)
			if(scorecard[saveLoc] == 0)
			{
				error = NOT_VALID;
				break;
			}

			// Must be one category with 5
			foundMatch = false;
			for(int i = 0; i < 6; i++)
			{
				if(counts[i] == 5)
				{
					foundMatch = true;
					break;
				}
			}
			
			if(foundMatch)
			{
				if(scorecard[saveLoc] < 0)
				{
					// First one
					scorecard[saveLoc] = 50;
				}
				else
				{
					// WOW! A second one
					scorecard[saveLoc] += 100;
				}
			}
			else
			{
				// Scratch the Yahtzee. Ouch.
				scorecard[saveLoc] = 0;
			}
			
			scoreRecorded = true;
			break;

		default:
			error = NONE_EXISTENT;
		}
	}
	
	// Tell user where it was saved and for how much
	cout << "Scored " << scorecard[saveLoc] << " in " << scoreNames[saveLoc] << "\n";
}

int Player::getScore()
{
	int upperScore = 0;
	int lowerScore = 0;

	// Total upper half (1-6s) and check for bonus (>=63)
	for(int i = ONES; i <= SIXES; i++)
	{
		upperScore += scorecard[i];
	}

	if(upperScore >= 63)
	{
		upperScore += 35;
	}

	// Total lower half
	for(int i = THREE_OF_A_KIND; i <= CHANCE; i++)
	{
		lowerScore += scorecard[i];
	}

	return upperScore + lowerScore;
}

string Player::getName()
{
	return name;
}

/************************
Human and eventually AI specifics
************************/
void HumanPlayer::playRoll(int dice[], bool reroll[])
{
	cout << "Your roll: ";
	for(int i = 0; i < NUM_DICE; i++)
	{
		cout << dice[i] << " ";
	}

	cout << "-- What do you want to re-roll? (y to roll, n to keep)\n";
	cout << "           ";

	// Read in the 5 letters
	for(int diceNum = 0; diceNum < NUM_DICE; diceNum++)
	{
		char choice;
		cin >> choice;

		// Reroll this dice?
		if(choice == 'y' || choice == 'Y')
		{
			reroll[diceNum] = true;
		}
		else
		{
			reroll[diceNum] = false;
		}
	}
}

int HumanPlayer::selectScore(int dice[], int error = NO_ERROR)
{
	// Unless there was an error, show the score card
	if(error == NO_ERROR)
	{
		// Display current scores/options in like:
		// Number	Name	Current Score
		cout.setf(ios::left);

		cout << "+------ Score Card -------+\n";
		for(int currCat = 0; currCat < 13; currCat++)
		{
			// Allow category to be selected if it hasn't been used or it's Yahtzee
			if(scorecard[currCat] < 0 || currCat == YAHTZEE)
			{
				cout << "| ";
				cout.width(2);
				cout << currCat + 1 << " ";
			}
			else
			{
				cout << "|    ";
			}
			
			// Name
			cout.width(17);
			cout << scoreNames[currCat];
			
			if(scorecard[currCat] >= 0)
			{
				// Current score in this category
				cout.width(4);
				cout << scorecard[currCat];
			}
			else
			{
				// Place holder
				cout << "    ";
			}

			// Finishing touch
			cout << "|\n";
		}
		
		cout << "+-------------------------+\n";
	}
	else
	{
		switch(error)
		{
		case ALREADY_SCORED:
			cout << "This category has already been filled.\n";
			break;
		
		case NOT_VALID:
			cout << "You cannot play this roll there.\n";
			break;
			
		case NONE_EXISTENT:
			cout << "The category you choose does not exist.\n";
			break; 
			
		default:
			cout << "An unknown error occured.\n";
		}
		
		cout << "Please choose a different location.\n";
	}
	
	// Tell user final dice roll again just for reference
	cout << "Final dice: ";
	for(int i = 0; i < NUM_DICE; i++)
	{
		cout << dice[i] << " ";
	}
	cout << "\n";
	
	// Loop until a valid place is selected
	bool scored = false;
	int saveLoc;
	while(!scored)
	{
		// Get where user wants to put it
		// Note that the displayed loctions start at 1, not 0, so we move it down
		cout << "Where do you want to save this roll? ";
		cin >> saveLoc;
		saveLoc--;
		scored = true;
	}
	
	return saveLoc;
}
