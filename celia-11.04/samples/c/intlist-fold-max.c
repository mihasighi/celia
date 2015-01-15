
#include "intlist.h"

// Computes the maximum value in a list
// Iterative version.

/*@ requires acyclic(x);
 */
int fold_max(intlist x) {
    int m;
    intlist xi, tmp;
    xi = tmp = NULL;
    xi = x;
    m = 0;
    while (xi != NULL) {
        if (xi->data > m)
            m = xi->data;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
    return m;
}


