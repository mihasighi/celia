
#include "intlist.h"

// Builds a new list by copying from x all elements greater than v.
// Recursive version.

/*@ requires acyclic(x);
 */
intlist fold_rec_copyGeV(intlist x, int v) {
    intlist y, z, tmp;
    y = z = tmp = NULL;
    if (x != NULL) {
        z = x->next;
        if (x->data >= v) {
            y = (intlist) malloc(sizeof (struct intlist_));
            y->data = x->data;
            y->next = NULL;
            tmp = fold_rec_copyGeV(z, v);
            y->next = tmp;
	    tmp = NULL; /* no more used */
        } else
            y = fold_rec_copyGeV(z, v);
	z = NULL; /* no more used */
    }
    return y;
}

