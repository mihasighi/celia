
#include "intlist.h"

// Returns 1 if two lists are equal
// Recursive version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
int fold2_rec_equal(intlist x, intlist y) {
    if (x != NULL) {
        if (y != NULL) {
            if (x->data != y->data)
                return 0;
            intlist xi, yi;
            int r;
            xi = yi = NULL;
            xi = x->next;
            yi = y->next;
            r = fold2_rec_equal(xi, yi);
            return r;
        }
    } else if (/* x == NULL && */ y == NULL)
        return 1;
    return 0;
}

