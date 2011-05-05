#include "memory.h"
#include "memory_functions_rmoreha.h"

#include <sstream>
using namespace std;

void initMemoryrmoreha(computerMemory &memory) {
   memory.mode        =  SEARCH;
   memory.hitRow      = -1;
   memory.hitCol      = -1;
   memory.hitShip     =  NONE;
   memory.shipDir     =  NONE;
   memory.distFromHit =  -1;
   memory.lastResult  =  NONE;

   for (int i = 0; i < BOARDSIZE; i++) {
      for (int j = 0; j < BOARDSIZE; j++) {
         memory.grid[i][j] = EMPTY_MARKER;
      }
   }
   
   // Initialize additional hit tracker
   memory.depth = 0;
   for(int i = 0; i < 5; i++)
   {
	memory.hitRows[i] = -1;
	memory.hitCols[i] = -1;
	memory.lastResults[i] = NONE;
   }
}

string smartMovermoreha(const computerMemory &memory)
{
	// Save move here
	string move;
	int row = -1;
	int col = -1;


	// State machine
	switch(memory.mode)
	{
	case SEARCH:
		do
		{
			if(search(row, col))
			{
				// Reset search every time we exhaust one
				if(col == 10)
				{
					// Third time through
					// This should actually never happen because we're
					// guarranteed to have hit every ship with the last run
					// through, but a safety net with only one extra line is
					// always good
					search(row, col, 7, 0);
				}
				else if(col == 12)
				{
					// Second time through
					search(row, col, 8, 0);
				}
				else
				{
					// First time through
					search(row, col, 9, 0);
				}
			}
		}
		while(memory.grid[row][col] != EMPTY_MARKER);

		break;

	case LOCKON:
		// We've hit something, now find which direction it is
		lockon(row, col, memory);
		break;

	case DESTROY:
		// Finish off the ship
		destroy(row, col, memory);
		break;

	default:
		// Bad state
		DEBUGOUT("Invalid state");
	}

	move = rowColToStr(row, col);
	return move;
}

void updateMemoryrmoreha(int row, int col, int result, computerMemory &memory)
{
	// Mark the spot as being shot at
	memory.grid[row][col] = '*';

	// If we ever sink a ship then return to searching
	if(ISASUNK(result))
	{
		// Reset everything just to be clean
		memory.hitRow = -1;
		memory.hitCol = -1;
		memory.hitShip = NONE;
		memory.distFromHit = -1;

		result = MISS;
		memory.mode = SEARCH;
	}
	
	// Check if there are any unkilled ships that have been found
	if(memory.mode == SEARCH && memory.depth > 0)
	{
		// Yep, fake a hit
		memory.depth--;
		row = memory.hitRows[memory.depth];
		col = memory.hitCols[memory.depth];
		result = memory.lastResults[memory.depth];
	}

	if(memory.mode == SEARCH && ISAHIT(result))
	{
		// Switch to lock on if we hit something when searching
		memory.mode = LOCKON;
		
		// Save info about the ship we hit
		// This will stay constant until we destroy it
		memory.hitRow = row;
		memory.hitCol = col;
		memory.hitShip = result - HIT;

		// Clear result
		result = MISS;
	}
	else if(memory.mode != SEARCH && ISAHIT(result) && result - HIT != memory.hitShip)
	{
		// If we hit a ship diffenent than the one we already had
		// while finding direction or destroying...
		// Save for later use
		memory.hitRows[memory.depth] = row;
		memory.hitCols[memory.depth] = col;
		memory.lastResults[memory.depth] = result;
		memory.depth++;
		
		// Fake miss
		result = MISS;
	}
	
	if(memory.mode == LOCKON && ISAHIT(result))
	{
		// If we hit the same ship while locking on
		// switch to destroy. Note that make sure it's
		// the same ship was taken care of above
	
		// Figure out which direction this was
		if(row < memory.hitRow)
		{
			memory.shipDir = NORTH;
		}
		else if(row > memory.hitRow)
		{
			memory.shipDir = SOUTH;
		}
		else if(col < memory.hitCol)
		{
			memory.shipDir = WEST;
		}
		else if(col > memory.hitCol)
		{
			memory.shipDir = EAST;
		}

		memory.distFromHit = 1;
		memory.mode = DESTROY;
	}
	
	// Destroying logic
	if(memory.mode == DESTROY)
	{
		// Increment so we hit further away next time
		memory.distFromHit++;

		// Watch for limit conditions (ship isn't the way we're going)...
		// 	... missed ship
		if(!ISAHIT(result))
		{
			reverseDir(memory);
		}
		//	... edge of battlefield
		else if(memory.shipDir == NORTH && row == 0)
		{
			reverseDir(memory);
		}
		else if(memory.shipDir == SOUTH && row == 9)
		{
			reverseDir(memory);
		}
		else if(memory.shipDir == EAST && col == 9)
		{
			reverseDir(memory);
		}
		else if(memory.shipDir == WEST && col == 0)
		{
			reverseDir(memory);
		}
		//	... next shot has already been done
		else if(memory.shipDir == NORTH && memory.grid[memory.hitRow - memory.distFromHit][memory.hitCol] != EMPTY_MARKER)
		{
			DEBUGOUT("Already");
			reverseDir(memory);
		}
		else if(memory.shipDir == SOUTH && memory.grid[memory.hitRow + memory.distFromHit][memory.hitCol] != EMPTY_MARKER)
		{
			DEBUGOUT("Already");
			reverseDir(memory);
		}
		else if(memory.shipDir == EAST && memory.grid[memory.hitRow][memory.hitCol + memory.distFromHit] != EMPTY_MARKER)
		{
			DEBUGOUT("Already");
			reverseDir(memory);
		}
		else if(memory.shipDir == WEST && memory.grid[memory.hitRow][memory.hitCol - memory.distFromHit] != EMPTY_MARKER)
		{
			DEBUGOUT("Already");
			reverseDir(memory);
		}
	}
	
	// Save result. Down here so that we can set it to an untrue
	// value to trick the logic
	memory.lastResult = result;
}

