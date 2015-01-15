
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

/*@ requires len >= 1 && val >=0 ;
 */
void
amain (int len, int val)
{

 intlist curr, head, hd;
    int i;

    head = NULL;
    curr = NULL;
  
  int m1 = val+1;
  int m2 = val;

    for (i = 1; i <= 2; i++) {
        curr = NULL;
	curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = m1 + m2;
        curr->next = head;
        m1 = m2;
        m2 = curr->data;
        head = NULL;
        head = curr;
        //curr = NULL;
    }
hd = NULL;
if(curr!=NULL){
    hd = (intlist) malloc(sizeof (struct intlist_));
    hd->data = m2 + m2;
    m1 = hd->data;
    m2 = hd->data;  
  
   curr = NULL;


 for (i = 1; i <= 1; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = m1 + m2;
        curr->next = hd;
        m1 = m2;
        m2 = curr->data;
        hd = NULL;
        hd = curr;
        curr = NULL;
    }

}

//  head = init(len);  //list of 10 to 1 
//  hd = init1(len);
  intlist tmp = NULL;
  tmp = fold_concat(hd,head);
  
}


