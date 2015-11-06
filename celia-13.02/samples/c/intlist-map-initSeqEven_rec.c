
#include "intlist.h"

// Initializes head with the sequence of even numbers starting from v.
// Recursive version.

/*@ requires acyclic(head);
 */
void map_rec_initSeqEven(intlist head, int v) {
    if (head != NULL) {
        int vv;
        intlist curr;
        head->data = v;
        vv = v + 2;
        curr = NULL;
        curr = head->next;
        map_rec_initSeq(curr, vv);
    }
}


