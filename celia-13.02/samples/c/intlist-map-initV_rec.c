
#include "intlist.h"

// Initializes all elements of head with value v.
// Recursive version.

/*@ requires acyclic(head);
 */
void map_rec_initV(intlist head, int v) {
    if (head != NULL) {
        intlist h = NULL;
        head->data = v;
        h = head->next;
        map_rec_initV(h, v);
    }
}

