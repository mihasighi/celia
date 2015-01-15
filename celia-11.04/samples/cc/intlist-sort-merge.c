
#include "linked_list.h"

// Merge sort: version creating new list

#define clone fold_clone
#define merge fold2_merge
#define split split

/*@ requires acyclic(x);
 */
intlist mergesort(intlist x) {
    intlist r, tmp;
    r = tmp = NULL;
    if (x != NULL) {
      tmp = x->next;
      if (tmp == NULL)
        r = fold_clone(x);
      else {
        intlist *l1, *l2;
        tmp = NULL;
        *l1 = NULL;
        *l2 = NULL;
        split (x, l1, l2);
        tmp = mergesort (*l1);
        *l1 = NULL; *l1 = tmp; tmp = NULL;
        tmp = mergesort (*l2);
        *l2 = NULL; *l2 = tmp; tmp = NULL;
        tmp = fold2_merge (*l1, *l2);
        *l1 = NULL; *l2 = NULL;
        r = tmp; tmp = NULL;
      }
    }
    return r;
} // end merge_sort function


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
}// end merge function

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

/*@ requires acyclic(x);
 */
void split(intlist x, intlist* l1, intlist* l2) {

    intlist tmp, nxt, ll1, ll2, ln1, ln2;

    if (x == NULL) {
        *l1 = NULL;
        *l2 = NULL;
        return;
    }

    nxt = x->next;
    ll1 = (intlist) malloc(sizeof (struct intlist_));
    ll1->data = x->data;
    ll1->next = NULL;

    if (nxt == NULL) {
        *l1 = ll1;
        *l2 = NULL; 
        nxt = ll1 = NULL;
        return;
    }

    ll2 = (intlist) malloc(sizeof (struct intlist_));
    ll2->data = nxt->data;
    ll2->next = NULL;
    tmp = nxt->next;

    split(tmp, l1, l2);

    ll1->next = *l1;
    ll2->next = *l2;
    *l1 = NULL; *l1 = ll1;
    *l2 = NULL; *l2 = ll2;
    
    ll1 = ll2 = tmp = nxt = NULL; /* no more used */

} // end split function

