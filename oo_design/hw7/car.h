// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Declares a class Car
#ifndef CAR_H
#define CAR_H

#include <string>
using std::string;

#include "vehicle.h"

class Car : public Vehicle
{
public:
	Car(string initName = "none", double initMPG = 0, double initTank = 0,
		string initTrim = "eh?")
		: Vehicle(initName, initMPG, initTank), trim(initTrim) {}
	
	void setTrim(string newTrim) { trim = newTrim; }

	string getTrim() { return trim; }

	virtual void print();
	virtual void read();

private:
	string trim;
};

#endif
