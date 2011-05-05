// File: event.h
// Author: Ryan Morehart
// Purpose: Definition of the Event holder class and it's supporting
//	comparator class

#ifndef EVENT_H
#define EVENT_H

#include <list>
using std::list;

#include "gate.h"

class Event
{
public:
	Event(int eventTime, Gate *eventGate);

	// Add another value for an input to this gate/wire
	void addInputVal(LogicValue val) { vals.push_back(val); }

	// Allow clients to easily pull off next input value, rather than processing list themselves
	// Returns unknown if it gets called too many times
	LogicValue getNextValue(bool restart = false);

	// Accessors
	Gate *getGate() { return gate; }
	int getTime() const { return time; }
	int getID() const { return id; }

	static int nextID;

private:
	// Event identifier stuff and such
	Gate *gate;
	const int time;

	// Input stuff. The iterator is for getNextValue()
	list<LogicValue> vals;
	list<LogicValue>::iterator currVal;

	// Unique ID for sorting
	int id;
};

// Comparer for Events used in priority queue
class EventCompare
{
public:
	bool operator()(const Event *rhs, const Event *lhs) const
	{
		if(rhs->getTime() == lhs->getTime())
		{
			return rhs->getID() > lhs->getID();
		}
		else
		{
			return rhs->getTime() > lhs->getTime();
		}
	}
};

#endif
