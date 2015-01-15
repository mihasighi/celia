
#include "intlist.h"

// Initializes x2 with the sequence of modulo 2.
// Iterative version.

/*@ requires acyclic(x2);
  @ ensures  \exists Node n. ls(n,#) && x2(n) &&
                 \data(n) = 0 &&
                 \forall int y1, int y2. (1<=y1<\length(n) &&
                     1<= y2 < \length(n) && y2=y1+1)
                     ==> (0<=n[y1]<=1 && 0<=n[y2]<= 1 && n[y1]+n[y2]==1)
 */
void map_initMod2(intlist x2) {
    intlist x1, x3;
    int x0 = 0;
    x3 = x1 = NULL;
    x3 = x2;
    /*@ loop invariant
            \exists Node n. ls(n,#) && x2(n) && x3(n) && x1(#) && x0 = 0
        ||  \exists Node n1, Node n2. ls(n2,n1) * ls(n1,#) && x2(n2) && x3(n1) && x1(#) &&
                \data(n2) = 0 && 0 <= k <= 1 &&
                \forall int y1, int y2. (1<=y1<\length(n2) &&
                     1<= y2 < \length(n2) && y2=y1+1)
                     ==> (0<=n2[y1]<=1 && 0<=n2[y2]<= 1 && n2[y1]+n2[y2]==1) &&
                \forall y. (1<= y < \length(n2) && y == \length(n2)-1) 
                     ==> n2(y)==1-k
        ||  \exists Node n. ls(n,#) && x2(n) && x3(#1) && x1(#) &&
                \data(n) = 0 && 0 <= k <= 1 &&
                \forall int y1, int y2. (1<=y1<\length(n) &&
                     1<= y2 < \length(n) && y2=y1+1)
                     ==> (0<=n[y1]<=1 && 0<=n[y2]<= 1 && n[y1]+n[y2]==1) &&
                \forall y. (1<= y < \length(n2) && y == \length(n2)-1)
                     ==> n(y)==1-k
     */
    while (x3 != NULL) {
        x3->data = x0;
        x1 = x3->next;
        x3 = NULL;
        x3 = x1;
        x1 = NULL;
        x0 = 1 - x0;
    }
}


