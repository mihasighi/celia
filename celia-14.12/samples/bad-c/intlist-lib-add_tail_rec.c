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
    } else if (head->next == NULL) { /* no constraints with next dereferencing */
        head->next = add_tail_rec(curr, value); /* pb in translating */
        curr = head;
    } else {
        curr = head->next;
        head->next = NULL;
        head->next = add_tail_rec(curr, value);
        curr = NULL;
        curr = head;
    }
    return curr;
}

