// Author:  Ryan Morehart
// Date: 11/18/07
// Purpose: Plays a human v. comp Battleship game

#include <string>
using namespace std;

#include "battleship.h"

int main()
{
	// variable declarations (you'll need others, of course)
	board human;
	board computer;
	bool done = false;
	
	int rowHum;
	int colHum;
	int rowComp;
	int colComp;
	int resultHum;
	int resultComp;
	int humShipsSunk = 0;
	int compShipsSunk = 0;
	
	string response;
	int valid;

	// Welcome the player to the game
	welcome();

	// Initialize the game boards
	initializeBoard(human);
	initializeBoard(computer);

	// Play the game until one player has sunk the other's ships
	while (!done)
	{
		// Clear the screen to prepare show the game situation
		// before the moves. Display the game board situation
		clearTheScreen();
		displayBoard(1, 1, HUMAN, human);
		displayBoard(1, 40, COMPUTER, computer);
		
		// Get and validate the human player's move
		valid = REUSED_MOVE;
		while(valid != VALID_MOVE)
		{
			clearTheLine(15);
			response = getResponse(15, 1, "Shoot at: ");
			valid = checkMove(response, computer,	
				rowHum, colHum);
			
			clearTheLine(14);
			if(valid == REUSED_MOVE)
			{
				writeMessage(14, 1, "You have already shot\
 there.");
			}
			else if(valid == ILLEGAL_FORMAT)
			{
				writeMessage(14, 1, "Illegal format for\
 location. Should be in the format B7.");
			}
		}

		// Get and validate the computer's move
		do
		{
			response = randomMove();
		}
		while(checkMove(response, human, rowComp, colComp)
			!= VALID_MOVE);

		// Execute both moves
		resultHum = playMove(rowHum, colHum, computer);
		resultComp = playMove(rowComp, colComp, human);
		
		// Show the results of the moves
		clearTheLine(14);
		writeResult(16, 1, resultHum, HUMAN);
		writeMessage(17, 1, "The computer shot at " + response);
		writeResult(18, 1, resultComp, COMPUTER);
		
		// Take note if there are any sunken ships
		if(isItSunk(resultHum))
		{
			compShipsSunk++;
		}
		if(isItSunk(resultComp))
		{
			humShipsSunk++;
		}
		 
		// determine if we have a winner
		if(humShipsSunk >= 5 || compShipsSunk >= 5)
		{
			done = true;
		}
		else
		{
			pauseForEnter();
		}
	}

	// Announce the winner
	clearTheScreen();
	writeMessage(9, 25, "************************");
	if(compShipsSunk == 5)
	{
		writeMessage(10, 25, "******* You win! *******");
	}
	else
	{
		writeMessage(10, 25, "*** The computer won ***");
	}
	writeMessage(11, 25, "************************");

	// pause to let the result of the game sink in
	pauseForEnter();
	clearTheScreen();
	
	return 0;
}
