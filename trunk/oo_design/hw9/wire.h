// File: wire.h
// Author: Ryan Morehart
// Purpose: Definition of a Wire, which is a specialized Gate with a variable number of outs 

#ifndef WIRE_H
#define WIRE_H

#include <vector>
using std::vector;

#include "gate.h"
#include "event.h"

class Wire : public Gate
{
public:
	Wire(string wireName) : name(wireName) {}
	
	LogicValue getOutput() { return in->getOutput(); }
	string getName() { return name; }
	
	// Allow configuring after construction.
	void setInputGate(Gate *inGate);
	void addOutputGate(Gate *inGate);
	
	// Wires have 0 delay and just pass the attached through, so just notify attached immediately
	virtual void update(Circuit *evQueue, Event *ev = NULL);
	
private:
	string name;

	Gate *in;
	vector<Gate*> outs;
};

#endif
