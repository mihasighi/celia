
#include "intlist.h"

// Concatenates two lists in the result.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_concat(intlist x, intlist y) {
    intlist r, z, zi, tmp, li;
    r = z = zi = tmp = li = NULL;
    // Copy X
    li = x;
    while (li != NULL) {
        zi = (intlist) malloc(sizeof (struct intlist_));
        zi->data = li->data;
        zi->next = NULL;
        if (r == NULL) {
            r = zi;
            z = zi;
        } else {
            z->next = zi;
            z = NULL;
            z = zi;
        }
        zi = NULL;
        zi = li->next;
        li = NULL;
        li = zi;
        zi = NULL;
    }
    // Copy y after z
    li = y;
    while (li != NULL) {
        zi = (intlist) malloc(sizeof (struct intlist_));
        zi->data = li->data;
        zi->next = NULL;
        if (r == NULL) {
            r = zi;
            z = zi;
        } else {
            z->next = zi;
            z = NULL;
            z = zi;
        }
        zi = NULL;
        zi = li->next;
        li = NULL;
        li = zi;
        zi = NULL;
    }
    z = NULL;
    return r;
}
