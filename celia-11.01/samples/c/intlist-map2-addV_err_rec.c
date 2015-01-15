
#include "intlist.h"

// Initializes each element of y with the corresponding element of x
// plus v.
// Does not test the length of the list.
// Recursive version.

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_rec_addV_err(intlist x, intlist y, int v) {
    if (x != NULL /* && y != NULL */ ) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + v;
        map2_rec_addV_err(xi, yi, v);
    }
}

