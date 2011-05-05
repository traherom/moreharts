// Author:  Keith Shomper
// Date:    24 Oct 03
// Purpose: Programming Assignment #7 - Practice with calling functions

#include <cstdlib>
using namespace std;

#include "battleship.h"
#include "memory.h"                                                     // NEW
#include "memory_functions_rmoreha.h"                                   // NEW

int main(int argc, char **argv)
{
	long int totalMoves = 0;
	long int myWins = 0;

	board          humanBoard, computerBoard;
	computerMemory memory;
	bool           done = false;

	int numToPlay = 100;
	if(argc == 2)
	{
		numToPlay = atoi(argv[1]);
	}

	for(int i = 0; i < numToPlay; i++)
	{
		string         humanMove, computerMove, message;
		int            numHumanShipsSunk = 0, numComputerShipsSunk = 0;
		int            humanRow, humanColumn, computerRow, computerColumn;
		int            checkValue, humanResult, computerResult;

		// Initializie the game boards
		initializeBoard(humanBoard);
		initializeBoard(computerBoard);
		initMemoryrmoreha(memory);                                              // NEW

		// Play the game until one player has sunk the other's ships
		done = false;
		while (!done) {
			// get the player's move
			humanMove  = randomMove();
			checkValue = checkMove(humanMove, computerBoard, humanRow, humanColumn);

					// validate the move
			while (checkValue != VALID_MOVE) {
				// if necessary, get another move
				humanMove  = randomMove();
				checkValue = checkMove(humanMove, computerBoard, humanRow,humanColumn);
			}

			// get the computer's move
			computerMove = smartMovermoreha(memory);                           // CHANGED
			while (checkMove(computerMove, humanBoard, computerRow, computerColumn)
				!= VALID_MOVE) {
				computerMove = randomMove();
			}

			totalMoves++;

			// execute the moves on the respective boards
			humanResult    = playMove(humanRow, humanColumn, computerBoard);
			computerResult = playMove(computerRow, computerColumn, humanBoard);
			updateMemoryrmoreha(computerRow, computerColumn, computerResult, memory);

			if (isItSunk(humanResult)) { numHumanShipsSunk++; }
			if (isItSunk(computerResult)) { numComputerShipsSunk++; }

			// determine if we have a winner
			if (numHumanShipsSunk == 5 || numComputerShipsSunk == 5) {
				done = true;
			}
		}

		// Announce the winner
		if (numComputerShipsSunk == 5) {
			myWins++;
		}
	}

	cout << "Avg. moves: " << totalMoves / numToPlay;
	cout << "\nWins: " << myWins << " out of " << numToPlay << "\n";

	return 0;
}
