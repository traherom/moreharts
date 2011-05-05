// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Implements methods of SportsCar
#include <iostream>
using std::cin;
using std::cout;

#include "sports_car.h"

void SportsCar::print()
{
	// Use parent's print()
	Car::print();

	cout << "\tAcceleration: " << acceleration << "\n";
}

void SportsCar::read()
{
	// Use parent's read()
	Car::read();

	cout << "Enter the acceleration for a " << getName() << ": ";
	cin >> acceleration;
}

