
#include "intlist.h"

// Copies all elements of x in y.
// Recursive version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
  @ requires len(x) == len(y);
 */
void map2_rec_copy_eq(intlist x, intlist y) {
    if (x != NULL) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data;
        map2_rec_copy_eq(xi, yi);
	xi = yi = NULL;
    }
}

