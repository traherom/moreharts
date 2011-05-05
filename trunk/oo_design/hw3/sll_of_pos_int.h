#ifndef SLLOFPOSINT_H
#define SLLOFPOSINT_H

const int HEADOFLIST = -1;

class node {
public:
   int   key;
   node *next;
};

typedef node* linkedList;
typedef node* nodePtr;

linkedList create();
void       destroy(linkedList &l);
void       print(linkedList l);
void       insert(linkedList l, int i);
int        remove(linkedList l, int i);
linkedList copy(linkedList l);
linkedList merge(linkedList l1, linkedList l2);
bool       isEmpty(linkedList l);

#endif //SLLOFPOSINT_H
