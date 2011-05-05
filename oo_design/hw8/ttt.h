// File name: ttt.h
// Author: Ryan Morehart
// Purpose: Just a simple declation for the main app class.

#ifndef TTT_H
#define TTT_H

#include "wx/wx.h"

class TicTacToe: public wxApp
{
public:
	virtual bool OnInit();

private:
	// none
};

#endif

