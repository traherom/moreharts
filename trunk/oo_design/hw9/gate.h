// File: gate.h
// Author: Ryan Morehart
// Purpose: Definition for a Gate, the basic unit in our logic circuit. Addionally contains simple derivatives of it.

#ifndef GATE_H
#define GATE_H

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::pair;

#include "circuit_exception.h"
class Event;
class Circuit;
class Wire;

// Possible values of a logic circuit
enum LogicValue { False = 0, True = 1, Unknown = -1 };

class Gate
{
public:
	Gate(string gateType, int gateDelay, Wire *outputWire1, Wire *inputWire1, Wire *inputWire2 = 0);
	
	// Figures out what this gate should be emitting
	virtual LogicValue getOutput();
	
	// Function to let attached gates notify us when their value updates
	// At that point we may either update our own value or put ourselves in the event queue
	// If instant is provided (and is true), then update immediately. This should really only be used by
	virtual void update(Circuit *evQueue, Event *ev = NULL);
	
	// Return reference to history
	vector< pair<int, LogicValue> > &getHistory() { return history; }
	
	// Gates which are attached to us
	virtual vector<Gate*> getInputGates();
	
	// Accessors
	string getType() { return type; }
	
protected:
	// Constructor mainly available for use by children
	Gate() : type("special"), currOutput(Unknown), in1(NULL),
		in2(NULL), out(NULL), delay(0) {}

	// Utility for converting from bool to LogicValue
	LogicValue toLV(bool t) const { return t ? True : False; }

	// What type of gate is this?
	string type;
	
	// Cache of output.
	LogicValue currOutput;
	
	// History of what it has been. Allows outsiders to take a look easily.
	// Also an easy fuction for adding to this
	void addHistory(Circuit *evQueue);
	vector< pair<int, LogicValue> > history;
	
	// Delay in "nano seconds"
	int delay;
	
	// Wires attached. There are never more than 2 ins and 1 out
	Wire *in1;
	Wire *in2;
	Wire *out;
};

// Special cases of gates
class Input : public Gate
{
public:
	// All an input needs to know about are what it attches to and what it's initial value is
	Input(string gateName, Wire *outputWire1) : name(gateName), out(outputWire1) {};

	virtual LogicValue getOutput() { return currOutput; }

	// Return name set at construction
	string getName() const { return name; }

	// Notify attached wire
	virtual void update(Circuit *evQueue, Event *ev = NULL);

private:
	// Do not allow people to use the input getter, as it makes no sense for us
	virtual vector<Gate*> getInputGates()
		{ throw new CircuitException("getInputGates() called on Input"); }

	// Name of this Input
	string name;

	// Wire attached to us
	Wire *out;
};

class Output : public Gate
{
public:
	// All an input needs to know about are what it attches to and what it's initial value is
	Output(string gateName, Wire *inputWire1) : name(gateName), in(inputWire1) {};

	// Returns
	virtual LogicValue getOutput();

	// Return name set at construction
	string getName() const { return name; }
	
	// Only thing we have to do on an update is save into history
	virtual void update(Circuit *evQueue, Event *ev = NULL) { addHistory(evQueue); }
	
	// Return one input, but needs to be a vector
	virtual vector<Gate *> getInputGates();
	
private:
	// Name of this output
	string name;
	
	// Wire which we are reading in from
	Wire *in;
};

#endif

