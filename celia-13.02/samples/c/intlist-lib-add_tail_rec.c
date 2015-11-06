#include "intlist.h"

/*@ requires acyclic(head);
  @
 */
intlist add_tail_rec(intlist head, int value) {
    intlist curr, tmp;
    tmp = curr = NULL;
    if (head == NULL) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = value;
        curr->next = NULL;
    } else {
	curr = head->next;
        tmp = add_tail_rec(curr, value);
        head->next = NULL;
        head->next = tmp;
        curr = NULL;
        curr = head;
        tmp = NULL;
    }
    return curr;
}

