
#include "intlist.h"

// Builds a new list by copying from x all elements greater than v.
// Iterative version.

/*@ requires acyclic(x);
 */
intlist fold_copyGeV(intlist x, int v) {
    intlist z, y, xi;
    z = y = xi = NULL;
    xi = x;
    while (xi != NULL) {
        if (xi->data >= v) {
            z = (intlist) malloc(sizeof (struct intlist_));
            z->data = xi->data;
            z->next = NULL;
            z->next = y;
            y = NULL;
            y = z;
            z = NULL;
        }
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    return y;
}


