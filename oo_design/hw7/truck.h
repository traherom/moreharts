// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Declares a class Truck
#ifndef TRUCK_H
#define TRUCK_H

#include "vehicle.h"

class Truck : public Vehicle
{
public:
	Truck(string initName = "none", double initMPG = 0, double initTank = 0, double initCargo = 0)
		: Vehicle(initName, initMPG, initTank), cargoCapacity(initCargo) {}
	
	void setCargoCapacity(double newCargo) { cargoCapacity = newCargo; }

	double getCargoCapacity() { return cargoCapacity; }

	virtual void print();
	virtual void read();

private:
	double cargoCapacity;
};

#endif
