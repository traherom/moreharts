// File name: mainWin.h
// Author: Ryan Morehart
// Purpose: Definition for the main window.

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "wx/wx.h"
#include "tttgame.h"

// Event enum
enum eventIDs
{
	ID_Quit = 1,
	ID_Reset = 2,
	ID_Square = 20,
};

class MainWindow : public wxFrame
{
public:
	MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size);
	~MainWindow();

	void OnQuit(wxCommandEvent &event);
	void OnReset(wxCommandEvent &event);
	void OnSquareChoosen(wxCommandEvent& event);

private:
	void resetGame();

	TTTGame *game;
	wxButton *btnSquares[9];
};

#endif

