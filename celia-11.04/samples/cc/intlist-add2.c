
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
        curr->data = i;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}

/*@ requires len >= 1;
 */
intlist init_rec(int len) {
    intlist curr,tmp;
    curr = NULL;
    intlist res = NULL;

    curr = (intlist) malloc(sizeof (struct intlist_));
    curr->data = len ;
    curr->next = NULL;
    tmp = NULL;
    len = len - 1;


    if (len >= 2)
	tmp = init_rec(len);
    
    curr->next = tmp;
    res = curr ;

    return res;
}



void
amain ()
{

  intlist head;
  int i;

  head = NULL;
  i=10;
  head = init_rec (i);	//list of 10 integers 0

 // map_add2 (head);

  printf ("Output after Adding 2 to every node of list\n");

}
