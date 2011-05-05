// File name: mainWin.cpp
// Author: Ryan Morehart
// Purpose: Implements the main window for the program. Interacts with game logic backend.
#include "mainWin.h"
#include "tttgame.h"

#include <iostream>
using namespace std;

MainWindow::MainWindow(const wxString &title, const wxPoint &pos, const wxSize &size)
       : wxFrame((wxFrame *)NULL, -1, title, pos, size)
{
	// Main panel area
	wxPanel *panel = new wxPanel(this, -1);
	
	// Create menu
	// ...bar
	wxMenuBar *menuBar = new wxMenuBar;
	wxMenu *menuFile = new wxMenu;
	
	// ...entries
	menuFile->Append(ID_Quit, wxT("E&xit"));
	Connect(ID_Quit, wxEVT_COMMAND_MENU_SELECTED,
		(wxObjectEventFunction) &MainWindow::OnQuit);
	
	// ...put on bar
	menuBar->Append(menuFile, wxT("&File"));
	
	// ...put bar on frame
	SetMenuBar(menuBar);

	// Create status bar
	CreateStatusBar();
	SetStatusText(wxT("Game started, X's turn"));
	
	// Create sizers and arrange
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);
	wxGridSizer *grid = new wxGridSizer(3, 3, 5, 5);
	vbox->Add(grid, 1);
	vbox->Add(hbox, 0, wxALIGN_RIGHT | wxRIGHT | wxBOTTOM, 10);
	
	// Put 9 buttons unto grid. Give each a unique ID, but use same handler.
	wxButton *tmpBtn = 0;
	for(int i = 0; i < 9; i++)
	{
		btnSquares[i] = new wxButton(panel, ID_Square + i, wxT(""));
		Connect(ID_Square + i, wxEVT_COMMAND_BUTTON_CLICKED,
			wxCommandEventHandler(MainWindow::OnSquareChoosen));
		grid->Add(btnSquares[i], 0, wxEXPAND);
	}
	
	// Create quit button and add
	wxButton *quitBtn = new wxButton(panel, wxID_EXIT, wxT("Quit"));
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(MainWindow::OnQuit));
	hbox->Add(quitBtn);
	
	// Create reset button and add
	wxButton *resetBtn = new wxButton(panel, ID_Reset, wxT("Reset"));
	Connect(ID_Reset, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(MainWindow::OnReset));
	hbox->Add(resetBtn);
	
	panel->SetSizer(vbox);
	
	// Finally, set up the game
	game = new TTTGame();
}

MainWindow::~MainWindow()
{
	// Kill game
	delete game;
}

void MainWindow::OnQuit(wxCommandEvent& event)
{
	wxMessageDialog *dial = new wxMessageDialog(NULL,
		wxT("Are you sure to quit?"), wxT("Quit"),
		wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

	int ret = dial->ShowModal();
	dial->Destroy();

	// Only close if they really want to
	if(ret == wxID_YES)
	{
		Close(true);
	}
}

void MainWindow::OnReset(wxCommandEvent& event)
{
	// Make sure they really do want to reset the game
	wxMessageDialog *dial = new wxMessageDialog(NULL,
		wxT("Are you sure to reset the game?"), wxT("Reset"),
		wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);

	int ret = dial->ShowModal();
	dial->Destroy();

	if(ret == wxID_NO)
	{
		// Nope, don't really
		return;
	}
	
	// Use helper
	resetGame();
}

void MainWindow::OnSquareChoosen(wxCommandEvent& event)
{
	// Save current player number because when we mark on the board
	// it will switch it, but we don't want to update the buttons until we
	// know that the play was safe. Also, we need to check if this play wins.
	int player = game->getCurrentPlayer();
	
	// Mark on game
	int btnId = event.GetId();
	if(!game->mark(btnId - ID_Square))
	{
		// Space already used
		SetStatusText(wxString::Format(wxT("Space already used, still %c's turn"),
			game->getSymbol(game->getCurrentPlayer())));
		return;
	}
	
	// Update button
	wxButton *squ = btnSquares[btnId - ID_Square];
	squ->SetLabel(wxString::Format(wxT("%c"), game->getSymbol(player)));
	
	// Check for a winner
	TTTGame::winState winner = game->checkWinner(player);
	if(winner != TTTGame::NoWin)
	{
		wxMessageDialog *dial;
		if(winner == TTTGame::Win)
		{
			// Update status bar
			SetStatusText(wxT("We have a winner!"));
	
			dial = new wxMessageDialog(NULL,
				wxString::Format(wxT("%c has won the game.\nPlay again?"),
				game->getSymbol(player)), wxT("Game Over"),
				wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
		}
		else
		{
			// Tie
			// Update status bar
			SetStatusText(wxT("We have a winner!"));
	
			dial = new wxMessageDialog(NULL,
				wxString::Format(wxT("Cat's game!\nPlay again?"),
				game->getSymbol(player)), wxT("Game Over"),
				wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
		}
		
		// Display dialog and handle response
		int ret = dial->ShowModal();
		dial->Destroy();

		if(ret == wxID_YES)
		{
			// Reset game
			resetGame();
		}
		else
		{
			// Quit
			Close();
		}
		
	}
	
	// Update status bar to show next player
	SetStatusText(wxString::Format(wxT("%c's turn"),
		game->getSymbol(game->getCurrentPlayer())));
}

void MainWindow::resetGame()
{
	// Remove existing game and create new
	if(game != 0)
	{
		delete game;
	}
	game = new TTTGame();
	
	// Clear board
	for(int i = 0; i < 9; i++)
	{
		wxButton *squ = btnSquares[i];
		squ->SetLabel(wxT(""));
	}
	
	// Set status bar
	SetStatusText(wxT("Game reset, X's turn"));
}

