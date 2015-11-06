
#include "intlist.h"

// Initializes head with the sequence of even numbers starting from v.
// Iterative version.

/*@ requires acyclic(head);
 */
void map_initSeqEven(intlist head, int v) {
    intlist h, tmp;
    h = tmp = NULL;
    h = head;
    while (h != NULL) {
        h->data = v;
        v = v + 2;
        tmp = h->next;
        h = NULL;
        h = tmp;
        tmp = NULL;
    }
}


