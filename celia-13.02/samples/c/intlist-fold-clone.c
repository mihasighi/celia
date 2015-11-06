
#include "intlist.h"

// Clones x in the result.
// Iterative version.

/*@ requires acyclic(x);
 */
intlist fold_clone(intlist x) {
    intlist xi, y, yi, z;
    xi = y = yi = z = NULL;
    xi = x;
    while (xi != NULL) {
        z = (intlist) malloc(sizeof (struct intlist_));
        z->data = xi->data;
        z->next = NULL;
        if (y == NULL)
            y = z;
        else
            yi->next = z;
        yi = NULL;
        yi = z;
        z = NULL;
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    yi = NULL;
    return y;
}


