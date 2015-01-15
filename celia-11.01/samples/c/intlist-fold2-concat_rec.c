
#include "intlist.h"

// Concatenates two lists in the result.
// Recursive version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_rec_concat(intlist x, intlist y) {
    intlist r;
    r = NULL;
    if (x == NULL) {
        r = fold_clone(y);
    } else {
        intlist tmp, rr;
        tmp = rr = NULL;
        tmp = x->next;
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = x->data;
        r->next = NULL;
        rr = fold2_rec_concat(tmp, y);
        r->next = rr;
        tmp = rr = NULL;
    }
    return r;
}

