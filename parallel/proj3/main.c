#include <mpi.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "util.h"

int main(int argc, char **argv)
{
	// Variables are all nice unsigned long nows because I didn't
	// like being limited to ranges of only 32,000/64,000 numbers.
	// Or whatever int is on this system. I think it might already
	// have been long, but I didn't want to check. This is safer
	// Settings
	bool csvOutput;
	
	char *boardFileName;
	FILE *boardFile;
	unsigned long heightTotal;
	unsigned long widthTotal;
	unsigned long startRow;
	unsigned long height;
	unsigned long width;
	
	unsigned long time;
	unsigned long maxTimeSteps;
	unsigned long displayDelay;
	
	// Results
	bool **board;
	bool **workingBoard;
	bool **tempBoard;
	double elapsedTime;
	
	// MPI stuff
	int processorCount;
	int id;
	
	// Misc
	unsigned long row;
	unsigned long col;
	int alive;
	char *line;
	MPI_Status status;
	
	// Make MPI strip off any special args the user gave it on the command line
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	MPI_Comm_size(MPI_COMM_WORLD, &processorCount);
	
	// Grab our board size
	if(argc == 4)
	{
		boardFileName = argv[1];
		maxTimeSteps = atol(argv[2]);
		displayDelay = atol(argv[3]);
		csvOutput = FALSE;
	}
	else if(argc == 5)
	{
		boardFileName = argv[2];
		maxTimeSteps = atol(argv[3]);
		displayDelay = atol(argv[4]);
		csvOutput = TRUE;
	}
	else
	{
		if(id == 0)
			fprintf(stderr, "Usage: %s [-t] <board file> <sim steps> <display delay>\n", argv[0]);
		fprintf(stderr, "argc: %d", argc);
		MPI_Finalize();
		exit(1);
	}
	
	// Find the size of the full board
	// File should be formatted like:
	// 3 6
	// ----X-
	// ---XXX
	// -XXXX-
	boardFile = fopen(boardFileName, "r");
	if(boardFile == NULL)
	{
		fprintf(stderr, "Unable to find specified board file\n");
		MPI_Finalize();
		exit(2);
	}
	
	fscanf(boardFile, "%lu %lu\n", &heightTotal, &widthTotal);
	
	// Determine local board sizes
	// We will break the board up by rows. Everyone does the full width
	// and comm only has to occur for the bottom row
	height = calcHeight(id, processorCount, heightTotal);
	width = widthTotal;
	startRow = calcStartRow(id, processorCount, heightTotal);
	
	//fprintf(stderr, "Proc %d: height = %lu, width = %lu, start row: %lu\n", id, height, width, startRow);
	
	// Read in the appropriate starting point for this local board
	// Begin by skipping to where we start. Yes, I know a char is 1 byte
	line = malloc(sizeof(char) * (width + 2));
	for(row = 0; row < startRow; row++)
	{
		fgets(line, width + 2, boardFile);
		//fprintf(stderr, "Proc %d: skipping \"%s\"\n", id, line);
	}
	
	// Create board with sentinel dead stops on left and right
	// and a copy of the proc's bottom/top row above/below us (respectively)
	// The inner dimension is setup this way so we ensure that all memory
	// remains contiguous and can be quickly sent using MPI_Send()
	board = makeBoard(height + 2, width + 2);
	workingBoard = makeBoard(height + 2, width + 2);
	
	// Now fill out our bit o' the board
	for(row = 1; row <= height; row++)
	{
		fgets(line, width + 2, boardFile);
		//fprintf(stderr, "Proc %d: using \"%s\"\n", id, line);
		
		for(col = 1; col <= width; col++)
		{
			if(line[col - 1] == 'X')
				board[row][col] = TRUE;
			else
				board[row][col] = FALSE;
		}
	}
	
	fclose(boardFile);
	
	// Display initial state
	displayFullBoard(id, processorCount, 0, board, height, width, heightTotal, widthTotal);
	
	// Start timer after everyone initializes.
	MPI_Barrier(MPI_COMM_WORLD);
	elapsedTime = -MPI_Wtime();
	
	// Play games
	for(time = 1; time <= maxTimeSteps; time++)
	{
		// Send top row to person above and receive from one below
		// This shouldn't deadlock because proc 0 will receive and free up
		// each proc successively. Reminder, we send [1] because of the sentinel row
		// This shouldn't have to be done for the first time step, we could road in
		// the row above/below us initially, but it doesn't throw off results for any
		// significant board sizes/timesteps
		if(id != 0)
			MPI_Send(board[1], width + 2, MPI_CHAR, id - 1, TOP_ROW_MSG, MPI_COMM_WORLD);
		if(id != processorCount - 1)
			MPI_Recv(board[height + 1], width + 2, MPI_CHAR, id + 1, TOP_ROW_MSG, MPI_COMM_WORLD, &status);
		
		// Send bottom row to person below and receive from one above
		if(id != processorCount - 1)
			MPI_Send(board[height], width + 2, MPI_CHAR, id + 1, BOTTOM_ROW_MSG, MPI_COMM_WORLD);
		if(id != 0)
			MPI_Recv(board[0], width + 2, MPI_CHAR, id - 1, BOTTOM_ROW_MSG, MPI_COMM_WORLD, &status);
				
		// Play games, be merry
		for(row = 1; row <= height; row++)
		{
			for(col = 1; col <= width; col++)
			{
				// Check everyone around us
				//fprintf(stderr, "Proc %d: Calculating board[%lu][%lu]\n", id, row, col);
				alive = 0;
				if(board[row - 1][col - 1])
					alive++;
				if(board[row - 1][col])
					alive++;
				if(board[row - 1][col + 1])
					alive++;
				if(board[row][col - 1])
					alive++;
				if(board[row][col + 1])
					alive++;
				if(board[row + 1][col - 1])
					alive++;
				if(board[row + 1][col])
					alive++;
				if(board[row + 1][col + 1])
					alive++;
				
				// How many do we have alive vs. were we alive
				if(board[row][col])
				{
					// We're alive currently
					if(alive == 2 || alive == 3)
						workingBoard[row][col] = TRUE;
					else
						workingBoard[row][col] = FALSE;
				}
				else
				{
					// We're dead currently
					if(alive == 3)
						workingBoard[row][col] = TRUE;
					else
						workingBoard[row][col] = FALSE;
				}
			}
		}
		
		// And make the working board our current one
		tempBoard = board;
		board = workingBoard;
		workingBoard = tempBoard;
		
		//fprintf(stderr, "Proc %d: time %lu\n", id, time);
		//displayBoard(id, board, height, width);
		
		// Do we have to output the board now?
		if(time % displayDelay == 0)
		{
			displayFullBoard(id, processorCount, time,
				board,
				height, width,
				heightTotal, widthTotal);
		}
	}
	
	// Stop timer and output the result
	elapsedTime += MPI_Wtime();
	if(id == 0)
	{
		if(csvOutput)
		{
			//printf("%ld, %d, %ld, %d, %10.15f\n", maxNum, calcMode, globalCount, processorCount, elapsedTime);
		}
		else
		{
			printf("Total elapsed time: %10.15f\n", elapsedTime);
			printf("Processors used: %d\n", processorCount);
		}
	}
	
	MPI_Finalize();
	return 0;
}

