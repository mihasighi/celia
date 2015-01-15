
#include "intlist.h"

// Initializes all elements of x2 with value x0.
// Iterative version.

/*@ requires acyclic(x2);
  @ ensures  \exists Node n. x2(n) && ls(n,#) && \length(n) >= 1 &&
		\data(n) == x0 &&
		\forall int y . 1 <= y < \length(n) ==> n[y] == x0
	;
 */
void map_initV(intlist x2, int x0) {
    intlist x1, x3;
    x1 = x3 = NULL;
    x1 = x2;
    /*@ loop invariant
		\exists Node n. ls(n,#) && x2(n) && x1(n) && x3(#) && \length(n) >= 1
        || 	\exists Node n1, Node n2. ls(n2,n1) * ls(n1,#) && x2(n2) && x1(n1) &&
			\length(n2) >= 1 && \length(n1) >= 1 &&
			\data(n2) == x0 &&
			\forall int y. 1 <= y < \length(n2) ==> n2[y] == x0
        ||	\exists Node n. ls(n,#) && x2(n) && x1(#) && x3(#) && \length(n) >= 1
			\data(n) == x0 &&
			\forall int y. 1 <= y < \length(n) ==> n[y] == x0
	;
     */
    while (x1 != NULL) {
        x1->data = x0;
        x3 = x1->next;
        x1 = NULL;
        x1 = x3;
        x3 = NULL;
    }
}

