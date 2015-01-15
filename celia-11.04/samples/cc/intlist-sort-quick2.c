
#include "stdlib.h"
#include "stdio.h"



typedef struct intlist_ *intlist;
struct intlist_
{
  int data;
  intlist next;
};

/* =================================================================== */
/* Predicates and functions used to specify intlist */
/* =================================================================== */

// - reachability predicate
/*@ predicate reach(intlist from, intlist to) =
  @   (from == to) || (\valid(from) && reach(from->next,to)) ;
 */

// - acyclic predicate
/*@ predicate acyclic(intlist l) = 
  @    reach(l,\null) ;
 */

// - disjoint predicate
/*@ predicate disjoint(intlist l1, intlist l2) =
  @   ! (\exists intlist l ; reach(l1,l) && reach(l2,l)) ;
  @ 
  @ predicate disjoint3(intlist l1, intlist l2, intlist l3) =
  @   disjoint(l1,l2) && disjoint(l2,l3) && disjoint(l1,l3) ;
  @ 
*/

// - length function
/*@ logic integer len(intlist ptr) =
  @    (ptr == \null) ? 0 : 1 + len(ptr->next)
  @ ;
 */


// Quick sort: version creating new list



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
        if (start == NULL){
            //res = fold_clone(a);a
            res = NULL;
	    res  = (intlist) malloc(sizeof (struct intlist_));
            res->data = a->data;
            res->next = NULL;

	}
        else {
            d = a->data;
            l = 1;
            pivot = NULL;
	    pivot = (intlist) malloc(sizeof (struct intlist_));
            pivot->next = NULL;
            //pivot = init(l);
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




