// ********************************************************
// split.cpp
//
// Summary: HW6 - This file contains the main controller
//		for the Yahtzee game.
//
// Author: Ryan Morehart
// Created: Sep 2007
// *******************************************************

#include <iostream>
#include <cstdlib>
#include <string>
using namespace std;

#include "player.h"

int main(int argc, char **argv)
{
	// Players. Up to 10 may play
	int numPlayers;
	Player *players[10];

	// Check params and pull in player names
	// Having nothing is a special case of a player named "player"
	if(argc > 11)
	{
		cout << "This game only supports 10 players per game\n";
		return 1;
	}
	if(argc > 1)
	{
		numPlayers = argc - 1;
		for(int i = 1; i < argc; i++)
		{
			players[i - 1] = new HumanPlayer(argv[i]);
		}
	}
	else
	{
		// Only one human player
		numPlayers = 1;
		players[0] = new HumanPlayer("Player");
	}

	// Seed random with system clock
	srand(time(0));

	// Just a quick note for user
	cout << "Beginning new game with " << numPlayers << " players..\n";

	// A game continues until everyone has taken all their turns
	bool peopleLeft = true;
	while(peopleLeft)
	{
		// Start off with the assumption we've finished everyone
		peopleLeft = false;
		
		for(int i = 0; i < numPlayers; i++)
		{
			if(players[i]->takeTurn() > 0)
			{
				// This person has rounds left, so loop again
				peopleLeft = true;
			}
		}
	}

	// Display scores
	
	cout << "\n\n\n+-------- Final Scores --------+\n";
	for(int i = 0; i < numPlayers; i++)
	{
		cout << "| ";
		cout.width(26);
		cout << players[i]->getName();
		cout.width(0);
		cout << players[i]->getScore() << " |\n";
	}
	cout << "+------------------------------+\n";
	
	return 0;
}

