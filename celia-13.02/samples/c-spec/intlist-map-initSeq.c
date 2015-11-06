
#include "intlist.h"
#include "sl3.h"

/**
 * Initializes @code{x2} with the first natural numbers.
 */

/*@ requires (\exists Node n ; ls(n,nilNode) && len(n)>=1 && label(x2,n));
  @ ensures  (\exists Node n ; ls(n,nilNode) && len(n)>=1 && label(x2,n) &&
                data(n,(int)0)==(int)0 &&
		(\forall int y ; Gall(y,n) ==> data(n,y) == y)
	     )
	     ;
 */
void map_initSeq0(intlist x2) {
    int x0 = 0;
    intlist x1, x3;
    x1 = x3 = NULL;
    x1 = x2;
    /*@ loop invariant
   	 (\exists Node n ; ls(n,nilNode) && len(n)>=1 && 
	        label(x2,n) && label(x1,n) &&
		x0 == 0)
      || (\exists Node n1, Node n2 ; ls(n2,n1) && ls(n1,nilNode) && len(n1)>=1 && len(n2)>=1 && 
		\separated(sloc(n1),sloc(n2)) &&
		label(x2,n2) && label(x1,n1) && label(x3,nilNode) && 
		x0 >= 1 && len(n2)==x0 && 
		data(n2,(int)0)==(int)0 &&
		(\forall int y ; Gall(y,n2) ==> (data(n2,y)==y))
	 )
      || (\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
                label(x2,n) && label(x1,nilNode) &&
		x0 >= 1 && len(n)==x0 && 
		data(n,(int)0) ==(int)0 &&
                (\forall int y ; Gall(y,n) ==> (data(n,y) == y))
	 )
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

