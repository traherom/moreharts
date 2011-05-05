// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Declares a class SportsCar
#ifndef SPORTS_CAR_H
#define SPORTS_CAR_H

#include "car.h"

class SportsCar : public Car
{
public:
	SportsCar(string initName = "none", double initMPG = 0, double initTank = 0,
		string initTrim = "eh?", double initAcc = 0)
		: Car(initName, initMPG, initTank, initTrim), acceleration(initAcc) {}
	
	void setAcceleration(double newAcc) { acceleration = newAcc; }

	double getAcceleration() { return acceleration; }

	virtual void print();
	virtual void read();

private:
	double acceleration;
};

#endif
