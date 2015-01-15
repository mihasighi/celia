
#include "intlist.h"

// Clones x in the result.
// Recursive version.

/*@ requires acyclic(x);
 */
intlist fold_rec_clone(intlist x) {
    intlist r, tmp;
    r = tmp = NULL;
    if (x != NULL) {
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = x->data;
        r->next = NULL;
        tmp = x->next;
        x = NULL; /* value passing */
        x = fold_rec_clone(tmp);
        r->next = x;
        x = tmp = NULL;
    }
    return r;
}


