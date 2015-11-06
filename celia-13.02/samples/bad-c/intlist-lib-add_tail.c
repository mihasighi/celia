#include "intlist.h"

// Returns a list which adds to head the value at the end of head.
// Iterative and recursive versions.

/*@ requires acyclic(head);
  @
 */
intlist add_tail(intlist head, int value) {
    intlist curr, h;
    curr = NULL;
    h = NULL;
    h = head;
    while (h != NULL && h->next != NULL) {
        curr = h->next;
        h = NULL;
        h = curr;
        curr = NULL;
    }
    curr = (intlist) malloc(sizeof (struct intlist_));
    curr->data = value;
    curr->next = NULL;
    if (h != NULL) {
        h->next = curr;
        h = NULL;
        h = head;
    } else
        h = curr;
    return h;
}

