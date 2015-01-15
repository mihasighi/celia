
#include "intlist.h"

// Removes from x all elements greater than the first element of v
// and returns the removed elements
// Iterative version.

/*@ requires acyclic(x);
 */
intlist fold_delLtFst(intlist x) {
    intlist tmp, xip, xi, r;
    int v;
    tmp = xip = xi = r = NULL;
    if (x != NULL) {
        v = x->data;
        xip = x;
        xi = x->next;
        while (xi != NULL) {
            if (xi->data < v) { // put it in r
                tmp = xi->next;
                xi->next = NULL;
                xi->next = r;
                r = NULL;
                r = xi;
                xi = NULL;
                xi = tmp;
                tmp = NULL;
                xip->next = NULL;
                xip->next = xi;
            } else { // keep it in x
                xip = NULL;
                xip = xi;
                xi = NULL;
                xi = xip->next;
            }
        }
        xip = NULL; /* no more used */
    }
    return r;
}


