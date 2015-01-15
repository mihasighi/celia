
#include "intlist.h"

// Initializes with the sequence starting from v the elements of head.
// Recursive version.

/*@ requires acyclic(head);
 */
void map_rec_initSeq(intlist head, int v) {
    if (head != NULL) {
        int vv;
        intlist curr;
        head->data = v;
        vv = v + 1;
        curr = NULL;
        curr = head->next;
        map_rec_initSeq(curr, vv);
    }
}

