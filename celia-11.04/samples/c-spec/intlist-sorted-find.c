
#include "intlist.h"

// Finds an integer into a sorted list
// Iterative version

/*@ requires acyclic(x) && \length(x)>=1 && sorted(x);
  @ ensures  v == \result->data || \result == NULL;
 */
intlist
sorted_find(intlist x, int v) {
    intlist xi, tmp;
    xi = NULL;
    xi = x;
    /*@ loop invariant
     *       \exists Node n ; ls(n,#) && x(n) && xi(n) && tmp(#)
     *              && \length(n) >= 1 && sorted(n)
     *    || \exists Node n, Node n1 ; ls(n,n1) * ls(n1,#)
     *              && x(n) && xi(n1) && tmp(#)
     *              && \length(n) >= 1 && \length(n1) >= 1
     *              && \data(n) <= \data(n1)
     *              && sorted(n) && sorted(n1)
     *              && \forall in y ; 1<= y < \length(n) ==> d(y)<= v-1 && d(y) <= d(n1)
     *    || \exists Node n ; ls(n,#)
     *              && x(n) && xi(#) && tmp(#)
     *              && \length(n) >= 1 && sorted(n)
     *              && \forall int y ; 1<= y < \length(n) ==> n[y]<=v-1
     *     ;
     */
    while (xi != NULL) {
        if (xi->data < v) {
            tmp = xi->next;
            xi = NULL;
            xi = tmp;
            tmp = NULL;
        }
        else break;
    }
    if (xi) {
        if (xi->data > v)
            xi = NULL;
    }
    return xi;
}
