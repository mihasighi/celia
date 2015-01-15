
#include "intlist.h"

// Initializes each element of y with the corresponding element of x
// plus v.
// Recursive version.

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_rec_addV(intlist x, intlist y, int v) {
    if (x != NULL)
    if (y != NULL) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + v;
        map2_rec_addV(xi, yi, v);
    }
}

