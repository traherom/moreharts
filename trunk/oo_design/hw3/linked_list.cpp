// Linked list example

#include <iostream>
#include "sll_of_pos_int.h"

using namespace std;

void showMenu();

int main() {

   const int  NUMLISTS    = 10;
   linkedList l[NUMLISTS] = {0};
   char       cmd;

   int        i, k, lnum;

   do {
      showMenu();
      cin >> cmd;

      switch (cmd) {
         case 'c' : i = 0;
                  while (i < NUMLISTS && l[i] != NULL) i++;
                  if (i == NUMLISTS) {
                     cerr << "No more lists may be created\n";
                     break;
                  }
                  l[i] = create();
                  break;
         case 'd' : cin >> lnum;
                  destroy(l[lnum]);
                  break;
         case 'p' : cin >> lnum;
                  cerr << "List " << lnum << ":  ";
                  print(l[lnum]);
                  break;
         case 'P' : for (i = 0; i < NUMLISTS; i++) {
                     if (l[i] != NULL) {
                         cerr << "List " << i << ":  ";
                        print(l[i]);
                     }
                  }
                  break;
         case 'i' : cin >> i >> lnum;
                  insert(l[lnum], i);
                  break;
         case 'r' : cin >> i >> lnum;
                  if (remove(l[lnum], i) < 0) {
                     cerr << i << " not in list " << lnum << endl;
                  }
                  break;
         case 'y' : cin >> i >> lnum;
                  if (lnum == i) {
                     break;
                  }
                  destroy(l[i]);
                  l[i] = copy(l[lnum]);
                  break;
         case 'm' : cin >> i >> k >> lnum;
                  if (lnum == i && lnum == k) {
                     linkedList ltemp = copy(l[lnum]);
                     destroy(l[lnum]);
                     l[lnum] = merge(ltemp, ltemp);
                  } else if (lnum == i) {
                     linkedList ltemp = copy(l[lnum]);
                     destroy(l[lnum]);
                     l[lnum] = merge(ltemp, l[k]);
                  } else if (lnum == k) {
                     linkedList ltemp = copy(l[lnum]);
                     destroy(l[lnum]);
                     l[lnum] = merge(l[i], ltemp);
                  } else {
                     destroy(l[lnum]);
                     l[lnum] = merge(l[i], l[k]);
                  }
                  break;
         case 'e' : cin >> lnum;
                  if (isEmpty(l[lnum])) {
                     cerr << "List " << lnum << " is empty\n";
                  } else {
                     cerr << "List " << lnum << " is not empty\n";
                  }
                  break;
         case 'q' : break;
         default  : cout << "The letter '" << cmd << "' is not a valid cmd\n";
                  break;
      }
   } while (cmd != 'q');

   return 0;
}

void showMenu() {
   cout << "This program lets you test your linked list implementation\n";
   cout << "Use the following single-letter commands to test:\n";
   cout << "  c          - create a list              ";
   cout << "d #        - destroy list #\n";
   cout << "  p #        - print list #               ";
   cout << "P          - print all lists\n";
   cout << "  i #1 #2    - insert elem 1 into list 2  ";
   cout << "r #1 #2    - remove elem 1 from list 2\n";
   cout << "  y #1 #2    - copy list 2 into list 1    ";
   cout << "m #1 #2 #3 - merge list 1 & 2 into 3\n";
   cout << "  e #        - ask if list # is empty     ";
   cout << "q          - quit the test program\n\n";
   cout << "Command:  ";
}
