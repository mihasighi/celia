
#include "intlist.h"

// Initializes each element of y with the corresponding element of x2
// plus x0.
// Iterative version.

/*@ requires acyclic(x2) && acyclic(x4) && \length(x2) == \length(x4);
  @ ensures  \exists Node n1, Node n2. ls(n1,#) * ls(n2,#) && 
		x2(n1) && x4(n2) && \length(n1) == \length(n2) && \length(n1) >= 1
		\forall int y1, int y2. 1 <= y1 < \length(n1) &&
			1 <= y2 < \length(n2) && y1 == y2 ==> n1[y1]+x0 == n2[y2]
	;
 */
void map2_addV_eq(intlist x2, intlist x4, int x0) {
    intlist x3, x5, x1;
    x3 = x5 = x1 = NULL;
    x3 = x2;
    x5 = x4;
    /*@ loop invariant
		\exists Node n1, Node n2. ls(n1,#) * ls(n2,#) && 
			x2(n1) && x4(n2) && x3(n1) && x5(n2) && x1(#) &&
			\length(n1) == \length(n2) && \length(n1) >= 1
	     || \exists Node n1, Node n1i, Node n2, Node n2i .
			ls(n1,n1i) * ls(n1i,#) * ls(n2, n2i) * ls(n2i,#) &&
			\length(n1) == \length(n2) && \length(n1) >= 1 &&
			\length(n1i) == \length(n2i) && \length(n1i) >= 1 &&
			x2(n1) && x3(n1i) && x4(n2) && x5(n2i) && x1(#) &&
			\forall int y1, int y2. 1 <= y1 < \length(n1) &&
				1 <= y2 < \length(n2) && y1 == y2 ==> n1[y1]+x0 == n2[y2]
	     || \exists Node n1, Node n2 .
			ls(n1,#) * ls(n2,#) &&
			\length(n1) == \length(n2) && \length(n1) >= 1 &&
                        x2(n1) && x4(n2) && x3(#) && x5(#) && x1(#) &&
			\forall int y1, int y2. 1 <= y1 < \length(n1) &&
				1 <= y2 < \length(n2) && y1 == y2 ==> n1[y1]+x0 == n2[y2]
 	;
     */
    while (x3 != NULL) {
        x5->data = x3->data + x0;
        x1 = x3->next;
        x3 = NULL;
        x3 = x1;
        x1 = NULL;
        x1 = x5->next;
        x5 = NULL;
        x5 = x1;
        x1 = NULL;
    }
}


