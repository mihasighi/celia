
#include "intlist.h"
#include "sl3.h"

/**
 * Initializes @code{x3} with the Fibonacci sequence.
 */

/*@ requires (\exists Node n ; ls(n,nilNode) && label(x3,n) && len(n)>=1);
  @ ensures  (\exists Node n ; ls(n,nilNode) && label(x3,n) && len(n)>=1 &&
                 data(n,(int)0)==(int)2 &&
                 (\forall int y1, int y2 ; Gsucc2(y1,y2,n) ==> (data(n,y2) - data(n,y1)>=y1)
		 )
	     )
	     ;
 */
void map_initFibo(intlist x3) {
    int x0 = 2;
    int x1 = 3;
    intlist x4, x2;
    x4 = x2 = NULL;
    x4 = x3;
    /*@ loop invariant
           (\exists Node n ; ls(n,nilNode) && len(n)>=1 && 
	          label(x3,n) && label(x4,n) && label(x2,nilNode) && 
		  x0==2 && x1==3
	   )
        || (\exists Node n1, Node n2 ; ls(n2,n1) && ls(n1,nilNode) &&  len(n2)>=1 && len(n1)>=1 &&
	       \separated(sloc(n1), sloc(n2)) &&
	       label(x3,n2) && label(x4,n1) && label(x2,nilNode) &&
               data(n2,(int)0)==(int)2 && x0>=len(n2) && x1-x0>=len(n2) &&
               (\forall int y1, int y2 ; Gsucc2(y1,y2,n2) ==> (data(n2,y2)-data(n2,y1)>=y1)) &&
               (\forall int y ; Glst(y,n2) ==> ((x0-data(n2,y)>=len(n2)) && (data(n2,y)>=2)))
	    )
        || (\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
	       label(x3,n) && label(x4,nilNode) && label(x2,nilNode) &&
               data(n,(int)0)==(int)2 && x0>=len(n) && x1-x0>=len(n) &&
               (\forall int y1, int y2 ; Gsucc2(y1,y2,n) ==> (data(n,y2)-data(n,y1)>=y1)) &&
               (\forall int y ; Glst(y,n) ==>((x0-data(n,y)>=len(n)) && (data(n,y)>=2)))
	   )
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


