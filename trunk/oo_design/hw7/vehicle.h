// File: vehicle.h
// Author: Ryan Morehart
// Purpose: Declares a class Vehicle
#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
using std::string;

class Vehicle
{
public:
	Vehicle(string initName = "none", double initMPG = 0, double initTank = 0)
	{
		name = initName;
		mpg = initMPG;
		tankCapacity = initTank;
	}	
	
	void setName(string newName) { name = newName; }
	void setMPG(double newMPG) { mpg = newMPG; }
	void setTankCapacity(double newTank) { tankCapacity = newTank; }

	string getName() { return name; }
	double getMPG() { return mpg; }
	double getTankCapacity() { return tankCapacity; }

	virtual void print();
	virtual void read();

private:
	string name;
	double mpg;
	double tankCapacity;
};

#endif
