
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

//- length function
/*@ logic integer len(intlist ptr) =
  @    (ptr == \null) ? 0 : 1 + len(ptr->next)
  @ ;
 */


// Quick sort: version creating new list



/*@ requires acyclic (a) ;
 */
intlist quicksort(intlist a) {
    intlist *left, *right;
    intlist pivot, res, start, tmp;
    int d, l;

    intlist xis, ys, tmps;


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
            pivot->data = d;
    	    xis = ys = tmps = NULL;
            xis = start;
	    *left = NULL;
	    *right = NULL;
	    while (xis != NULL) {
	        ys = (intlist) malloc(sizeof (struct intlist_));
	        ys->data = xis->data;
	        ys->next = NULL;
        	tmps = xis->next;
	        xis = NULL;
        	xis = tmps;
	        tmps = NULL;
        	if (ys->data <= d) {
	            ys->next = *left;
        	    *left = NULL;
	            *left = ys;
        	} else {
	            ys->next = *right;
	            *right = NULL;
        	    *right = ys;
        	}
	        ys = NULL;
    	     }
            start = NULL; /* no more used */
            l = 1 ;     
	    tmp = quicksort(*left);
            *left = NULL;
            *left = tmp;
            tmp = NULL;
            tmp = quicksort(*right);
            *right = NULL;
            *right = tmp;
            tmp = NULL;
            res = fold_concat3(*left, pivot, *right);
            *left = *right = pivot = NULL;
        }
    }
    return res;
}


/*@ requires acyclic(x) && acyclic(y)  && acyclic(h) && disjoint(x,y) && disjoint(y,h) && disjoint(x,h);
 */
intlist fold_concat3(intlist x, intlist y, intlist h) {
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

// Copy h after z
    li = h;
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


/*@ requires len ==1 ; 
 */
void amain(int len) 
{

    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = NULL;
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = len - i;
        curr->next = NULL;
        curr->next = head;
        head = NULL;
        head = curr;
    }
    curr = NULL;

    curr = quicksort(head);

    head = NULL;

}
