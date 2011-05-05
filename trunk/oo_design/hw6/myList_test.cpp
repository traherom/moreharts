// Linked list example

#include <iostream>
#include <time.h>
#include "myList.h"

using namespace std;

int main() {
   void showMenu();

   const int  NUMLISTS = 6;
   linkedList l[NUMLISTS];
   string     carriage_return;
   char       cmd;

   int        i, k, lnum;
   int       *a, A[] = {3, 7, 12, 19, 45, 10, 1, 6};

   linkedList L(A, 8);

   cout << "L is " << L << endl;

   do {
      showMenu();
      cin >> cmd;

      switch (cmd) {
         case 'd' : cin  >> lnum;
                    getline(cin, carriage_return);
                    cout << "Enter a comma-separated list of elements:  \n";
                    cin  >> l[lnum];
                    break;
         case 'P' : for (i = 0; i < NUMLISTS; i++) {
                          cerr << "List " << i << ":  " << l[i];
                    }
                    break;
         case 'i' : cin >> i >> lnum;
                    l[lnum] = l[lnum] + i;
                    break;
         case 'r' : cin >> i >> lnum;
                    l[lnum] = l[lnum] - i;
                    break;
         case 'c' : cin >> i >> lnum;
                    l[i] = l[lnum];
                    break;
         case 'z' : cin >> lnum;
                    cerr << "List " << lnum << " has " << l[lnum].size()
                         << " elements\n";
                    break;
         case '+' : cin >> i >> k >> lnum;
                    l[lnum] = l[i] + l[k];
                    break;
         case '-' : cin >> i >> k >> lnum;
                    l[lnum] = l[i] - l[k];
                    break;
         case '|' : cin >> i >> k >> lnum;
                    l[lnum] = l[i] | l[k];
                    break;
         case '&' : cin >> i >> k >> lnum;
                    l[lnum] = l[i] & l[k];
                    break;
         case 'a' : cin >> lnum;
                    cerr << "List " << lnum << " has an array value of:  [";
                    a = l[lnum].sequence();
                    for (i = 0; i < l[lnum].size(); i++) {
                       cerr << a[i] << " ";
                    }
                    cerr << "]\n";
                    break;
         case 's' : cin >> lnum;
                    cerr << "List " << lnum << " has a string value of: \""
                         << l[lnum].str()   << "\"\n";
                    break;
         case 'e' : cin >> lnum >> i;
                    cerr << "Lists " << lnum << " and " << i << " are"
                         << ((l[lnum] == l[i]) ? " " 
                                               : " not ") 
                         << "equal"  << endl;
                    break;
         case 'n' : cin >> lnum >> i;
                    cerr << "Lists " << lnum << " and " << i << " are"
                         << ((l[lnum] != l[i]) ? " not " 
                                               : " ") 
                         << "equal"  << endl;
                    break;
         case 'l' : cin >> lnum >> i;
                    cerr << "List " << lnum
                         << ((l[lnum].isElement(i)) ? " contains" 
                                                    : " does not contain") 
                         << " element " << i << endl;
                    break;
         case 'y' : cin >> lnum;
                    if (l[lnum].isEmpty()) {
                       cerr << "List " << lnum << " is empty\n";
                    } else {
                         cerr << "List " << lnum << " is not empty\n";
                    }
                    break;
         case 'q' : break;
         default  : cerr << "Letter '" << cmd << "' is not a valid cmd\n";
                    break;
      }
   } while (cmd != 'q');

   return 0;
}

void showMenu() {
   cout << "This program lets you test your linked list implementation\n";
   cout << "Use the following single-letter commands to test:\n";
   cout << "  d #1       - read list from keyboard    ";
   cout << "P          - print all lists\n";
   cout << "  i #1 #2    - insert elem 1 into list 2  ";
   cout << "r #1 #2    - remove elem 1 from list 2\n";
   cout << "  c #1 #2    - copy list 2 into list 1    ";
   cout << "z #1       - get num elements in list\n";
   cout << "  + #1 #2 #3 - merge 1 & 2 into list 3    ";
   cout << "- #1 #2 #3 - remove 2 from 1 into 3 \n";
   cout << "  | #1 #2 #3 - union 1 & 2 into list 3    ";
   cout << "& #1 #2 #3 - intersect 1 & 2 into 3 \n";
   cout << "  a #1       - turn 1 into an array       ";
   cout << "s #1       - turn 1 into a string\n";
   cout << "  e #1 #2    - are lists 1 & 2 are same   ";
   cout << "n #1 #2    - are 1 & 2 are different\n";
   cout << "  y #1       - ask if list 1 is empty     ";
   cout << "l #1 #2    - ask if 1 has element 2\n";
   cout << "  q          - quit the test program\n\n";
   cout << "Command:  ";
}
