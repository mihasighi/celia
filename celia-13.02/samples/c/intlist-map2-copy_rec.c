
#include "intlist.h"

// Copies all elements of x in y.
// Recursive version.

/*@ requires acyclic(src) && acyclic(dst) && disjoint(src,dst);
 */
void map2_rec_copy(intlist dst, intlist src) {
    if (dst != NULL)
    if (src != NULL) {
        intlist srci, dsti;
        srci = dsti = NULL;
        srci = src->next;
        dsti = dst->next;
        dst->data = src->data;
        map2_rec_copy(dsti, srci);
        srci = dsti = NULL;
    }
}


