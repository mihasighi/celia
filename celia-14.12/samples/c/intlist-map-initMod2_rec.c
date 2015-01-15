
#include "intlist.h"

// Initializes head with the sequence of modulo 2.
// Recursive version.

/*@ requires acyclic(head);
 */
void map_rec_initMod2(intlist head, int v) {
    if (head != NULL) {
        intlist curr;
        int k;
        k = 1 - v;
        curr = NULL;
        curr = head->next;
        map_rec_initMod2(curr, k);
    }
}

