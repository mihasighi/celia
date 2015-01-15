
#include "intlist.h"
#include "sl3.h"

/**
 * Initializes each element of @code{x4} with the corresponding element of 
 * @code{x2} plus @code{x0}.
 */

/*@ requires \exists Node n1, Node n2 ; ls(n1,nilNode) && ls(n2,nilNode) &&
  @             \separated(sloc(n1),sloc(n2)) && label(x2,n1) && label(x4,n2) &&
  @             len(n1) == len(n2) && len(n1) >= 1;
  @ ensures  \exists Node n1, Node n2 ; ls(n1,nilNode) && ls(n2,nilNode) && 
		\separated(sloc(n1),sloc(n2)) && label(x2,n1) && label(x4,n2) && 
		len(n1) == len(n2) && len(n1) >= 1 &&
		data(n1,(int)0)+x0 == data(n2,(int)0) &&
		(\forall int y1, int y2 ; Geq2(y1,n1,y2,n2) ==> data(n1,y1)+x0 == data(n2,y2))
	;
 */
void map2_addV_eq(intlist x2, intlist x4, int x0) {
    intlist x3, x5, x1;
    x3 = x5 = x1 = NULL;
    x3 = x2;
    x5 = x4;
    /*@ loop invariant
		(\exists Node n1, Node n2 ; 
			ls(n1,nilNode) && ls(n2,nilNode) && 
			\separated(sloc(n1),sloc(n2)) && 
			label(x2,n1) && label(x4,n2) && label(x3,n1) && label(x5,n2) && label(x1,nilNode) &&
			len(n1) == len(n2) && len(n1) >= 1)
	     || (\exists Node n1, Node n1i, Node n2, Node n2i ;
			ls(n1,n1i) && ls(n1i,nilNode) && ls(n2, n2i) && ls(n2i,nilNode) &&
			\separated(sloc(n1),sloc(n1i),sloc(n2),sloc(n2i)) && 
			label(x2,n1) && label(x3,n1i) && label(x4,n2) && label(x5,n2i) && label(x1,nilNode) &&
			len(n1) == len(n2) && len(n1) >= 1 &&
			len(n1i) == len(n2i) && len(n1i) >= 1 &&
		        data(n1,(int)0)+x0 == data(n2,(int)0) &&
			(\forall int y1, int y2 ; Geq2(y1,n1,y2,n2) ==> data(n1,y1)+x0 == data(n2,y2)))
	     || (\exists Node n1, Node n2 ;
			ls(n1,nilNode) && ls(n2,nilNode) &&
			\separated(sloc(n1),sloc(n2)) && 
			label(x2,n1) && label(x3,nilNode) && label(x4,n2) && label(x5,nilNode) && label(x1,nilNode) &&
			len(n1) == len(n2) && len(n1) >= 1 &&
		        data(n1,(int)0)+x0 == data(n2,(int)0) &&
			(\forall int y1, int y2 ; Geq2(y1,n1,y2,n2) ==> data(n1,y1)+x0 == data(n2,y2)))
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


