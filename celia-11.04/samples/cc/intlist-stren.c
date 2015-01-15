
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

/*@ requires acyclic(x);
 */
void
insertsort(intlist x)
{

  intlist xi;
  intlist y;
  intlist yi;
  int m;
  int n;

  xi = NULL;
  y = NULL;
  yi = NULL;
  xi = x->next;
  while (xi != NULL)
    {
      y = x;
      while (y != xi) {
        if (y->data <= xi->data)
        {
          yi = y->next;
          y = NULL;
          y = yi;
          yi = NULL;
        }
        else break;
      }
      m = xi->data;
      while (y != xi)
        {
          n = y->data;
          y->data = m;
          m = n;
          yi = y->next;
          y = NULL;
          y = yi;
          yi = NULL;
        }
      y = NULL;
      xi->data = m;
      yi = xi->next;
      xi = NULL;
      xi = yi;
      yi = NULL;
    }
}


/*@ requires len >= 1 ;
  @ requires l >= 1;
 */
void
amain (int len, int l)
{

 intlist curr, head, hd;
    int i;

    head = NULL;
    curr = NULL;
  
  int m1 = l+1;
  int m2 = l;

    for (i = 1; i <= len; i++) {
        curr = NULL;
	curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = l + i;
        curr->next = head;
        m1 = m2;
        m2 = curr->data;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    
    hd = NULL;
    insertsort(head);

    m1 = m2;  
}



