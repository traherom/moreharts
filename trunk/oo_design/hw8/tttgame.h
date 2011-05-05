// File name: tttgame.h
// Author: Ryan Morehart
// Purpose: Definition for a simple tic tac toe game. Hardly perfect by any stretch of the imagination.

#ifndef TTTGAME_H
#define TTTGAME_H

class TTTGame
{
public:
	TTTGame();
	
	// Index loops from top left to lower
	// right like 0 1 2
	//            3 4 5
	//            6 7 8
	// Returns false if the spot was already used
	bool mark(int index);
	
	// Or you can use the easier method for a human:
	bool mark(int row, int col);
	
	// Returns current player number
	int getCurrentPlayer() { return currPlayer; }

	// Random helper to allow the GUI to ensure X and O equate
	// to player numbers consistently
	char getSymbol(int playerNum) { return (playerNum == 1 ? 'X' : 'O'); }

	// Checks if the given player has won
	enum winState { NoWin = 0, Win, Tie, };
	winState checkWinner(int player);
	
private:	
	int currPlayer;
	int board[3][3];
	int turnCnt;
};

#endif
