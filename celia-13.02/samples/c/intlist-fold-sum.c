
#include "intlist.h"

// Computes the sum of values in a list
// Iterative version.

/*@ requires acyclic(x);
 */
int fold_sum(intlist x) {
    intlist xi, tmp;
    int sum = 0;
    xi = tmp = NULL;
    xi = x;
    while (xi != NULL) {
        sum = sum + xi->data;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
    return sum;
}

