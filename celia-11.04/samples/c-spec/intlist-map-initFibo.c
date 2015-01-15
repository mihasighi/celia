
#include "intlist.h"

// Initializes x3 with the Fibonacci sequence.

/*@ requires acyclic(x3);
  @ ensures  \exists Node n. ls(n,#) && x3(n) &&
                 \data(n)==0 &&
                 \forall int y1, int y2. (1<= y1 < \length(n) &&
                     1<= y1 < \length(n) && y1+1==y2)
                     ==> n[y2]-n[y1]>=y1-2
               ;
 */
void map_initFibo(intlist x3) {
    int x0 = 0;
    int x1 = 1;
    intlist x4, x2;
    x4 = x2 = NULL;
    x4 = x3;
    /*@ loop invariant
           \exists Node n. ls(n,#) && x3(n) && x4(n) && x2(#) && X0 == 0 && x1 == 1
        || \exists Node n1, Node n2. ls(n2,n1) * ls(n1,#) && x(n2) && xi(n1) && tmp(#)
               && \data(n2) == 0 && x1-x0>=\length(n2)-2
               && \forall int y1, int y2. (1<= y1 < \length(n2) &&
                      1 <= y2 < \length(n2) && y2==y1+1)
                      ==> n2[y2]-n2[y1]>=y1-2
               && \forall int y. (1<= y < \length(n2) && y = \length(n2)-1)
                      ==> x0-n2[y]>=\length(n2)-2 && n2[y]>=1
        || \exists Node n. ls(n,#) && x(n) && xi(#) && tmp(#)
               && \data(n) == 0 && x1-x0>=\length(n)-2
               && \forall int y1, int y2. (1<= y1 < \length(n) &&
                      1 <= y2 < \length(n) && y2==y1+1)
                      ==> n[y2]-n[y1]>=y1-2
               && \forall int y. (1<= y < \length(n) && y = \length(n)-1)
                      ==> x0-n[y]>=\length(n)-2 && n[y]>=1
         ;
     */
    while (x4 != NULL) {
        x4->data = x0;
        x0 = x1;
        x1 = x0 + x4->data;
        x2 = x4->next;
        x4 = NULL;
        x4 = x2;
        x2 = NULL;
    }
}


