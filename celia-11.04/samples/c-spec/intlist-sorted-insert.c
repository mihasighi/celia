
#include "intlist.h"

// Inserts an integer into a sorted list
// Iterative version

/*@ requires acyclic(x) && len(x)>=1 && sorted(x);
  @ ensures  acyclic(\result) && sorted(\result);
 */
intlist
sorted_insert(intlist x, int v) {
    intlist curr, succ, item;
    curr = succ = item = NULL;
    curr = x;
    succ = x->next;
    /*@ loop invariant
     *       \exists Node n, Node n2 ; ls(n,n2) * ls(n2,#)
     *              && x(n) && curr(n) && succ(n2) && item(#)
     *              && \length(n) == 1 && \length(n2) >= 1
     *              && \data(n) <= \data(n2)
     *              && sorted(n2)
     *    || \exists Node n ; ls(n,#) && x(n) && curr(n) && succ(#) && item(#)
     *              && \length(n) == 1
     *    || \exists Node n, Node n1 ; ls(n,n1) * ls(n1,#)
     *              && x(n) && curr(n1) && succ(#) && item(#)
     *              && \length(n1) == 1 && \data(n) <= \data(n1)
     *              && sorted(n) 
     *              && \forall int y ; 1<= y < \length(n) ==> n[y]<=\data(n1)
     *    || \exists Node n, Node n1, Node n2 ; ls(n,n1) * ls(n1,n2) * ls(n2,#)
     *              && x(n) && curr(n1) && succ(n2) && item(#)
     *              && \length(n1) == 1 && \length(n) >= 1 && \length(n2) >= 1
     *              && data(n) <= \data(n1) <= \data(n2)
     *              && sorted(n) && sorted(n2)
     *              && \forall int y ; 1<= y < \length(n) ==> n[y]<=\data(n1)
     *     ;
     */
    while (succ != NULL) {
        if (succ->data < v) {
            curr = NULL;
            curr = succ;
            succ = NULL;
            succ = curr->next;
        }
        else break;
    }
    item = (intlist) malloc(sizeof(struct intlist_));
    item->data = v;
    item->next = NULL;
    if (x->data > v) {
        item->next = x;
        x = NULL;
        x = item;
    }
    else {
        item->next = succ;
        curr->next = NULL;
        curr->next = item;
    }
    item = succ = curr = NULL; // no more used
    return x;
}
