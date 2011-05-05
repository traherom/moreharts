// File name: myList.cpp
// Author: Ryan Morehart
// Purpose: Implements a linkedList using an STL list<int> as storage

#include <iostream>
#include <string>
#include <stdlib.h>

using namespace std;

#include "myList.h"

//pre:   none
//post:  an empty linked list is created
linkedList::linkedList() {
}

//pre:   none
//post:  an empty linked list is created
linkedList::linkedList(int i) {
   // insert the integer parameter as the only element
   insert(i);
}

//pre:   array a[] contains only positive integers and size is correct
//post:  a linked list is created containing the integers found in the array a[]
linkedList::linkedList(string str) {
	// action accomplished in helper function in order to make capability
	// available to friend functions, specifically, operator>>
	string num;
	int size = str.size();
	int *a = new int[size];
	int c = 0, pos = 0, i = 0;

	// parse the input string str to build an array of integers
	while (c >= 0) {
		c = str.find(",", pos);
		if (c > 0) {
			num = str.substr(pos, c-pos);
			a[i++] = atoi(num.c_str());
			pos = c + 1;
		}
	}
	num = str.substr(pos, size);
	a[i++] = atoi(num.c_str());

	// Remove existing elements
	ls.clear();

	// insert the elements in the input array one-by-one
	for (int j = 0; j < i; j++) {
		// Use push instead of helper because
		// sorting after insert would be incredibly inefficient
		ls.push_back(a[j]);
	}
	ls.sort();
}

//pre:   array a[] contains only positive integers and size is correct
//post:  a linked list is created containing the integers found in the array a[]
linkedList::linkedList(int a[], int size) {
	// insert the elements in the input array one-by-one
	for (int i = 0; i < size; i++) {
		insert(a[i]);
	}
}

//pre:   none
//post:  a copy of list c is created
linkedList::linkedList(const linkedList &c) {
	// Copy list
	ls = c.ls;
}

// pre:   none
// post:  all elements of the list are released and the list becomes null
linkedList::~linkedList() {
}

//pre:   none
//post:  a copy of list r is created
linkedList& linkedList::operator=(const linkedList &r) {
	// handle self assignment
	if (&r == this) return *this;

	// Copy list
	ls = r.ls;

	return *this;
}

// pre:   none
// post:  true returned if lists have the same elements; otherwise return false
bool linkedList::operator==(const linkedList &r) const {
	// Return if the lists match
	return (ls == r.ls);
}

// pre:   none
// post:  true returned if lists have the same elements; otherwise return false
bool linkedList::operator!=(const linkedList &r) const {
   return !(*this == r);
}

// pre:   l2 is valid linked list
// post:  returned list contains all the elements of the implicit list and l2 
linkedList linkedList::operator+(const linkedList &r) const {
	// Create copy of current list
	linkedList L(*this);

	// Make copy because merge needs non-const
	list<int> tempCopy(r.ls);

	// Merge
	L.ls.merge(tempCopy);
	L.ls.sort();

	return L;
}

// pre:   l2 is valid linked list
// post:  returned list contains elements of implicit list except those in l2
linkedList linkedList::operator-(const linkedList &r) const {
	// Create copy of current list
	linkedList L(*this);

	// Remove pieces of second list
	for(list<int>::const_iterator j = r.ls.begin(); j != r.ls.end(); j++)
	{
		L.remove(*j);
	}

	return L;
}

// pre:   l2 is valid linked list
// post:  returned list contains all unique elements of implicit list and l2
linkedList linkedList::operator|(const linkedList &r) const {
	linkedList L;

	// Combine the two lists
	L = *this + r;

	// Take out non-unique bits
	L.ls.unique();

	return L;
}

// pre:   l2 is valid linked list
// post:  returned list contains only elements in both the implicit list and l2
linkedList linkedList::operator&(const linkedList &r) const {
	// copy the first list
	linkedList L(*this);

	// Remove pieces which aren't in second list
	for(list<int>::const_iterator j = ls.begin(); j != ls.end(); j++)
	{
		if(!r.isElement(*j))
		{
			L.remove(*j);
		}
	}

	return L;
}

//pre:   none
//post:  an array of integers containing the elements of list r is created
int* linkedList::sequence() const {
	// if the list is empty, return a null array
	if (size() == 0) return NULL;

	// allocate a new array to hold the elements
	int *a = new int(size());
	int i = 0;

	// create the array by copying list elements into the new array
	for(list<int>::const_iterator j = ls.begin(); j != ls.end(); j++, i++)
	{
		a[i] = *j;
	}

	return a;
}

//pre:   none
//post:  create a string of comma-separated integers containing elements of r
string linkedList::str() const {
	string outstr = "";
	char   stemp[80];

	// create the string by copying list elements into the new string
	for(list<int>::const_iterator j = ls.begin(); j != ls.end(); j++)
	{
		sprintf(stemp, "%d, ", *j);
		outstr += stemp;
	}
	outstr = outstr.substr(0, outstr.size()-2);

	return outstr;
}

// pre:   l is a valid linked list
// post:  if l is empty (or undefined) return true, else return false
bool linkedList::isEmpty() const {
   return (size() == 0);
}

// pre:   l is a valid linked list
// post:  if l is empty (or undefined) return true, else return false
bool linkedList::isElement(int i) const {
	for(list<int>::const_iterator j = ls.begin(); j != ls.end(); j++)
	{
		if(*j == i)
		{
			return true;
		}
	}
	
	// Didn't hit it, must not be there
	return false;
}

// pre:   i is a positive integer
// post:  i is inserted into the list in ascending numerical order
void linkedList::insert(int i) {
	// Put in
	ls.push_back(i);

	// To be consistent with the old implementation, sort into order
	ls.sort();
}

// pre:   none
// post:  i removed from the list and returned.  if i not in l, then -1 returned
int linkedList::remove(int i) {
	// Find the element to remove 
	for(list<int>::iterator j = ls.begin(); j != ls.end(); j++)
	{
		if(*j == i)
		{
			ls.erase(j);
			return i;
		}
	}

	// Not found
	return -1;
}

// pre:   none
// post:  the elements of the list are printed as a list to standard output  
istream& operator>>(istream& in, linkedList &l) {
   string str;

   // read the input stream
   getline(in, str, '\n');

   // initialize the linked list with the string
   l = linkedList(str);
   
   return in;
}

// pre:   none
// post:  the elements of the list are printed as a list to standard output  
ostream& operator<<(ostream& out, const linkedList &l) {
	// print each node until the end of the list is reached
	out << "<";
	for(list<int>::const_iterator j = l.ls.begin(); j != l.ls.end(); j++)
	{
		out << *j << ", ";
	}

	// backspace over the trailing comma and space before writing the '>'
	out << "\010\010>\n";

	return out;
}
