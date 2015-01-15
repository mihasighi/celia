
#include "intlist.h"

// Builds two lists l and u by dispatching x wrt v.
// Recursive version.

/*@ requires acyclic(x);
 */
void fold_rec_split(intlist x, int v, intlist* l, intlist* u) {
    intlist y, tmp;
    y = tmp = NULL;
    if (x != NULL) {
        tmp = x->next;
        fold_rec_split(tmp, v, l, u);
        y = (intlist) malloc(sizeof (struct intlist_));
        y->data = x->data;
        y->next = NULL;
        if (y->data <= v) {
            y->next = *l;
            *l = NULL;
            *l = y;
        } else {
            y->next = *u;
            *u = NULL;
            *u = y;
        }
    }
}


