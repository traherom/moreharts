// File: event.cpp
// Author: Ryan Morehart
// Purpose: Implementation of the Event class.

#include "gate.h"
#include "event.h"

// Actually define nextID
int Event::nextID;
Event::Event(int eventTime, Gate *eventGate) : gate(eventGate), time(eventTime)
{
	currVal = vals.begin();
	id = nextID++;
}

LogicValue Event::getNextValue(bool restart)
{
	// Allow client to start us over
	if(restart)
	{
		currVal = vals.begin();
	}

	// Not at end, right?
	if(currVal != vals.end())
	{
		// Save value as we have to move to next one
		LogicValue t = *currVal;

		// Move on
		currVal++;

		return t;
	}
	else
	{
		// Make it unknown. Seems reasonable , as what it should be is unknown:)
		return Unknown;
	}
}
