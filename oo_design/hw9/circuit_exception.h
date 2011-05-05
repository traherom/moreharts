// File: circuit_exception.h
// Author: Ryan Morehart
// Purpose: Really basic exception class(es)

#ifndef CIRCUIT_EXCEPTION_H
#define CIRCUIT_EXCEPTION_H

class CircuitException
{
public:
	CircuitException(string s) : desc(s) {}
	
	string toString() { return desc; }
	
private:
	string desc;
};

class FileException : public CircuitException
{
public:
	FileException(string s) : CircuitException(s) {}
};

#endif
