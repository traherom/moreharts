#ifndef LISTOFPOSINT_H
#define LISTOFPOSINT_H

#include <string>
#include <list>

using namespace std;

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
   bool        operator==(const linkedList &r) const;
   bool        operator!=(const linkedList &r) const;

   // algebraic operators:  concatenate, subtract, union, and intersection
   linkedList  operator+(const linkedList &r) const;
   linkedList  operator-(const linkedList &r) const;
   linkedList  operator|(const linkedList &r) const;
   linkedList  operator&(const linkedList &r) const;

   // accessor function
   int         size() const {return ls.size();}

   // functions converting to arrays and strings
   int*        sequence() const;
   string      str() const;

   // test functions for emptiness and specific elements
   bool        isEmpty() const;
   bool        isElement(int i) const;

   // input/output operators
   friend istream&    operator>>(istream&  in,       linkedList &l);
   friend ostream&    operator<<(ostream& out, const linkedList &l);

private:
   // helper functions
   void        insert(int i);
   int         remove(int i);

   list<int> ls;
};

#endif //LISTOFPOSINT_H
