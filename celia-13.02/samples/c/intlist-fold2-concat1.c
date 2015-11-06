
#include "intlist.h"

// Concatenates two lists in the result (in place).

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_concat1(intlist x, intlist y) {
    intlist r, xi, xl;
    r = xi = xl = NULL;
    if (x == NULL)
        r = y;
    else if (y == NULL)
        r = x;
    else { // x ad y are not empty
        // link the last element of x to y
        // see end of x
        xl = x;
        xi = x->next;
        while (xi != NULL) {
            xl = NULL;
            xl = xi;
            xi = NULL;
            xi = xl->next;
        }
        xl->next = y;
        xl = NULL; /* no more used after */
        r = x;
    }
    return r;
}

