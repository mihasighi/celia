
#include "intlist.h"

// Initializes each element of y with the corresponding element of x
// plus v.
// Does not test the length of the list.
// Iterative version.

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_addV_err(intlist x, intlist y, int v) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL /* && yi != NULL */ ) {
        yi->data = xi->data + v;
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


