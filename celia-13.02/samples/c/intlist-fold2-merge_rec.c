
#include "intlist.h"

// Merge two lists in the result.
// If the two lists are sorted, the result is sorted.
// Recursive version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_rec_merge(intlist x, intlist y) {
    intlist r, xi, yi, tmp;
    int v;
    r = xi = yi = tmp = NULL;
    if (x != NULL) {
        if (y != NULL) {
            if (x->data < y->data) {
                v = x->data;
                xi = x->next;
                yi = y;
            } else {
                v = y->data;
                yi = y->next;
                xi = x;
            }
            // add v in front of the recursive call result
            r = (intlist) malloc(sizeof (struct intlist_));
            r->data = v;
            r->next = NULL;
            tmp = fold2_rec_merge(xi, yi);
            r->next = tmp;
        } else {
            // add v in front of the recursive call result
            xi = x->next;
            r = (intlist) malloc(sizeof (struct intlist_));
            r->data = x->data;
            r->next = NULL;
            tmp = fold2_rec_merge(xi, y);
            r->next = tmp;
        }
    } else if (y != NULL) { // add v at the end of r, pointed by ri
        yi = y->next;
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = y->data;
        r->next = NULL;
        tmp = fold2_rec_merge(x, yi);
        r->next = tmp;
    }
    tmp = xi = yi = NULL; /* no more used */
    return r;
}
