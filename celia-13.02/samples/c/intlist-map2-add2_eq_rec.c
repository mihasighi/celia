
#include "intlist.h"

// Initializes each element of y with the corresponding element of x
// plus 2.
// Recursive version.

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_rec_add2_eq(intlist x, intlist y) {
    if (x != NULL) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + 2;
        map2_rec_add2_eq(xi, yi);
    }
}

