
#include "intlist.h"

// Initializes with the sequence starting from v the elements of head.
// Iterative version.

/*@ requires acyclic(head);
 */
void map_initSeq(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = v;
        v = v + 1;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}

