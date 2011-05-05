#include <iostream>
#include "sll_of_pos_int.h"

using std::cerr;
using std::endl;

//pre:   none
//post:  l is an empty linked list
linkedList create() {
   // create the sentinal node at the head of the list
   linkedList l = new node;
   l->key       = HEADOFLIST;
   l->next      = NULL;

   return l;
}

// pre:   none
// post:  all elements of l are released and l becomes a null list  
void destroy(linkedList &l) {
   // return if l is an undefined list
   if (l == NULL) return;

   // define pointer current and previous pointers to 'walk' the list
   nodePtr c = l->next;
   nodePtr p = l;

   // walk the list deleting nodes as we go
   while (c != NULL) {
      delete p;
      p = c;
      c = c->next;
   }
   delete p;

   l = NULL;
}

// pre:   l is a valid linked list
// post:  the elements of l are printed as a list to standard output  
void print(linkedList l) {
   // print error and return if l is an undefined list
   if (l == NULL) {
      cerr << "Invalid list\n";
      return;
   }

   // define a pointer to 'walk' the list
   nodePtr c = l->next;

   // print the list
   cerr << "<";
   while (c != NULL) {
      cerr << c->key << ", ";
      c = c->next;
   }

   // erase the last ", " charactrers and append the closing '>' character
   cerr << "\010\010>\n";
}

// pre:   l is a valid linked list
// post:  i is inserted in l in ascending numerical order
void insert(linkedList l, int i) {
   // return if l is an undefined list
   if (l == NULL) return;

   // return if the key is not a positive integer
   if (i < 0) return;

   // define pointer current and previous pointers to 'walk' the list
   nodePtr c = l->next;
   nodePtr p = l;

   // while we've not reached the end of the list and the element to be
   // inserted is still smaller than the current element, walk the list
   while (c != NULL && c->key <= i) {
      c = c->next;
      p = p->next;
   }

   // insert the new element
   p->next = new node;
   p->next->key = i;
   p->next->next = c;
}

// pre:   l is a valid linked list
// post:  i is removed from l and returned.  if i not in l, then -1 is returned
int remove(linkedList l, int i) {
	nodePtr currNode = l;
	nodePtr prevNode = NULL;
	int key;

	// Find the one we want to get rid of
	while(currNode != NULL && currNode->key != i)
	{
		prevNode = currNode;
		currNode = currNode->next;
	}

	// Reached end of node without finding the key to remove
	if(currNode == NULL)
	{
		return -1;
	}

	// Remove currNode
	prevNode->next = currNode->next;
	key = currNode->key;
	delete currNode;
	currNode = 0;
	return key;
}

// pre:   l is a valid linked list
// post:  a copy of l is returned
linkedList copy(linkedList l) {
	// create the new copy
	linkedList L = create();
	nodePtr currNode = l;

	// Loop through old list, creating new nodes as we find them in the old list
	while(currNode != NULL)
	{
		insert(L, currNode->key);
		currNode = currNode->next;
	}

	return L;
}

// pre:   l1 and l2 are valid linked lists
// post:  a merged list containing all the elements of l1 and l2 is returned
linkedList merge(linkedList l1, linkedList l2) {
	// create the new merged list
	linkedList L = create();

	// Loop through the first list, creating new nodes as we find them
	nodePtr currNode = l1;
	while(currNode != NULL)
	{
		insert(L, currNode->key);
		currNode = currNode->next;
	}
	
	// Same for the second list
	currNode = l2;
	while(currNode != NULL)
	{
		insert(L, currNode->key);
		currNode = currNode->next;
	}

   return L;
}

// pre:   l is a valid linked list
// post:  if l is empty (or undefined) return true, else return false
bool isEmpty(linkedList l) {
   return (l == NULL || l->next == NULL);
}

