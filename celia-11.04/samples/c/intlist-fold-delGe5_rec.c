
#include "intlist.h"

/*@ requires acyclic(x);
 *  */
intlist fold_rec_delGe5(intlist x) {
    intlist r;
    int v;
    r = NULL;
    v = 5;
    r = fold_rec_delGeV(x, v);
    return r;
}