bool search(int &row, int &col, int initRow, int initCol)
{
	// Instead of abusing the computer memory struct, we'll
	// just use statics
	static int searchDone = true;
	static int rowSearch = -1;
	static int colSearch = -1;

	// Has the user told us to (re)start at a certain spot?
	if(initRow > -1 && initCol > -1)
	{
		// Set to one less because the function will
		// increment them before returning
		rowSearch = initRow - 1;
		colSearch = initCol - 1;

		searchDone = false;
	}

	// If the search is finished, then don't do anything
	if(!searchDone)
	{
		// By default we follow a diagonal pattern
		if(rowSearch == 9)
		{
			// Start search again from 3 above where we did last time
			rowSearch = (BOARDSIZE - 1) - colSearch - 3;
			colSearch = 0;

			// Catch the case where we transition to moving along top
			if(rowSearch < 0)
			{
				colSearch = -1 * rowSearch;
				rowSearch = 0;
			}
		}
		else if(colSearch == 9)
		{
			// Start search again from 3 to the right
			colSearch = (BOARDSIZE - 1) - rowSearch + 3;
			rowSearch = 0;

			// Catch the case where we transition to moving along top
			if(colSearch > 9)
			{
				searchDone = true;
			}
		}
		else
		{
			// Do diagonal
			rowSearch++;
			colSearch++;
		}
	}

	// Return coordinates
	row = rowSearch;
	col = colSearch;

	return searchDone;
}

void lockon(int &row, int &col, const computerMemory &memory)
{
	// Figure out which directions are actually possible
	// Do this every time  because that will cause 
	bool vert, horz, left, right, up, down;
	checkFit(memory.hitRow, memory.hitCol, getShipSize(memory.hitShip), memory.grid,
		horz, vert, up, down, left, right);

	// First try single-direction possiblities, then vert and horz
	if(up)
	{
		row = memory.hitRow - 1;
		col = memory.hitCol;
	}
	else if(down)
	{
		row = memory.hitRow + 1;
		col = memory.hitCol;
	}
	else if(left)
	{
		row = memory.hitRow;
		col = memory.hitCol - 1;
	}
	else if(right)
	{
		row = memory.hitRow;
		col = memory.hitCol + 1;
	}
	else if(vert)
	{
		// Try down
		row = memory.hitRow + 1;
		col = memory.hitCol;
	}
	else if(horz)
	{
		// Try right
		row = memory.hitRow;
		col = memory.hitCol + 1;
	}
	else
	{
		DEBUGOUT("No direction valid.");
	}
}

