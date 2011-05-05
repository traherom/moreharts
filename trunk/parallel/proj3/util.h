#ifndef UTIL_H
#define UTIL_H

typedef char bool;
#define TRUE 1
#define FALSE 0

#define TOP_ROW_MSG 1
#define BOTTOM_ROW_MSG 2
#define BOARD_STATE_MSG 3

void displayFullBoard(int id, int procCount, unsigned long time,
	bool **localBoard,
	unsigned long height, unsigned long width,
	unsigned long heightTotal, unsigned long widthTotal);
void displayBoard(int id, bool **localBoard, unsigned long height, unsigned long width);

unsigned long calcHeight(int id, int procCount, unsigned long heightTotal);
unsigned long calcStartRow(int id, int procCount, unsigned long heightTotal);
bool **makeBoard(unsigned long height, unsigned long width);
void freeBoard(bool **board);

#endif

