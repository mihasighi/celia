
#include "intlist.h"

// Merge two lists in the result.
// If the two lists are sorted, the result is sorted.
// Iterative version.

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_merge(intlist x, intlist y) {
    intlist r, ri, xi, yi, tmp;
    int v;
    r = ri = xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL && yi != NULL) {
        if (xi->data < yi->data) {
            v = xi->data;
            tmp = xi->next;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
        } else {
            v = yi->data;
            tmp = yi->next;
            yi = NULL;
            yi = tmp;
            tmp = NULL;
        }
        // add v at the end of r, pointed by ri
        tmp = (intlist) malloc(sizeof (struct intlist_));
        tmp->data = v;
        tmp->next = NULL;
        if (r == NULL) {
            r = tmp;
            ri = tmp;
        } else {
            ri->next = tmp;
            ri = NULL;
            ri = tmp;
        }
        tmp = NULL;
    }
    while (xi != NULL) {
        // add v at the end of r, pointed by ri
        tmp = (intlist) malloc(sizeof (struct intlist_));
        tmp->data = xi->data;
        tmp->next = NULL;
        if (r == NULL) {
            r = tmp;
            ri = tmp;
        } else {
            ri->next = tmp;
            ri = NULL;
            ri = tmp;
        }
        tmp = NULL;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
    while (yi != NULL) { // add v at the end of r, pointed by ri
        tmp = (intlist) malloc(sizeof (struct intlist_));
        tmp->data = yi->data;
        tmp->next = NULL;
        if (r == NULL) {
            r = tmp;
            ri = tmp;
        } else {
            ri->next = tmp;
            ri = NULL;
            ri = tmp;
        }
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
    ri = NULL; /* no more used */
    return r;
}

