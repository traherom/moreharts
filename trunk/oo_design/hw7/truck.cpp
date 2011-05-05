// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Implements methods of Truck
#include <iostream>
using std::cin;
using std::cout;

#include "truck.h"

void Truck::print()
{
	// Use parent's print()
	Vehicle::print();

	cout << "\tCargo capacity: " << cargoCapacity << "\n";
}

void Truck::read()
{
	// Use parent's read()
	Vehicle::read();

	cout << "Enter the cargo capacity of a " << getName() << ": ";
	cin >> cargoCapacity;
}
