// File name: ttt.cpp
// Author: Ryan Morehart
// Purpose: Implements the main app class and includes the main() that wxwidgets provides.

#include "wx/wx.h"
#include "mainWin.h"
#include "ttt.h"

// Let wxWidgets make main()
IMPLEMENT_APP(TicTacToe)

bool TicTacToe::OnInit()
{
	// Create window
	MainWindow *frame = new MainWindow(wxT("Hello World"), wxPoint(0, 0),
		wxSize(275, 300));
	
	// Display
	frame->Show(true);
	SetTopWindow(frame);
	
	return true;
}

