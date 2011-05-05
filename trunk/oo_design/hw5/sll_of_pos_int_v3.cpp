#include <iostream>
#include <string>
#include <sstream>
#include <stdlib.h>

using namespace std;

#include "sll_of_pos_int_v3.h"

//pre:   none
//post:  an empty linked list is created
linkedList::linkedList() : s(0) {
   // allocate a new node as a sentinal
   allocateSentinal();
}

//pre:   none
//post:  an empty linked list is created
linkedList::linkedList(int i) : s(0) {
   // allocate a new node as a sentinal
   allocateSentinal();

   // insert the integer parameter as the only element
   insert(i);
}

//pre:   array a[] contains only positive integers and size is correct
//post:  a linked list is created containing the integers found in the array a[]
linkedList::linkedList(string str) : s(0) {
   // action accomplished in helper function in order to make capability
   // available to friend functions, specifically, operator>>
   initWithString(str);
}

//pre:  f array a[] contains only positive integers and size is correct
//post:  a linked list is created containing the integers found in the array a[]
linkedList::linkedList(int a[], int size) : s(0) {
	allocateSentinal();
	
	// Insert each element of array into linked list
	for(int i = 0; i < size; i++)
	{
		insert(a[i]);
	}
}

//pre:   none
//post:  a copy of list c is created
linkedList::linkedList(const linkedList &c) : s(0) {
   // allocate a new node as a sentinal
   allocateSentinal();

   // create copy by inserting elements into the new list based on the copy
   node *p = c.l->next;
   while (p != NULL) {
      insert(p->key);
      p = p->next;
   }
}

// pre:   none
// post:  all elements of the list are released and the list becomes null
linkedList::~linkedList() {
   // reset the size
   s = 0;

   // set up the pointers to "walk" the list
   node *c = l->next;
   node *p = l;

   // delete each node in the list, including the sentinal
   while (c != NULL) {
      delete p;
      p = c;
      c = c->next;
   }
   delete p;
}

//pre:   none
//post:  a copy of list r is created
linkedList& linkedList::operator=(const linkedList &r) {
   // handle self assignment
   if (&r == this) return *this;

   // watch for memory leak here
   this->~linkedList();

   // reset the size
   s = 0;

   // allocate a new node as a sentinal
   allocateSentinal();

   // create copy by inserting elements into the new list based on the copy
   node *p = r.l->next;
   while (p != NULL) {
      insert(p->key);
      p = p->next;
   }

   return *this;
}

// pre:   none
// post:  true returned if lists have the same elements; otherwise return false
bool linkedList::operator==(const linkedList &r) {
   // Loop through each element of both listsensuring they are the same
   node *p1 = l->next;
   node *p2 = r.l->next;
   while (p1 != NULL && p2 != NULL) {
      if(p1->key != p2->key)
      {
          return false;
      }

      p1 = p1->next;
      p2 = p2->next;
   }

   // Make sure we didn't exit the loop because we hit the end of just one
   // of the lists. If we hit both at the same time it's fine (actually,
   // that's how it should be)
   return (p1 == NULL && p2 == NULL);
}

// pre:   none
// post:  true returned if lists have the same elements; otherwise return false
bool linkedList::operator!=(const linkedList &r) {
   return !(*this == r);
}

// pre:   l2 is valid linked list
// post:  returned list contains all the elements of the implicit list and l2 
linkedList linkedList::operator+(const linkedList &r) {
   // copy the first list using the constructor
   linkedList* L = new linkedList(*this);

   // add the elements of the second list using insert()
   node *c = r.l->next;
   while (c != NULL) {
      L->insert(c->key);
      c = c->next;
   }

   return *L;
}

// pre:   l2 is valid linked list
// post:  returned list contains elements of implicit list except those in l2
linkedList linkedList::operator-(const linkedList &r) {
   // copy the first list using the constructor
   linkedList* L = new linkedList(*this);

   // remove the elements of the second list using remove()
   node *c = r.l->next;
   while (c != NULL) {
      L->remove(c->key);
      c = c->next;
   }

   return *L;
}

// pre:   l2 is valid linked list
// post:  returned list contains all unique elements of implicit list and l2
linkedList linkedList::operator|(const linkedList &r) {
	// copy the first list using the constructor
	linkedList* L = new linkedList(*this);

	// Copy all the elements of the second list which don't already exist in L
   node *c = r.l->next;
	while (c != NULL) {
		if(!L->isElement(c->key))
		{
			L->insert(c->key);
		}
		c = c->next;
	}

	return *L;
}

