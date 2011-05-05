// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Implements methods of Car
#include <iostream>
using std::cin;
using std::cout;

#include "car.h"

void Car::print()
{
	// Use parent's print()
	Vehicle::print();
	
	cout << "\tTrim style: " << trim << "\n";
}

void Car::read()
{
	// Use parent's read()
	Vehicle::read();

	cout << "Enter the trim of a " << getName() << ": ";
	cin >> trim;
}
