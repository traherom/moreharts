// File: main.cpp
// Author: Ryan Morehart
// Purpose: Sets up circuit simulation and UI

#include <string>
#include <iostream>
#include <vector>
#include <cstring>
using std::vector;
using std::cout;
using std::cin;
using std::ios;
using std::atoi;

#include "circuit.h"
#include "circuit_exception.h"

int main(int argc, char **argv)
{
	// Get circuit file name if not provided in command line
	string circuitFN;
	if(argc == 1)
	{
		cout << "What is the circuit file's name? ";
		cin >> circuitFN;
	}
	else
	{
		circuitFN = argv[1];
	}
	
	// Input file name
	string inputFN;
	if(argc == 2)
	{
		// Figure out input file name
		int namePartEnd = circuitFN.find('.');
		inputFN = circuitFN.substr(0, namePartEnd);
		inputFN += "_v" + circuitFN.substr(namePartEnd);
	}
	else
	{
		// Given on command line
		inputFN = argv[2];
	}
	
	try
	{
		// Create circuit
		cout << "Loading " << circuitFN << " and " << inputFN << "... ";
		Circuit *cir = new Circuit(circuitFN, inputFN);
		cout << "done\n";
		
		// Run circuit
		// Just tick until done
		cout << "Running " << cir->getTitle() << "... ";
		while(cir->tick())
		{
			// Random safety. Mostly put in so I could test the SR latch example, as I'm not sure
			// that would ever terminate otherwise. :)
			// This is only a rough limit, as an event could occur at, say, 58, which sets an
			// event for a gate at 61, hence making the simulator not realize where it is until then 
			if(cir->getTime() > 60)
			{
				cout << "safety limit reached (over 60 clock cycles)... ";
				break;
			}
		}
		cout << "done\n";
		
		// Display results
		// Get list of names
		vector<string> inputs, outputs;
		inputs = cir->getInputNames();
		outputs = cir->getOutputNames();
		
		// Display each one
		cout << "\nFinal results:\n";
		vector<LogicValue> hist;
		
		// Inputs
		for(int i = 0; i < inputs.size(); i++)
		{
			cout << "Input " << inputs[i] << ":  \t";
			
			hist = cir->getHistory(inputs[i]);
			for(int t = 0; t < hist.size(); t++)
			{
				if(hist[t] == True)
					//cout << "\xAF";
					cout << "-";
				else if(hist[t] == False)
					cout << "_";
				else
					cout << "x";
			}
			
			cout << "\n";
		}
		
		// Outputs
		for(int i = 0; i < outputs.size(); i++)
		{
			cout << "Output " << outputs[i] << ":\t";
			
			hist = cir->getHistory(outputs[i]);
			for(int t = 0; t < hist.size(); t++)
			{
				if(hist[t] == True)
					//cout << "\xAF"; // topscore (vs. underscore)
					cout << "-";
				else if(hist[t] == False)
					cout << "_";
				else
					cout << "x";
			}
			
			cout << "\n";
		}
		
		// Display a time reference along the bottom.
		cout << "Time:\t\t";
		for(int i = 0; i < hist.size(); i++)
		{
			// I rather like Shomper's output style:
			if(i % 10 != 0)
			{
				cout << i % 10;
			}
			else
			{
				cout << "-";
			}
		}
		cout << "\n";
	}
	catch(FileException *e)
	{
		cout << "\n\nFile error: " << e->toString() << "\n\n";
	}
	catch(CircuitException *e)
	{
		cout << "\n\nError: " << e->toString() << "\n\n";
	}
	
	return 0;
}
