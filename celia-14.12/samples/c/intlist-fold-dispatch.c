
#include "intlist.h"

// Builds two lists l and u by dispatching x wrt v.
// Iterative version.

/*@ requires acyclic(x);
 */
void fold_dispatch(intlist x, int v, intlist* xlev, intlist* xgtv) {
    intlist xi, y;
    xi = y = NULL;
    xi = x;
    *xgtv = NULL;
    *xlev = NULL;
    while (xi != NULL) {
        y = xi;
        xi = NULL;
        xi = y->next;
        y->next = NULL;
        if (y->data <= v) {
            y->next = *xlev;
            *xlev = NULL;
            *xlev = y;
        } else {
            y->next = *xgtv;
            *xgtv = NULL;
            *xgtv = y;
        }
        y = NULL;
    }
}

