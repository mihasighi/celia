#include "intlistp.h"

// Returns a list which adds to head the value at the beginning of head.
// Iterative version.

intlist add(intlist head, int value) {
    intlist curr;
    curr = NULL;
    curr = (intlist) malloc(sizeof (struct intlist_));
    curr->data = value;
    curr->next = NULL;
    curr->next = head;
    return curr;
}
