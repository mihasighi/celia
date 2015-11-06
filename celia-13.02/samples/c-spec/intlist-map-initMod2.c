
#include "intlist.h"
#include "sl3.h"

/** 
 * Initializes @code{x2} with the sequence of modulo 2.
 */

/*@ requires (\exists Node n; ls(n,nilNode) && label(x2,n) && len(n)>=1);
  @ ensures  (\exists Node n; ls(n,nilNode) && label(x2,n) && len(n)>=1 &&
                 (data(n,(int)0) == (int)0) &&
                 (\forall int y1, int y2; Gsucc2(y1,y2,n)
                     ==> (0<=data(n,y1) && data(n,y1)<=1 && 
			  0<=data(n,y2) && data(n,y2)<=1 && 
			  (data(n,y1)+data(n,y2))==1))
		);
 */
void map_initMod2(intlist x2) {
    intlist x1, x3;
    int x0 = 0;
    x3 = x1 = NULL;
    x3 = x2;
    /*@ loop invariant
            (\exists Node n; ls(n,nilNode) && len(n)>=1 &&
			     label(x2,n) && label(x3,n) && label(x1,nilNode) && 
			     x0 == 0)
        ||  (\exists Node n1, Node n2; ls(n2,n1) && ls(n1,nilNode) && len(n2)>=1 && len(n1)>=1 &&
		label(x2,n2) && label(x3,n1) && label(x1,nilNode) &&
                (data(n2,(int)0) == (int)0) && (0 <= x0 && x0 <= 1) &&
                (\forall int y1, int y2; Gsucc2(y1,y2,n2) 
                     ==> (0<=data(n2,y1) && data(n2,y1)<=1 && 0<=data(n2,y2) && data(n2,y2)<= 1 && 
			  data(n2,y1)+data(n2,y2)==1)) &&
                (\forall int y; Glst(y,n2) ==> data(n2,y)==1-x0)
	    )
        ||  (\exists Node n; ls(n,nilNode) && len(n)>=1 &&
	        label(x2,n) && label(x3,nilNode) && label(x1,nilNode) &&
                data(n,(int)0) == (int)0 && 0 <= x0 && x0 <= 1 &&
                (\forall int y1, int y2; Gsucc2(y1,y2,n) 
                     ==> (0<=data(n,y1) && data(n,y1)<=1 && 0<=data(n,y2) && data(n,y2)<= 1 && 
			  data(n,y1)+data(n,y2)==1)) &&
                (\forall int y; Glst(y,n) ==> data(n,y)==1-x0)
	    )
	;
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


