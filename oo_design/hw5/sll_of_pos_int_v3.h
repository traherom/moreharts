#ifndef SLLOFPOSINTV3_H
#define SLLOFPOSINTV3_H

#include <string>

using namespace std;

const int HEADOFLIST = -1;

class node {
public:
  int    key;
  node  *next;
};

class linkedList {
public:
   // constructors:  default, conversion (three types), copy
   linkedList();
   linkedList(int i);                 // int to list
   linkedList(int a[], int size);     // int array to list
   linkedList(string str);            // comma-separated string to list
   linkedList(const linkedList &c);

   // destructor
   ~linkedList();

   // default assignment operator 
   linkedList& operator=(const linkedList &r);

   // equality operators
   bool        operator==(const linkedList &r);
   bool        operator!=(const linkedList &r);

   // algebraic operators:  concatenate, subtract, union, and intersection
   linkedList  operator+(const linkedList &r);
   linkedList  operator-(const linkedList &r);
   linkedList  operator|(const linkedList &r);
   linkedList  operator&(const linkedList &r);

   // accessor function
   int         size() {return s;}

   // functions converting to arrays and strings
   int*        sequence();
   string      str();

   // test functions for emptiness and specific elements
   bool        isEmpty();
   bool        isElement(int i);

   // input/output operators
   friend istream&    operator>>(istream&  in,       linkedList &l);
   friend ostream&    operator<<(ostream& out, const linkedList &l);

private:
   // helper functions
   void        insert(int i);
   int         remove(int i);
   void        allocateSentinal();
   void        initWithString(string str);

   node *l;
   int   s;
};

#endif //SLLOFPOSINTV3_H
