
#include "intlist.h"

// Merge two sorted lists.
// Iterative version

/*@ requires acyclic(x) && acyclic(y) && sorted(x) && sorted(y) && disjoint(x,y);
  @ ensures  acyclic(\result) && sorted(\result);
 */
intlist
sorted_merge(intlist x, intlist y) {
    intlist r, ri, xi, yi, tmp;
    int v;
    r = ri = xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    /*@ loop invariant
     *       \exists Node n1, Node n2 ; ls(n1,#) * ls(n2,#)
     *              && x(n1) && xi(n1) && y(n2) && yi(n2)
     *              && r(#) && ri(#) && tmp(#)
     *              && sorted(n1) && sorted(n2)
     *    || \exists Node n, Node n1, Node n1p, Node n2, Node n2p ;
     *              ls(n1,n1p) * ls(n1p,#) * ls(n2,n2p) * ls(n2p,#) * ls(n,#)
     *              && x(n1) && xi(n1p) && y(n2) && yi(n2p) && tmp(#) && r(n) & ri(#)
     *              && \length(n) == 1
     *              && sorted(n1) && sorted(n1p) && sorted(n2) && sorted(n2p)
     *              && \data(n) <= \data(n2p) && \data(n) <= \data(n1p)
     *              && sorted(n2)
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
