
#include "intlist.h"

// Adds to each element of head the value v.
// Iterative version.

/*@ requires acyclic(head);
 */
void map_addV(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = h->data + v;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

