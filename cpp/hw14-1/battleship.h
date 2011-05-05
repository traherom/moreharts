#ifndef BATTLESHIP_H
#define BATTLESHIP_H

// Author:  Keith Shomper
// Date:    24 Oct 03
// Purpose: Header file for implementing a text-based battleship game

// include files for implementing battleship
#include <iostream>
#include <time.h>
#include <curses.h>
#include <kasbs.h>

using namespace std;

// use these constants to indicate if the player is a human or a computer
// battleship board size is 10x10 grid
const   int   BOARDSIZE = 10;

// data structure for position
class  position {
 public:
   int  startRow;          // ship's initial row
   int  startCol;          // ship's initial column
   int  orient;            // indicates whether the ship is running across
                           // or up and down
};

// data structure for ship
class  ship {
 public:
   position pos;            // where the ship is on the board
   int  size;               // number of hits required to sink the ship
   int  hitsToSink;         // number of hits remaining before the ship is sunk
   char marker;             // the ASCII marker used to denote the ship on the
                            // board
};

// a game board is made up of a 10x10 playing grid and the ships
class  board {
 public:
   char grid[BOARDSIZE][BOARDSIZE];
   ship s[6];               // NOTE:  the first (zeroth) position is left empty
};

// use these constants for designating to which player we are referring
const   int   HUMAN          = 0;
const   int   COMPUTER       = 1;

// use these constants for deciding whether or not the user gave a proper move
const   int   VALID_MOVE     = 0;
const   int   ILLEGAL_FORMAT = 1;
const   int   REUSED_MOVE    = 2;

// functions for screen control and I/O
void    welcome(void);
void    clearTheLine(int x);
void    clearTheScreen(void);
void    pauseForEnter(void);
string  getResponse(int x, int y, string prompt);
void    writeMessage(int x, int y, string message);
void    writeResult(int x, int y, int result, int playerType);
void    displayBoard(int x, int y, int playerType, const board &gameBoard);

// functions to control the board situation
void    initializeBoard(board &gameBoard);
int     playMove(int row, int col, board &gameBoard);

// function to tell what happened in the last play_move() command
bool    isItSunk(int playMoveResult);
 
// misc game functions
string  randomMove(void);
int     checkMove(string move, const board &gameBoard, int &row, int &col);
void    debug(int x, int y, string s);
string  numToString(int x);

// a debug macro 
#ifdef DEBUG
#define DEBUGOUT(str) debug (22, 1, (str));
#else
#define DEBUGOUT(str)
#endif

#endif // BATTLESHIP_H
