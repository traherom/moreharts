// File: vehicle.cpp
// Author: Ryan Morehart
// Purpose: Implements methods of Vehicle
#include <iostream>
using std::cin;
using std::cout;

#include "vehicle.h"

void Vehicle::print()
{
	cout << "The " << name << " has the following attributes:\n";
	cout << "\tMPG: " << mpg << "\n";
	cout << "\tTank capacity: " << tankCapacity << "\n";
}

void Vehicle::read()
{
	cout << "What is the name of your vehicle: ";
	cin >> name;
	cout << "Enter the MPG for a " << name << ": ";
	cin >> mpg;
	cout << "Enter the tank capacity (in gallons) for a " << name << ": ";
	cin >> tankCapacity;
}
