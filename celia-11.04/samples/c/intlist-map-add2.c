
#include "intlist.h"

// Adds 2 to each element of head.
// Iterative version.

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

