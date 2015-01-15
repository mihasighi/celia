
#include "intlist.h"

// Initializes with the first natural numbers.
// Iterative version.

/*@ requires acyclic(x2);
  @ ensures  \exists Node n. ls(n,#) && x2(n) && \length(n) >= 1 &&
                \data(n) = 0 &&
		\forall int y. 1<= y < \length(n) ==> n[y] = y
	;
 */
void map_initSeq(intlist x2) {
    int x0 = 0;
    intlist x1, x3;
    x1 = x3 = NULL;
    x1 = x2;
    /*@ loop invariant
   	 \exists Node n. ls(n,#) && x2(n) && x1(n) &&
		\length(n)>=1 && x0 == 0
      || \exists Node n1, Node n2. ls(n2,n1) * ls(n1,#) && 
		x2(n2) && x1(n1) && x3(#) && \length(n1) >= 1 && x0 >= 1 &&
		\length(n2) == x0 && \data(n2) == 0 &&
                \forall int y. 1<= y < \length(n2) ==> n2[y] == y
      || \exists Node n. ls(n,#) && x2(n) && x1(#) && x0 >= 1 &&
                \length(n) == x0 && \data(n) == 0 &&
                \forall int y. 1<= y < \length(n) ==> n[y] == y
	;
     */
    while (x1 != NULL) {
        x1->data = x0;
        x0 = x0 + 1;
        x3 = x1->next;
        x1 = NULL;
        x1 = x3;
        x3 = NULL;
    }
}

