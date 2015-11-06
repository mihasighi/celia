#include "intlist.h"

/*@ requires acyclic(head);
  @
 */
intlist add_tail_rec(intlist head, int value) {
    intlist curr;
    curr = NULL;
    if (head == NULL) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = value;
        curr->next = NULL;
    } else {
	curr = head->next;
        head->next = NULL;
        head->next = add_tail_rec(curr, value); /* pb in translating */
        curr = NULL;
        curr = head;
    }
    return curr;
}

