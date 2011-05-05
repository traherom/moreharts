// ********************************************************
// player.h
//
// Summary: HW6 - This file includes the class definition for
//		the Player class and all its children.
//
// Author: Ryan Morehart
// Created: Sep 2007
// *******************************************************

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
using namespace std;

// ********************************************************
// Class Name: Player
//
// Description:  This class provides the major logic for
//		a Yahtzee game. In paticular rolling and scoring.
//		It also defines two pure virtual functions which
//		children must override to specify how they want
//		to choose what to roll and where to score the dice.
//
//  *******************************************************
class Player
{
protected:
	// Round player is on
	// This was done this way so that players could be skipped (say
	// they walked off for a bit) but still get the chance to play
	// all there rounds
	int roundNum;
	
	// Actual scores ordered like scoreNames
	int scorecard[13];

	// What should we call this player?
	string name;
	
	// Settings
	const static int NUM_DICE = 5;
	const static int ROUNDS_IN_GAME = 13;
	const static int NUM_REROLLS = 2;
	
	// Errors in scoring
	const static int NO_ERROR = 0;
	const static int ALREADY_SCORED = 1;
	const static int NOT_VALID = 2;
	const static int NONE_EXISTENT = 3;
	
	// Scoring
	// Unscored (not used yet) categories are -1, scored are >=0
	const static int ONES = 0;
	const static int TWOS = 1;
	const static int THREES = 2;
	const static int FOURS = 3;
	const static int FIVES = 4;
	const static int SIXES = 5;
	const static int THREE_OF_A_KIND = 6;
	const static int FOUR_OF_A_KIND = 7;
	const static int FULL_HOUSE = 8;
	const static int SMALL_STRAIGHT = 9;
	const static int LARGE_STRAIGHT = 10;
	const static int YAHTZEE = 11;
	const static int CHANCE = 12;
	
	// The consts above will be tied to corresponding string names via this array:
	string scoreNames[13];
	
	//////////////////////////
	// Pure virtuals for children to override
	// Doing this way allows for relatively easy creation of
	// AI, network, etc. players 
	//////////////////////////
	// Player type specific interations
	// playRoll - Lets user decide what to do with dice roll
	//		dice - the dice for the user to look at
	//		reroll - the return array, true to reroll a dice
	virtual void playRoll(int dice[], bool reroll[]) = 0;
	
	// selectScore - Allows user select where to score
	//		dice - the dice for the user to look at
	//	Return - Place to score (ONES, TWOS, etc)
	virtual int selectScore(int dice[], int error = NO_ERROR) = 0;
	
public:
	// Player - Initializes everything
	Player(string playerName)
	{
		// Corresponding names for each constant. Because I made these strings
		// I couldn't initialize it statically. I hate C char arrays
		scoreNames[ONES] = "Ones";
		scoreNames[TWOS] = "Twos";
		scoreNames[THREES] = "Threes";
		scoreNames[FOURS] = "Fours";
		scoreNames[FIVES] = "Fives";
		scoreNames[SIXES] = "Sixes";
		scoreNames[THREE_OF_A_KIND] = "Three of a Kind";
		scoreNames[FOUR_OF_A_KIND] = "Four of a Kind";
		scoreNames[FULL_HOUSE] = "Full House";
		scoreNames[SMALL_STRAIGHT] = "Small Straight";
		scoreNames[LARGE_STRAIGHT] = "Large Straight";
		scoreNames[YAHTZEE] = "Yahtzee!";
		scoreNames[CHANCE] = "Chance";
		
		// Save name
		name = playerName;
	
		// Round this person will play next
		roundNum = 1;

		// Scoring
		// Unscored (not used yet) categories are -1, scored are >=0
		for(int i = 0; i < ROUNDS_IN_GAME; i++)
		{
			scorecard[i] = -1;
		}
	}

	// takeTurn - Plays a turn. Done this way so that an AI could be 
	//		developed later
	// 	Return - Number of rounds this play still has to play
	int takeTurn();
	
	// roll - Comes up with the final dice roll of player
	//		int dice[] - pointer to array of dice rolled
	void roll(int dice[]);
	
	// score - Saves the roll to a valid location
	//		int dice[] - array of dice
	void score(int dice[]);
	
	// score - Calculates current/final score for user
	// 	Return - int score
	int getScore();
	
	// getName - Tells name Player was constructed with
	//	Return - string name
	string getName();
};

// ********************************************************
// Class Name: HumanPlayer
//
// Description:  This class provides the interface for
//		interacting with a human player.
//
//  *******************************************************
class HumanPlayer : public Player
{
protected:
	void playRoll(int dice[], bool reroll[]);
	int selectScore(int dice[], int error);
	
public:
	HumanPlayer(string name) : Player(name) { }
};

#endif
