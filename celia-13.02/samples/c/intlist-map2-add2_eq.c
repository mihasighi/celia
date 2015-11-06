
#include "intlist.h"

// Initializes each element of y with the corresponding element of x
// plus 2.
// Iterative version.

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_add2_eq(intlist x, intlist y) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL) {
        yi->data = xi->data + 2;
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


