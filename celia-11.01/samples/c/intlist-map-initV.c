
#include "intlist.h"

// Initializes all elements of head with value v.
// Iterative version.

/*@ requires acyclic(head);
 */
void initV(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = v;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

