
#include "linked_list.h"


/*@ requires len >= 1;
 */
intlist initn(int len) {
    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}


void amain(){

  intlist h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, curr;
  int len,i;
  h1 = h2 = h3 = h4 = h5 = h6 = h7 =h9 = h8 = NULL;
  h10 = NULL;
  len=10;
  
  h1 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h1;
        h1 = NULL;
        h1 = curr;
        curr = NULL;
    }

  h2 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h2;
        h2 = NULL;
        h2 = curr;
        curr = NULL;
    }
  
  h3 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h3;
        h3 = NULL;
        h3 = curr;
        curr = NULL;
    }
  h4 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h4;
        h4 = NULL;
        h4 = curr;
        curr = NULL;
    }

  h5 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h5;
        h5 = NULL;
        h5 = curr;
        curr = NULL;
    }
  h6 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h6;
        h6 = NULL;
        h6 = curr;
        curr = NULL;
    }
  h7 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h7;
        h7 = NULL;
        h7 = curr;
        curr = NULL;
    }

  h8 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h8;
        h8 = NULL;
        h8 = curr;
        curr = NULL;
    }

  h9 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h9;
        h9 = NULL;
        h9 = curr;
        curr = NULL;
    }

  h10 = NULL;
  curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = h10;
        h10 = NULL;
        h10 = curr;
        curr = NULL;
    }
len =len +1;

}



void
main ()
{

  intlist h1, h2, h3, h4, h5, h6, h7, h8, h9, h10, h11, h12;
  int i;
  h1 = h2 = h3 = h4 = h5 = h6 = h7 =h9 = h8 = NULL;
  h10 = h11 = h12 = NULL;
  i=10;

  h1 = initn(i);h2 = initn(i);h3 = initn(i);h4 = initn(i);h5 = initn(i);
 h6 = initn(i);h7 = initn(i);h8 = initn(i);h9 = initn(i);h10 = initn(i);
 
  i=i+1;


}
