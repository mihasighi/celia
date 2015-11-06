
#include "intlist.h"

// Copies all elements of x in y.
// Iterative version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
void map2_copy(intlist x, intlist y) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL && yi != NULL) {
        yi->data = xi->data; 
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
}

