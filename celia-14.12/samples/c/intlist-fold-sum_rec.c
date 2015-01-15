
#include "intlist.h"

// Computes the sum of values in a list
// Recursive version.

/*@ requires acyclic(x);
 */
int fold_rec_sum(intlist x) {
    intlist tmp;
    int s;
    s = 0;
    if (x != NULL) {
        tmp = NULL;
        tmp = x->next;
        s = fold_rec_sum(tmp);
        s = s + x->data;
    }
    return s;
}

