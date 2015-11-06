
#include "intlist.h"

// Finds an integer into a sorted list
// Iterative version

/*@ requires acyclic(x) && sorted(x);
 */
intlist
sorted_find(intlist x, int v) {
    intlist xi, tmp;
    xi = NULL;
    xi = x;
    while (xi != NULL) {
        if (xi->data < v) {
            tmp = xi->next;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
        }
        else break;
    }
    if (xi) {
        if (xi->data > v)
            xi = NULL;
    }
    return xi;
}
