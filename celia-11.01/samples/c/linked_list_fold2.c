
#include "linked_list.h"


/* =================================================================== */
/* FOLD2 class: Iterates over two list to build a result */
/* =================================================================== */

// Returns 1 if two lists are equal

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
int fold2_equal(intlist x, intlist y) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL && yi != NULL) {
        if (xi->data != yi->data)
            return 0;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
    if (xi == NULL && yi == NULL)
        return 1;
    return 0;
}

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
int fold2_rec_equal(intlist x, intlist y) {
    if (x != NULL && y != NULL) {
        if (x->data != y->data)
            return 0;
        intlist xi, yi;
        int r;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        r = fold2_rec_equal(xi, yi);
        return r;
    } else if (x == NULL && y == NULL)
        return 1;
    return 0;
}

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
    }
    return r;
}

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_rec_concat(intlist x, intlist y) {
    intlist r;
    r = NULL;
    if (x == NULL) {
        r = fold_clone(y);
    } else {
        intlist tmp;
        tmp = NULL;
        tmp = x->next;
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = x->data;
        r->next = NULL;
        r->next = fold2_rec_concat(tmp, y);
    }
    return r;
}

// Merge two lists in the result.
// If the two lists are sorted, the result is sorted.

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
    return r;
}

/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold2_rec_merge(intlist x, intlist y) {
    intlist r, xi, yi;
    int v;
    r = xi = yi = NULL;
    if (x != NULL && y != NULL) {
        if (x->data < y->data) {
            v = x->data;
            xi = x->next;
            yi = y;
        } else {
            v = y->data;
            yi = yi->next;
            xi = x;
        }
        // add v in front of the recursive call result
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = v;
        r->next = NULL;
        r->next = fold2_rec_merge(xi,yi);
    }
    else if (x != NULL) {
        // add v in front of the recursive call result
        xi = x->next;
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = x->data;
        r->next = NULL;
        r->next = fold2_rec_merge(xi,y);
    }
    else if (y != NULL) { // add v at the end of r, pointed by ri
        yi = y->next;
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = y->data;
        r->next = NULL;
        r->next = fold2_rec_merge(x,yi);
    }
    return r;
}
