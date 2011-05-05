// File: circuit.cpp
// Author: Ryan Morehart
// Purpose: Implements a Circuit

#include <fstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
using std::ifstream;
using std::atoi;
using std::strtok;
using std::string;
using std::vector;
using std::pair;
using std::priority_queue;

#include "circuit.h"
#include "gate.h"
#include "wire.h"
#include "circuit_exception.h"
#include "event.h"

Circuit::Circuit(string circuitFileName, string inputFileName)
{
	// Open files
	ifstream circuitFile(circuitFileName.c_str());
	if(circuitFile.fail())
	{
		throw new FileException("Circuit file " + circuitFileName + " not found");
	}
	
	ifstream inputFile(inputFileName.c_str());
	if(circuitFile.fail())
	{
		throw new FileException("Input file " + inputFileName + " not found");
	}
	
	// Stuff needed for processing
	char line[100];
	const char SEPS[] = " \n";
	
	// Get title. Ignore the initial word, which should be just CIRCUIT
	string marker;
	circuitFile.getline(line, 100);
	marker = strtok(line, SEPS);
	title = strtok(NULL, SEPS);
	
	// Check format
	if(marker != "CIRCUIT")
	{
		throw new FileException("Circuit file " + circuitFileName + " is not in the correct format");
	}
	
	// Now just get each line and add to circuit
	while(!circuitFile.eof())
	{
		// Read in the entire line. This helps in detecting the end of the file, where there could be 
		// a blank line. A hassle, I know.
		circuitFile.getline(line, 100);
		if(strlen(line) == 0)
		{
			break;
		}

		// Start breaking up and get the keyword
		string key;
		key = strtok(line, SEPS);
		
		// Handle different types
		if(key == "INPUT")
		{
			// Get parameters
			string name, wireName;
			name = strtok(NULL, SEPS);
			wireName = strtok(NULL, SEPS);
			
			// Find the wire this attaches to
			Wire *outWire = findWire(wireName);
			
			// Create and add to circuit
			Input *newIn = new Input(name, outWire);
			ins.push_back(newIn);
			
			outWire->setInputGate(newIn);
		}
		else if(key == "OUTPUT")
		{
			// Get parameters
			string name, wireName;
			name = strtok(NULL, SEPS);
			wireName = strtok(NULL, SEPS);
			
			// Find the wire this attaches to
			Wire *inWire = findWire(wireName);
			
			// Create and add to circuit
			Output *newOut = new Output(name, inWire);
			outs.push_back(newOut);
	
			inWire->addOutputGate(newOut);
		}
		else if(key == "NOT" || key == "INVERTER")
		{
			// NOTs are special because they only have 1 in wire
			// Get parameters
			string outWireName, inWireName, delayStr;
			delayStr = strtok(NULL, SEPS);
			inWireName = strtok(NULL, SEPS);
			outWireName = strtok(NULL, SEPS);
			
			int delay;
			delay = atoi(delayStr.c_str());
			
			// Find the wires attached
			Wire *outWire = findWire(outWireName);
			Wire *inWire = findWire(inWireName);
			
			// Create and add to circuit
			Gate *newGate = new Gate(key, delay, outWire, inWire);
			gates.push_back(newGate);
			
			outWire->setInputGate(newGate);
			inWire->addOutputGate(newGate);
		}
		else
		{
			// Assume it's a gate (the class will handle error if it isn't supported)
			// Get parameters
			string outWireName, inWireName1, inWireName2, delayStr;
			delayStr = strtok(NULL, SEPS);
			inWireName1 = strtok(NULL, SEPS);
			inWireName2 = strtok(NULL, SEPS);
			outWireName = strtok(NULL, SEPS);
			
			int delay;
			delay = atoi(delayStr.c_str());
			
			// Find the wires this attaches to
			Wire *outWire = findWire(outWireName);
			Wire *inWire1 = findWire(inWireName1);
			Wire *inWire2 = findWire(inWireName2);
			
			// Create and add to circuit
			Gate *newGate = new Gate(key, delay, outWire, inWire1, inWire2);
			gates.push_back(newGate);
			
			outWire->setInputGate(newGate);
			inWire1->addOutputGate(newGate);
			inWire2->addOutputGate(newGate);
		}
	}
	
	// This constructor really needs to be broken up :)
	// Ignore the first line, who cares
	inputFile.getline(line, 100);
	marker = strtok(line, SEPS);
	
	// Check format
	if(marker != "VECTOR")
	{
		throw new FileException("Input file " + inputFileName + " is not in the correct format");
	}
	if(title != strtok(NULL, SEPS))
	{
		// Titles don't match
		throw new FileException("Input file " + inputFileName
			+ " does not correspond to circuit file " + circuitFileName + "(titles do not match)");
	}
	
	// Now just get each line and add to circuit
	while(!inputFile.eof())
	{
		// Read in the entire line. This helps in detecting the end of the file, where there could be 
		// a blank line. A hassle, I know.
		inputFile.getline(line, 100);
		if(strlen(line) == 0)
		{
			break;
		}

		// Start breaking up and get the keyword (should always be INPUT)
		string key;
		key = strtok(line, SEPS);
		if(key != "INPUT")
		{
			throw new FileException("Input file " + inputFileName + " is not in the correct format");
		}
		
		// Get parameters		
		string name, time, value;
		name = strtok(NULL, SEPS);
		time = strtok(NULL, SEPS);
		value = strtok(NULL, SEPS);
		
		// Add to queue. We manually add it rather that use addEvent() because
		// we have to specify the input value explicitly
		Input *in = findInput(name);
		
		LogicValue logVal;
		if(value == "0")
			logVal = False;
		else if(value == "1")
			logVal = True;
		else
			logVal = Unknown;

		Event *ev = new Event(atoi(time.c_str()), in);
		ev->addInputVal(logVal);
		
		// Stick on queue
		events.push(ev);
	}
	
	// Close files
	circuitFile.close();
	inputFile.close();
}

