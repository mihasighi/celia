
#include "intlist.h"

// Computes the maximum value in a list
// Recursive version.

/*@ requires acyclic(x);
 */
int fold_rec_max(intlist x) {
    int m, m1;
    intlist tmp;
    m = 0;
    tmp = NULL;
    if (x != NULL) {
        tmp = x->next;
        m1 = fold_rec_max(tmp);
	tmp = NULL;
        if (m1 < x->data)
            m = x->data;
        else
            m = m1;
    }
    return m;
}


