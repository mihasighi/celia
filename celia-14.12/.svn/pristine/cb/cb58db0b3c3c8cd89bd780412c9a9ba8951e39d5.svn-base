
#include "intlist.h"
#include "sl3.h"

/**
 * Initializes all elements of @code{x2} with value @code{x0}
 */

/*@ requires (\exists Node n ; ls(n,nilNode) && len(n)>=1 && label(x2,n));
  @ ensures  (\exists Node n ; ls(n,nilNode) && len(n)>=1 && label(x2,n) && 
                  data(n,(int)0) == x0 &&
		  (\forall int y ; Gall(y,n) ==> (data(n,y)==x0))
	     )
	     ;
 */
void map_initV(intlist x2, int x0) {
    intlist x1, x3;
    x1 = x3 = NULL;
    x1 = x2;
    /*@ loop invariant
		(\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
		        label(x2,n) && label(x1,n) && label(x3,nilNode)
		)
        || 	(\exists Node n1, Node n2 ; ls(n2,n1) && ls(n1,nilNode) && len(n1)>=1 && len(n2)>=1 &&
	                label(x2,n2) && label(x1,n1) &&
			data(n2,(int)0) == x0 &&
			(\forall int y ; Gall(y,n2) ==> (data(n2,y)==x0))
		)
        ||	(\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
	                label(x2,n) && label(x1,nilNode) && label(x3,nilNode) &&
			data(n,(int)0) == x0 &&
			(\forall int y ; Gall(y,n) ==> (data(n,y)==x0))
		)
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

