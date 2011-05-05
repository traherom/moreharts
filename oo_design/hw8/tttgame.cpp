// File name: tttgame.cpp
// Author: Ryan Morehart
// Purpose: Implements a tic tac toe game.
#include "tttgame.h"

TTTGame::TTTGame() : currPlayer(1), turnCnt(0)
{
	// Clear entire board
	for(int row = 0; row < 3; row++)
	{
		for(int col = 0; col < 3; col++)
		{
			board[row][col] = 0;
		}
	}
}

bool TTTGame::mark(int index)
{
	// Convert to row, column format
	int row = index / 3;
	int col = index % 3;
	return mark(row, col);
}

bool TTTGame::mark(int row, int col)
{
	// Check if used
	if(board[row][col] != 0)
	{
		return false;
	}
	
	// Mark the play with the current player
	board[row][col] = currPlayer;
	
	// Toggle player
	if(currPlayer == 1)
	{
		currPlayer = 2;
	}
	else
	{
		currPlayer = 1;
	}
	
	// Increment turn count
	turnCnt++;
	
	// Worked fine
	return true;
}

TTTGame::winState TTTGame::checkWinner(int player)
{
	// Look for strings of 3
	winState found = Win;
	
	// Rows
	for(int row = 0; row < 3; row++)
	{
		// Assume there is a winner and look for something makes it not work
		found = Win;
		
		for(int col = 0; col < 3; col++)
		{
			// Check for something which makes this not true
			if(board[row][col] != player)
			{
				found = NoWin;
				break;
			}
		}
		
		// Did it work?
		if(found != NoWin)
		{
			return Win;
		}
	}
	
	// Cols
	for(int col = 0; col < 3; col++)
	{
		// Assume there is a winner and look for something makes it not work
		found = Win;
		
		for(int row = 0; row < 3; row++)
		{
			// Check for something which makes this not true
			if(board[row][col] != player)
			{
				found = NoWin;
				break;
			}
		}
		
		// Did it work?
		if(found != NoWin)
		{
			return Win;
		}
	}
	
	// Diagonals
	// Top left to bottom right
	found = Win;
	for(int i = 0; i < 3; i++)
	{
		if(board[i][i] != player)
		{
			found = NoWin;
			break;
		}
	}
	if(found != NoWin)
	{
		return Win;
	}
	 
	// Top right to bottom left
	found = Win;
	for(int i = 0; i < 3; i++)
	{
		if(board[i][2 - i] != player)
		{
			found = NoWin;
			break;
		}
	}
	if(found != NoWin)
	{
		return Win;
	}
	
	// Well, no one won, check for tie
	if(turnCnt == 9)
	{
		return Tie;
	}
	
	// Nope :(
	return NoWin;
}

