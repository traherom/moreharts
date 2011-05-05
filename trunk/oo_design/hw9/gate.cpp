// File: gate.cpp
// Author: Ryan Morehart
// Purpose: Implementation for the Gate, Input, and Output classes.

#include <string>
#include <vector>
using std::string;
using std::vector;
using std::pair;

#include "gate.h"
#include "wire.h"
#include "event.h"
#include "circuit.h"
#include "circuit_exception.h"

// Constructor for a Gate
Gate::Gate(string gateType, int gateDelay, Wire *outputWire1, Wire *inputWire1, Wire *inputWire2)
{
	// Ensure type is valid
	char *validTypes[] = { "AND", "OR", "NOT", "INVERTER", "XOR", "NAND", "NOR", "XNOR", NULL };
	bool foundMatch = false;
	int i = 0;
	while(validTypes[i] != NULL)
	{
		// Match?
		if(gateType == validTypes[i])
		{
			// Yep, mark and move on
			foundMatch = true;
			break;
		}
		
		// Next one
		i++;
	}
	
	if(!foundMatch)
	{
		// The gate type wasn't found in the list
		throw new CircuitException(gateType + " not a valid gate type");
	}
	
	// Save gate type. Convert INVERTER to NOT
	type = gateType;
	if(type == "INVERTER")
	{
		type = "NOT";
	}
	
	// Delay
	delay = gateDelay;
	
	// Grab and save Wires
	out = outputWire1;
	in1 = inputWire1;
	in2 = inputWire2;

	// We should always start out Unknown (darn bug solved, hopefully)
	currOutput = Unknown;
}

// The different getOutputs for each type of gate
LogicValue Gate::getOutput()
{
	return currOutput;
}

// Update functions
void Gate::update(Circuit *evQueue, Event *ev)
{
	LogicValue in1Val, in2Val;
		
	// Is there a delay for this gate (and this isn't the call for the actual update)
	if(ev == NULL && delay > 0)
	{
		// Latch the values that the attached wires are currently at
		Event *delayEv = new Event(evQueue->getTime() + delay, this);
		if(in1 != NULL)
		{
			delayEv->addInputVal(in1->getOutput());
		}
		if(in2 != NULL)
		{
			delayEv->addInputVal(in2->getOutput());
		}
		
		// Queue up
		evQueue->addEvent(delayEv);
		return;
	}
	else if(ev == NULL)
	{
		// No delay for this gate, so we can't get the values we need from the event
		if(in1 != NULL)
		{
			in1Val = in1->getOutput();
		}
		if(in2 != NULL)
		{
			in2Val = in2->getOutput();
		}
	}
	else
	{
		// Get values we should operate on from the event
		// If there was only 1 value in the event the second thing will just get Unknown
		in1Val = ev->getNextValue(true);
		in2Val = ev->getNextValue();
	}
	
	// Perform correct logic for this type of gate
	if(type == "AND") // Verified
	{
		if(in1Val == False || in2Val == False)
		{
			currOutput = False;
		}
		else if(in1Val == Unknown || in2Val == Unknown)
		{
			currOutput = Unknown;
		}
		else
		{
			currOutput = toLV(in1Val && in2Val);
		}
	}
	else if(type == "OR") // Verified
	{
		if(in1Val == True || in2Val == True)
		{
			currOutput = True;
		}
		else if(in1Val == False && in2Val == False)
		{
			currOutput = False;
		}
		else
		{
			currOutput = Unknown;
		}
	}
	else if(type == "NOT") // Verified
	{
		// Can't do logic without knowing value
		if(in1Val == Unknown)
		{
			currOutput = Unknown;
		}
		else
		{
			currOutput = toLV(!in1Val);
		}
	}
	else if(type == "XOR") // Verified
	{
		// Can't do logic without knowing one or the other
		if(in1Val == Unknown || in2Val == Unknown)
		{
			currOutput = Unknown;
		}
		else
		{
			currOutput = toLV((in1Val && !in2Val) || (!in1Val && in2Val));
		}
	}
	else if(type == "NOR") // Verified
	{
		if(in1Val == True || in2Val == True)
		{
			currOutput = False;
		}
		else if(in1Val == False && in2Val == False)
		{
			currOutput = True;
		}
		else
		{
			currOutput = Unknown;
		}
	}
	else if(type == "NAND") // Verified
	{
		if(in1Val == False || in2Val == False)
		{
			currOutput = True;
		}
		else if(in1Val == Unknown || in2Val == Unknown)
		{
			currOutput = Unknown;
		}
		else
		{
			currOutput = toLV(!(in1Val && in2Val));
		}
	}
	else if(type == "XNOR") // Verified
	{
		// Can't do logic without knowing one or the other
		if(in1Val == Unknown || in2Val == Unknown)
		{
			currOutput = Unknown;
		}
		else
		{
			currOutput = toLV(!(in1Val && !in2Val) && !(!in1Val && in2Val));
		}
	}
	else
	{
		throw new CircuitException("Invalid gate type " + type + " uncaught at gate creation");
	}

	// Let out wire know we have updated
	out->update(evQueue);
	
	// Add to history
	addHistory(evQueue);
}

// Get input gates
vector<Gate*> Gate::getInputGates()
{
	vector<Gate *> gates;
	if(in1 != NULL)
	{
		gates.push_back(in1);
	}
	if(in2 != NULL)
	{
		gates.push_back(in2);
	}
	return gates;
}

// Tell wire that we have updated. Out here due to forward declaration
void Input::update(Circuit *evQueue, Event *ev)
{
	// Sanity check
	if(ev == NULL)
	{
		throw new CircuitException("Input received NULL as event for update");
	}
	
	// Get value we should go to
	currOutput = ev->getNextValue(true);
	
	// Tell attached wire we have updated
	out->update(evQueue);
	
	// Add to history
	addHistory(evQueue);
}

// Adds the current output of a gate to its history
void Gate::addHistory(Circuit *evQueue)
{
	// Note we use the getOutput so that children do not need to override this if they use some unusual method for
	// determining what their output is (I'm looking at you, Output)
	history.push_back(pair<int, LogicValue>(evQueue->getTime(), getOutput()));
}

vector<Gate*> Output::getInputGates()
{
	vector<Gate *> gates;
	gates.push_back(in1);
	return gates;
}

// This has to be placed out here due to forward declaration
LogicValue Output::getOutput()
{
	return in->getOutput();
}