Circuit::~Circuit()
{
	// Delete everything. Note that none of the contained class do this for us
	// because they are all so inter-dependent.
	// TBD
}

// Push an event onto the queue
void Circuit::addEvent(Event *ev)
{
	// Stick on queue
	events.push(ev);
}

// Advance time and run all events that are queued for that moment
// Returns true as long as there are more events to process
bool Circuit::tick()
{
	// Pop next thing off
	Event *ev = events.top();
	events.pop();
	
	// The current time is whatever was just popped off
	currTime = ev->getTime();
	
	// Allow the gate to update as it wants
	ev->getGate()->update(this, ev);
	delete ev;
	
	// Done? (no more events)
	return !events.empty();
}

// Return only names of inputs
vector<string> Circuit::getInputNames()
{
	vector<string> names;
	
	// Add each name to list
	for(int i = 0; i < ins.size(); i++)
	{
		names.push_back(ins[i]->getName());
	}
	
	return names;
}

// Return only names of outputs
vector<string> Circuit::getOutputNames()
{
	vector<string> names;
	
	// Add each name to list
	for(int i = 0; i < outs.size(); i++)
	{
		names.push_back(outs[i]->getName());
	}
	
	return names;
}

// Returns history for the given input or output value (searches both lists)
// Does some processing on the histories to make them continuous from 0-currTime
vector<LogicValue> Circuit::getHistory(string name)
{
	// Find input/output
	Gate *histGate = NULL;
	for(int i = 0; i < outs.size(); i++)
	{
		if(outs[i]->getName() == name)
		{
			// Found the gate
			histGate = outs[i];
		}
	}
	
	// Only check second list if we didn't find it in the first
	if(histGate == NULL)
	{
		for(int i = 0; i < ins.size(); i++)
		{
			if(ins[i]->getName() == name)
			{
				// Found the gate
				histGate = ins[i];
			}
		}
	}
	
	// Get history
	vector< pair<int, LogicValue> > hist;
	hist = histGate->getHistory();
	
	// Process into nice list
	vector<LogicValue> niceHist;
	int oldTime = 0;
	int newTime = 0;
	LogicValue val = Unknown;
	
	// Loop through each history item. For each one we encounter, count from the previous time
	// to the new one, inserting the old value as many times as we have to.
	for(int i = 0; i < hist.size(); i++)
	{
		newTime = hist[i].first;
		
		// Insert values into nice list until we reach this time
		for(int j = oldTime; j < newTime; j++)
		{
			niceHist.push_back(val);
		}
		
		oldTime = newTime;
		val = hist[i].second;
	}
	
	// Finish off to the current time, as the last event probably wasn't in the last clock cycle.
	for(int i = oldTime; i <= currTime; i++)
	{
		niceHist.push_back(val);
	}
	
	return niceHist;
}

// Utility for finding/creating stuff
Wire *Circuit::findWire(string name)
{
	// Look at every wire until we find the one we want
	for(int i = 0; i < wires.size(); i++)
	{
		if(wires[i]->getName() == name)
		{
			// Found!
			return wires[i];
		}
	}
	
	// Not found, create and add to list
	Wire *newWire = new Wire(name);
	wires.push_back(newWire);
	return newWire;
}

Input *Circuit::findInput(string name)
{
	// Look at every wire until we find the one we want
	for(int i = 0; i < ins.size(); i++)
	{
		if(ins[i]->getName() == name)
		{
			// Found!
			return ins[i];
		}
	}
	
	// Not found, create and add to list
	throw new FileException("The input file references an input that was not specified in the circuit definition");
}
