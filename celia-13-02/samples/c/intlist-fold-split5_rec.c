
#include "intlist.h"

// Builds two lists l and u by dispatching x wrt v.
// Instantiate with v at 5 to obtain relations between sums and lengths
// Recursive version.

/*@ requires acyclic(x);
 */
void fold_rec_split5(intlist x, intlist* l, intlist* u) {
    int v;
    v = 5;
    fold_rec_split(x, v, l, u);
}

