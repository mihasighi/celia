
#include "linked_list.h"
#include <limits.h>

/* =================================================================== */
/* FOLD class: Iterates over one list to build a result */
/* =================================================================== */

// Builds a new list by copying from x all elements greater than v.
// Iterative and recursive versions.

/*@ requires acyclic(x);
 */
intlist fold_copyGeV(intlist x, int v) {
    intlist z, y, xi;
    z = y = xi = NULL;
    xi = x;
    while (xi != NULL) {
        if (xi->data >= v) {
            z = (intlist) malloc(sizeof (struct intlist_));
            z->data = xi->data;
            z->next = NULL;
            z->next = y;
            y = NULL;
            y = z;
            z = NULL;
        }
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    return y;
}

/*@ requires acyclic(x);
 */
intlist fold_rec_copyGeV(intlist x, int v) {
    intlist y, z;
    y = z = NULL;
    if (x != NULL) {
        z = x->next;
        if (x->data >= v) {
            y = (intlist) malloc(sizeof (struct intlist_));
            y->data = x->data;
            y->next = NULL;
            y->next = fold_rec_copyGeV(z, v);
        } else
            y = fold_rec_copyGeV(z, v);
    }
    return y;
}

// Instantiate v with 5 to obtain relation between length and sum

/*@ requires acyclic(x);
 */
intlist fold_copyGe5(intlist x) {
    intlist xi, y, z;
    xi = y = z = NULL;
    xi = x;
    while (xi != NULL) {
        if (xi->data >= 5) {
            z = (intlist) malloc(sizeof (struct intlist_));
            z->data = xi->data;
            z->next = NULL;
            z->next = y;
            y = NULL;
            y = z;
            z = NULL;
        }
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    return y;
}

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

// Removes from x all elements greater than v.
// Iterative and recursive versions.

/*@ requires acyclic(x);
 */
intlist fold_delGeV(intlist x, int v) {
    intlist z, tmp, xip, xi;
    z = tmp = xip = xi = NULL;
    xi = x;
    while (xi != NULL) {
        if (xi->data >= v) {
            z = xi;
            tmp = xi->next;
            free(z);
            z = NULL;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
            if (xip == NULL) {
                x = NULL;
                x = xi;
            } else {
                xip->next = NULL;
                xip->next = xi;
            }
        } else {
            xip = NULL;
            xip = xi;
            xi = NULL;
            xi = xip->next;
        }
    }
    return x;
}

/*@ requires acyclic(x);
 */
intlist fold_rec_delGeV(intlist x, int v) {
    intlist r;
    r = NULL;
    if (x != NULL) {
        intlist tmp;
        tmp = NULL;
        tmp = x->next;
        if (x->data >= v) {
            free(x);
            r = fold_rec_delGeV(tmp, v);
        } else {
            r = x;
            r->next = NULL;
            r->next = fold_rec_delGeV(tmp, v);
        }
    }
    return r;
}

// Instantiate v with 5 to obtain relation between length and sum

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

/*@ requires acyclic(x);
 */
intlist fold_rec_delGe5(intlist x) {
    intlist r;
    int v;
    r = NULL;
    v = 5;
    r = fold_rec_delGeV(x, v);
    return r;
}

// Create a new list, the reverse of x.

/*@ requires acyclic(x);
 */
intlist fold_copyRev(intlist x) {
    intlist xi, y, z;
    xi = y = z = NULL;
    xi = x;
    while (xi != NULL) {
        z = (intlist) malloc(sizeof (struct intlist_));
        z->data = xi->data;
        z->next = NULL;
        z->next = y;
        y = NULL;
        y = z;
        z = NULL;
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    return y;
}

/*@ requires acyclic(x);
 */
intlist fold_rec_copyRev_aux(intlist x, intlist nxt) {
    intlist z, y, r;
    r = y = z = NULL;
    z = (intlist) malloc(sizeof (struct intlist_));
    z->data = x->data;
    z->next = NULL;
    z->next = nxt;
    y = x->next;
    r = fold_rec_copyRev_aux(y, x);
    return r;
}

intlist fold_rec_copyRev(intlist x) {
    intlist r, nxt;
    r = nxt = NULL;
    r = fold_rec_copyRev_aux(x, nxt);
    return r;
}

// Computes the maximum value in a list
// Iterative and recursive versions.

/*@ requires acyclic(x);
 */
int fold_max(intlist x) {
    int m;
    intlist xi, tmp;
    xi = tmp = NULL;
    xi = x;
    m = INT_MIN;
    while (xi != NULL) {
        if (xi->data > m)
            m = xi->data;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
    return m;
}

/*@ requires acyclic(x);
 */
int fold_rec_max(intlist x) {
    int m, m1;
    intlist tmp;
    m = INT_MIN;
    tmp = NULL;
    if (x != NULL) {
        tmp = x->next;
        m1 = fold_rec_max(tmp);
        if (m1 < x->data)
            m = x->data;
        else
            m = m1;
    }
    return m;
}

// Computes the sum of values in a list
// Iterative and recursive versions.

/*@ requires acyclic(x);
 */
int fold_sum(intlist x) {
    intlist xi, tmp;
    int sum = 0;
    xi = tmp = NULL;
    xi = x;
    while (xi != NULL) {
        sum = sum + xi->data;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
    return sum;
}

/*@ requires acyclic(x);
 */
int fold_rec_sum(intlist x) {
    intlist tmp;
    int s;
    s = 0;
    if (x != NULL) {
        tmp = NULL;
        tmp = x->next;
        s = fold_rec_sum(tmp);
        s = s + x->data;
    }
    return s;
}

// Clones x in the result.
// Iterative and recursive versions.

/*@ requires acyclic(x);
 */
intlist fold_clone(intlist x) {
    intlist xi, y, yi, z;
    xi = y = yi = z = NULL;
    xi = x;
    while (xi != NULL) {
        z = (intlist) malloc(sizeof (struct intlist_));
        z->data = xi->data;
        z->next = NULL;
        if (y == NULL)
            y = z;
        else
            yi->next = z;
        yi = NULL;
        yi = z;
        z = NULL;
        z = xi->next;
        xi = NULL;
        xi = z;
        z = NULL;
    }
    return y;
}

/*@ requires acyclic(x);
 */
intlist fold_rec_clone(intlist x) {
    intlist r, tmp;
    r = tmp = NULL;
    if (x != NULL) {
        r = (intlist) malloc(sizeof (struct intlist_));
        r->data = x->data;
        r->next = NULL;
        tmp = x->next;
        r->next = fold_rec_clone(tmp);
    }
    return r;
}

// Builds two lists l and u by dispatching x wrt v.
// Iterative and recursive versions.

/*@ requires acyclic(x);
 */
void fold_dispatch(intlist x, int v, intlist* xlev, intlist* xgtv) {
    intlist xi, y;
    xi = y = NULL;
    xi = x;
    *xgtv = NULL;
    *xlev = NULL;
    while (xi != NULL) {
        y = xi;
        xi = NULL;
        xi = y->next;
        y->next = NULL;
        if (y->data <= v) {
            y->next = *xlev;
            *xlev = NULL;
            *xlev = y;
        } else {
            y->next = *xgtv;
            *xgtv = NULL;
            *xgtv = y;
        }
        y = NULL;
    }
}

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


// Instantiate with v at 5 to obtain relations between sums and lengths

/*@ requires acyclic(x);
 */
void fold_split5(intlist x, intlist* l, intlist* u) {
    int v;
    v = 5;
    fold_split(x, v, l, u);
}

/*@ requires acyclic(x);
 */
void fold_rec_split5(intlist x, intlist* l, intlist* u) {
    int v;
    v = 5;
    fold_rec_split(x, v, l, u);
}