void destroy(int &row, int &col, const computerMemory &memory, bool reset)
{
	// Shoot in the current direction
	if(memory.shipDir == NORTH)
	{
		row = memory.hitRow - memory.distFromHit;
		col = memory.hitCol;
	}
	if(memory.shipDir == SOUTH)
	{
		row = memory.hitRow + memory.distFromHit;
		col = memory.hitCol;
	}
	if(memory.shipDir == EAST)
	{
		row = memory.hitRow;
		col = memory.hitCol + memory.distFromHit;
	}
	if(memory.shipDir == WEST)
	{
		row = memory.hitRow;
		col = memory.hitCol - memory.distFromHit;
	}
}

// Resets the shooting to the beginning in the opposite direction
void reverseDir(computerMemory &memory)
{
	// Reverse direction
	if(memory.shipDir == NORTH)
		memory.shipDir = SOUTH;
	else if(memory.shipDir == SOUTH)
		memory.shipDir = NORTH;
	else if(memory.shipDir == EAST)
		memory.shipDir = WEST;
	else if(memory.shipDir == WEST)
		memory.shipDir = EAST;
		
	// Reset dist
	// The loop inside reaches the part we need to start shooting at (skips spots already hit)
	memory.distFromHit = 1;
	if(memory.shipDir == NORTH)
	{
		while(memory.grid[memory.hitRow - memory.distFromHit][memory.hitCol] != EMPTY_MARKER)
		{
			memory.distFromHit++;
		}
	}
	else if(memory.shipDir == SOUTH)
	{
		while(memory.grid[memory.hitRow + memory.distFromHit][memory.hitCol] != EMPTY_MARKER)
		{
			memory.distFromHit++;
		}
	}
	else if(memory.shipDir == EAST)
	{
		while(memory.grid[memory.hitRow][memory.hitCol + memory.distFromHit] != EMPTY_MARKER)
		{
			memory.distFromHit++;
		}
	}
	else if(memory.shipDir == WEST)
	{
		while(memory.grid[memory.hitRow][memory.hitCol - memory.distFromHit] != EMPTY_MARKER)
		{
			memory.distFromHit++;
		}
	}
}

// Checks if the given size ship could fit at the location given,
// either vertically or horizontally
bool checkFit(int row, int col, int size, const char grid[10][10], bool &horz,
	bool &vert, bool &up, bool &down, bool &left, bool &right)
{
	int spaceL = 0;
	int spaceR = 0;
	int spaceU = 0;
	int spaceD = 0;

	int i;

	// Check spaces all around
	// Left
	i = 1;
	while(col - i >= 0 && grid[row][col - i] == EMPTY_MARKER)
	{
		i++;
		spaceL++;
	}
	
	// Right
	i = 1;
	while(col + i < BOARDSIZE && grid[row][col + i] == EMPTY_MARKER)
	{
		i++;
		spaceR++;
	}
	
	// Up
	i = 1;
	while(row - i >= 0 && grid[row - i][col] == EMPTY_MARKER)
	{
		i++;
		spaceU++;
	}
	
	// Down
	i = 1;
	while(row + i < BOARDSIZE && grid[row + i][col] == EMPTY_MARKER)
	{
		i++;
		spaceD++;
	}
	
	// Just return simple booleans if it can fit
	size--;
	left = spaceL >= size;
	right = spaceR >= size;
	up = spaceU >= size;
	down = spaceD >= size;
	horz = (spaceL + spaceR) >= size;
	vert = (spaceU + spaceD) >= size;
}

int getShipSize(int ship)
{
	int shipSize;
	if(ship == AC)
		shipSize = AC_SIZE;
	else if(ship == BS)
		shipSize = BS_SIZE;
	else if(ship == CR)
		shipSize = CR_SIZE;
	else if(ship == SB)
		shipSize = SB_SIZE;
	else if(ship == DS)
		shipSize = DS_SIZE;
	else
		shipSize = -1;
	return shipSize;
}

string rowColToStr(int row, int col)
{
	string t;

	// Row 0 is A (65)
	t = (char)(65 + row);
	t += numToString(col + 1);

	return t;
}

