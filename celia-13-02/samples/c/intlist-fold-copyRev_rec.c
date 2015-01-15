
#include "intlist.h"

// Create a new list, the reverse of x
// Recursive version.

/*@ requires acyclic(x);
 */
intlist fold_rec_copyRev_aux(intlist x, intlist nxt) {
    intlist z, y, r;
    r = y = z = NULL;
    z = (intlist) malloc(sizeof (struct intlist_));
    z->data = x->data;
    z->next = NULL;
    z->next = nxt;
    y = x->next;
    r = fold_rec_copyRev_aux(y, x);
    return r;
}

intlist fold_rec_copyRev(intlist x) {
    intlist r, nxt;
    r = nxt = NULL;
    r = fold_rec_copyRev_aux(x, nxt);
    return r;
}

