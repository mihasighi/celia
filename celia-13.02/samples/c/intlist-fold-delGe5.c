
#include "intlist.h"

// Instantiate fold_delGeV with constant value to obtain 
// relations between sum and length
/*@ requires acyclic(x);
 */
intlist fold_delGe5(intlist x) {
    intlist r;
    int v;
    r = NULL;
    v = 5;
    r = fold_delGeV(x, v);
    return r;
}


