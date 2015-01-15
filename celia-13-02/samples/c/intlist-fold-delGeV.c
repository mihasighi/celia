
#include "intlist.h"

// Removes from x all elements greater than v.
// Iterative version.

/*@ requires acyclic(x);
 */
intlist fold_delGeV(intlist x, int v) {
    intlist z, tmp, xip, xi;
    z = tmp = xip = xi = NULL;
    xi = x;
    while (xi != NULL) {
        if (xi->data >= v) {
            z = xi;
            tmp = xi->next;
            free(z);
            z = NULL;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
            if (xip == NULL) {
                x = NULL;
                x = xi;
            } else {
                xip->next = NULL;
                xip->next = xi;
            }
        } else {
            xip = NULL;
            xip = xi;
            xi = NULL;
            xi = xip->next;
        }
    }
    return x;
}


