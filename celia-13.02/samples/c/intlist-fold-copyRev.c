
#include "intlist.h"

// Create a new list, the reverse of x.
//  Iterative version.

/*@ requires acyclic(x);
 */
intlist fold_copyRev(intlist x) {
    intlist xi, y, z;
    xi = y = z = NULL;
    xi = x;
    while (xi != NULL) {
        z = (intlist) malloc(sizeof (struct intlist_));
        z->data = xi->data;
        z->next = NULL;
        z->next = y;
        y = NULL;
        y = z;
        z = NULL;
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    return y;
}


