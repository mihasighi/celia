
#include "linked_list.h"

// Quick sort: version creating new list

#define clone fold_clone
#define concat fold_concat
#define split fold_split

/*@ requires len >= 1;
 */
intlist init(int len) {
    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = i;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}


/*@ requires acyclic(a);
 */
intlist quicksort2(intlist a) {
    intlist *left, *right;
    intlist pivot, res, start, tmp;
    int d, l;
    if (a == NULL)
        res = NULL;
    else {
        start = a->next;
        if (start == NULL)
            res = fold_clone(a);
        else {
            d = a->data;
            l = 1;
            pivot = init(l);
            pivot->data = d;
            fold_split(start, d, left, right);
            start = NULL; /* no more used */
            l = 1 ;     
	    tmp = quicksort2(*left);
            *left = NULL;
            *left = tmp;
            tmp = NULL;
            tmp = quicksort2(*right);
            *right = NULL;
            *right = tmp;
            tmp = NULL;
            tmp = fold_concat(*left, pivot);
            res = fold_concat(tmp, *right);
            tmp = *left = *right = pivot = NULL;
        }
    }
    return res;
}


/*@ requires acyclic(x) && acyclic(y) && disjoint(x,y);
 */
intlist fold_concat(intlist x, intlist y) {
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
intlist fold_split_l(intlist x, int v) {
    intlist xi, y, tmp;
    xi = y = tmp = NULL;
    xi = x;
    intlist l = NULL;
    while (xi != NULL) {
        y = (intlist) malloc(sizeof (struct intlist_));
        y->data = xi->data;
        y->next = NULL;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        if (y->data <= v) {
            y->next = l;
            l = NULL;
            l = y;
        }
        y = NULL;
    }
    return l;
}

/*@ requires acyclic(x);
 */
intlist fold_split_r(intlist x, int v) {
    intlist xi, y, tmp;
    xi = y = tmp = NULL;
    xi = x;
    intlist l = NULL;
    while (xi != NULL) {
        y = (intlist) malloc(sizeof (struct intlist_));
        y->data = xi->data;
        y->next = NULL;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        if (y->data > v) {
            y->next = l;
            l = NULL;
            l = y;
        }
        y = NULL;
    }
    return l;
}



/*@ requires acyclic(a);
 */
intlist quicksort(intlist a) {
    intlist left, right;
    intlist pivot, res, start, tmp;
    int d, l;
    if (a == NULL)
        res = fold_clone(a);
    else {
        start = a->next;
        if (start == NULL)
            res = fold_clone(a);
        else {
            d = a->data;
            l = 1;
            pivot = init(l);
            pivot->data = d;
            left = fold_split_l(start,d);
            right = fold_split_r(start,d);
	    start = NULL; /* no more used */
            tmp = quicksort(left);
            left = NULL;
            left = tmp;
            tmp = NULL;
            tmp = quicksort(right);
            right = NULL;
            right = tmp;
            tmp = NULL;
            tmp = fold_concat(left, pivot);
            res = fold_concat(tmp, right);
            tmp = left = right = pivot = NULL;
        }
    }
    return res;
}



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
    yi = NULL;
    return y;
}//end clone function


