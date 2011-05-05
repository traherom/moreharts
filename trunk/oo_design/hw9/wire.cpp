// File: wire.cpp
// Author: Ryan Morehart
// Purpose: Implementation for the Wire class

#include <vector>
using std::vector;

#include "wire.h"
#include "event.h"
#include "circuit_exception.h"

void Wire::setInputGate(Gate *inGate)
{
	if(in != 0)
	{
		// Already set
		throw new CircuitException("Wire " + name + " already has an input gate associated with it");
	}
	
	in = inGate;
}

void Wire::addOutputGate(Gate *outGate)
{
	outs.push_back(outGate);
}

void Wire::update(Circuit *evQueue, Event *ev)
{
	// Notify all attached that we have updated
	for(int i = 0; i < outs.size(); i++)
	{
		outs[i]->update(evQueue);
	}
	
	// Add to history
	addHistory(evQueue);
}

