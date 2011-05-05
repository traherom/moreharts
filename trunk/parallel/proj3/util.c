#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "util.h"

void displayBoard(int id, bool **localBoard, unsigned long height, unsigned long width)
{
	// Do not include the sentinel rows/cols
	unsigned long row;
	unsigned long col;
	for(row = 1; row <= height; row++)
	{
		fprintf(stdout, "Proc %d: ", id);
		for(col = 1; col <= width; col++)
		{
			if(localBoard[row][col] == TRUE)
				fputc('X', stdout);
			else
				fputc('-', stdout);
		}
		fputc('\n', stdout);
	}
}

void displayFullBoard(int id, int procCount, unsigned long time,
	bool **localBoard,
	unsigned long height, unsigned long width,
	unsigned long heightTotal, unsigned long widthTotal)
{
	unsigned long row;
	unsigned long col;
	int i;
	bool **fullBoard;
	bool **remoteBoard;
	MPI_Status status;
	
	// Calculations happen correctly still, but it was possible for
	// the full board display itself to desynchronize, leading to some
	// bizarre output
	MPI_Barrier(MPI_COMM_WORLD);
	
	if(id == 0)
	{
		// Get bits from everyone and put in a big array, then
		// display that. Include room for sentinels around edges,
		// just so we can use displayBoard()
		fullBoard = makeBoard(heightTotal + 2, widthTotal + 2);
		
		// Copy ourselves into it
		memcpy(fullBoard[0], localBoard[0], (width + 2) * (height + 2));
		
		// Get pieces from all other procs (already have self, so need one less than total)
		// Get the most space here we might ever need, IE, one more than ourselves
		remoteBoard = makeBoard(height + 1, width + 2);
		
		for(i = 1; i < procCount; i++)
		{
			//printf("Proc %d: Current board:\n", id);
			//displayBoard(id, fullBoard, heightTotal, widthTotal);
			
			// Any proc may send to us in any order
			//printf("Proc %d: Waiting to receive\n", id);
			MPI_Recv(remoteBoard[0], (width + 2) * (height + 1),
				MPI_CHAR, MPI_ANY_SOURCE,
				BOARD_STATE_MSG, MPI_COMM_WORLD, &status);
			//printf("Proc %d: Received from %d\n", id, status.MPI_SOURCE);
		
			// Copy into correct location in array
 			memcpy(fullBoard[calcStartRow(status.MPI_SOURCE, procCount, heightTotal) + 1],
				remoteBoard[0],
				(width + 2) * calcHeight(status.MPI_SOURCE, procCount, heightTotal));
		}
		
		// Whew, all assembled. Show it and clean up
		fprintf(stdout, "Board at time step %lu\n", time);
		displayBoard(id, fullBoard, heightTotal, widthTotal);
		freeBoard(remoteBoard);
		freeBoard(fullBoard);
	}
	else
	{
		// Send our bit to proc 0
		//printf("Proc %d: Sending\n", id);
		MPI_Send(localBoard[1], (width + 2) * height, MPI_CHAR, 0, BOARD_STATE_MSG, MPI_COMM_WORLD);
	}
}

unsigned long calcHeight(int id, int procCount, unsigned long heightTotal)
{
	unsigned long height;
	
	height = floor((double)heightTotal / (double)procCount);
	
	if(heightTotal % procCount > procCount - id - 1)
	{
		// There were extra rows. Take one for this proc
		// Each proc, starting from the last one, will take
		// an extra row. The first always has less, which
		// is maybe good since they'll be doing display
		height++;
	}
	
	return height;
}

unsigned long calcStartRow(int id, int procCount, unsigned long heightTotal)
{
	unsigned long startRow;
	
	startRow = floor((double)heightTotal / (double)procCount) * id;
	if(heightTotal % procCount > procCount - id - 1)
	{
		// We must start one later for each processor _before_ us
		// that will be taking an extra row
		startRow += (heightTotal % procCount) - (procCount - (id + 1)) - 1;
	}
		
	return startRow;
}

bool **makeBoard(unsigned long height, unsigned long width)
{
	bool **board;
	unsigned long row;
	unsigned long col;
	
	board = calloc(height, sizeof(bool*));
	board[0] = calloc(height * width, sizeof(bool));
	for(row = 0; row < height; row++)
	{
		board[row] = board[0] + row * width;
		
		// 0 it all out
		for(col = 0; col < width; col++)
			board[row][col] = FALSE;
	}
	
	return board;
}

void freeBoard(bool **board)
{
	free((void *)board[0]);
	free((void *)board);
}

