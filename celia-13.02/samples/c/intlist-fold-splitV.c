
#include "intlist.h"

// Builds two lists l and u by dispatching x wrt v.
// Iterative version.

/*@ requires acyclic(x);
 */
void fold_split(intlist x, int v, intlist* l, intlist* u) {
    intlist xi, y, tmp;
    xi = y = tmp = NULL;
    xi = x;
    *l = NULL;
    *u = NULL;
    while (xi != NULL) {
        y = (intlist) malloc(sizeof (struct intlist_));
        y->data = xi->data;
        y->next = NULL;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        if (y->data <= v) {
            y->next = *l;
            *l = NULL;
            *l = y;
        } else {
            y->next = *u;
            *u = NULL;
            *u = y;
        }
        y = NULL;
    }
}


