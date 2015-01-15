
#include "intlist.h"

// Removes from x all elements greater than v.
// Recursive version.

/*@ requires acyclic(x);
 */
intlist fold_rec_delGeV(intlist x, int v) {
    intlist r;
    r = NULL;
    if (x != NULL) {
        intlist tmp;
        tmp = NULL;
        tmp = x->next;
        if (x->data >= v) {
            free(x);
            r = fold_rec_delGeV(tmp, v);
        } else {
            r = x;
	    x = NULL;
            x = fold_rec_delGeV(tmp, v);
            r->next = NULL;
	    r->next = x;
	    x = NULL; /* value passing, no effect */
        }
        tmp = NULL;
    }
    return r;
}

