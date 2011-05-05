#ifndef MEMORY_FUNCTIONS_H
#define MEMORY_FUNCTIONS_H

#include <string>
using namespace std;

#include "memory.h"

// Additional state for finding ship dir
// I liked the name SEARCH better for finding the ship
#define LOCKON	4

// Required functions
void   initMemoryrmoreha(computerMemory &memory);
void   updateMemoryrmoreha(int row, int col, int result,
	computerMemory &memory);
string smartMovermoreha(const computerMemory &memory);

// Additional functions
bool search(int &row, int &col, int initRow = -1, int initCol = -1);
void lockon(int &row, int &col, const computerMemory &memory);
void destroy(int &row, int &col, const computerMemory &memory,
	bool reset = false);
void reverseDir(computerMemory &memory);
bool checkFit(int row, int col, int size, const char grid[10][10], bool &horz,
	bool &vert, bool &up, bool &down, bool &left, bool &right);
int getShipSize(int ship);
string rowColToStr(int row, int col);

#endif

