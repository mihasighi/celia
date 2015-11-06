
#include "intlist.h"

// Builds a new list by copying from x all elements greater than v.
// Recursive version.

/*@ requires acyclic(x);
 */
intlist fold_rec_copyGe5(intlist x) {
    intlist z;
    int v;
    z = NULL;
    v = 5;
    z = fold_rec_copyGeV(x, v);
    return z;
}