// pre:   l2 is valid linked list
// post:  returned list contains only elements in both the implicit list and l2 
linkedList linkedList::operator&(const linkedList &r) {
   // copy the first list using the constructor
   linkedList* L = new linkedList(*this);
   int         key;

   // remove the elements of the first list that don't appear in the second list
   // note:  cast required to remove constness of r to call member function
   node *c = L->l->next;
   while (c != NULL) {
      key = c->key;
      c   = c->next; // why advance c here not at end of loop? diagram it to see
      if (!(const_cast<linkedList&> (r)).isElement(key)) {
         L->remove(key);
      }                  
   }

   return *L;
}

//pre:   none
//post:  an array of integers containing the elements of list r is created
int* linkedList::sequence() {
   // if the list is empty, return a null array
   if (s == 0) return NULL;

   // allocate a new array to hold the elements
   int *a = new int(s);
   int  i = 0;

   // create the array by copying list elements into the new array
   node *p = this->l->next;
   while (p != NULL) {
      a[i++] = p->key;
      p = p->next;
   }

   return a;
}

//pre:   none
//post:  create a string of comma-separated integers containing elements of r 
string linkedList::str() {
	string s;
	
	// Loop through each
	node *p = l->next;
	while (p != NULL) {
		stringstream out;
		out << p->key;
		s += ", " + out.str();

		p = p->next;
	}

	// Remove initial ", " and return
	// Watch for a list with nothing in it
	if(s.length() > 0)
	{
		return s.substr(2);
	}
	else
	{
		return "";
	}
}

// pre:   l is a valid linked list
// post:  if l is empty (or undefined) return true, else return false
bool linkedList::isEmpty() {
   return (l->next == NULL);
}

// pre:   l is a valid linked list
// post:  if i is in l, return true. Otherwise false.
bool linkedList::isElement(int i) {
	// Loop through each
	node *p = l->next;
	while (p != NULL) {
		// Is this a match?
		if(p->key == i)
		{
			// Woohoo!
			return true;
		}
		
		p = p->next;
	}
	
	// Didn't find
	return false;
}

// pre:   i is a positive integer
// post:  i is inserted into the list in ascending numerical order
void linkedList::insert(int i) {
   // make sure the element is positive
   if (i <= 0) {
      cerr << "Inserted element " << i << " is not positive\n";
      return;
   }

   // set up the pointers to "walk" the list
   node *p = l;
   node *c = l->next;

   // find the correct insertion point
   while (c != NULL && c->key <= i) {
      c = c->next;
      p = p->next;
   }

   // insert the new element
   p->next = new node;
   p->next->key = i;
   p->next->next = c;
   s++;
}

// pre:   none
// post:  i removed from the list and returned.  if i not in l, then -1 returned
int linkedList::remove(int i) {
   // set up the pointers to "walk" the list
   node *p = l;
   node *c = l->next;

   // "walk" the list looking for the requested element
   while (c != NULL && c->key != i) {
      c = c->next;
      p = p->next;
   }

   // If we don't hit the end of the list the element is there--delete it
   if (c != NULL) {
      p->next = c->next;
      delete c;
      s--;
      return i;
   } else {         // otherwise, return -1 (not found)
      return -1;
   }
}

//pre:   array a[] contains only positive integers and size is correct
//post:  a linked list is created containing the integers found in the array a[]
void linkedList::allocateSentinal() {
   l = new node;
   l->key = HEADOFLIST;
   l->next = NULL;
}

//pre:   str is a comma-separated list of positive integers
//post:  all integers in str are inserted in the list
void linkedList::initWithString(string str) {
   string num;
   int     size = str.size();
   int    *a = new int[size];
   int     c = 0, pos = 0, i = 0;

   s = 0;

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

   // allocate a new node as a sentinal
   allocateSentinal();

   // insert the elements in the input array one-by-one
   for (int j = 0; j < i; j++) {
      insert(a[j]);
   }
}

// pre:   none
// post:  the elements of the list are read in as a list from in 
istream& operator>>(istream& in, linkedList &l) {
	// Get string
	string elementStr;	
	getline(in, elementStr);
	
	l.initWithString(elementStr);
	
	return in;
}

// pre:   none
// post:  the elements of the list are printed as a list to standard output  
ostream& operator<<(ostream& out, const linkedList &l) {
   // set up a pointer to "walk" the list
   node *c = l.l->next;

   // print each node until the end of the list is reached
   out << "<";
   while (c != NULL) {
      out << c->key << ", ";
      c = c->next;
   }

   // backspace over the trailing comma and space before writing the '>'
   out << "\010\010>\n";

   return out;
}
