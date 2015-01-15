
#include "intlist.h"

// Returns 1 if two lists are equal
// Iterative version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
int fold2_equal(intlist x, intlist y) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL) {
        if (yi != NULL) {
            if (xi->data != yi->data)
                return 0;
            tmp = xi->next;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
            tmp = yi->next;
            yi = NULL;
            yi = tmp;
            tmp = NULL;
        } else break;
    }
    if (xi == NULL)
        if (yi == NULL)
            return 1;
    return 0;
}


