// File: circuit.h
// Author: Ryan Morehart
// Purpose: Definition for Circuit, which controls a simulated circuit.

#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <string>
#include <vector>
#include <queue>
using std::string;
using std::vector;
using std::priority_queue;

#include "gate.h"
#include "wire.h"
#include "event.h"

class Circuit
{
public:
	Circuit(string circuitFileName, string inputFileName);
	~Circuit();
	
	// Add an event to the queue. Takes a gate to update and an offset into the future.
	//void addEvent(Gate *updateGate, int delay);
	void addEvent(Event *ev);
	
	// Advance time
	bool tick();
	
	// Random accessors for UI
	int getTime() { return currTime; }
	string getTitle() { return title; }
	
	// Return only names of inputs and outputs
	vector<string> getInputNames();
	vector<string> getOutputNames();
	
	// Returns history for the given input or output value (searches both lists)
	// Does some processing on the histories to make them continuous from 0-currTime
	// This is in part for ease of use and in part because the way the history is defined is
	// freakin' scary. Seriously, who came up with that?
	vector<LogicValue> getHistory(string name);
	
private:
	// Utilities
	Wire *findWire(string name);
	Input *findInput(string name);
	
	// Metadata
	string title;
	
	// Event queue
	int currTime;
	priority_queue<Event*, vector<Event*>, EventCompare> events;
	
	// Nuts and bolts. Not really needed after event queue gets started, but useful for the
	// controller of Circuit (ie, main()) to see state of things
	vector<Wire*> wires;
	vector<Gate*> gates;
	vector<Input*> ins;
	vector<Output*> outs;
};

#endif
