
#include "linked_list.h"

/*@ requires acyclic(head);
 */
void map_add2(intlist head) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = h->data + 2;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

/*@ requires len >= 1;
 */
intlist init(int len) {
    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = 0;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}

void
main ()
{

  intlist head,hd;
  int i;
  hd = NULL;
  head = NULL;
  i=10;
 // head = init (i);	//list of 10 integers 0
  i=10;
  hd = init (i);
  map_add2 (hd);
  i=i+1;
//  map_add2(hd);
//  i=i+1;
 // printlist (head);

}
