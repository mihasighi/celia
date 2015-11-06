
#include "intlist.h"
#include "sl3.h"

/**
 * Finds the integer @code{v} into a sorted list @code{x}.
 */

/*@ requires (\exists Node n ; ls(n,nilNode) && len(n)>=1 && label(x,n) &&
		     (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
		     (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2)))
	     );
  @ ensures  (\exists Node n ; ls(n,nilNode) && len(n)>=1 && 
                     label(x,n) && label(\result,n) &&
		     data(n,(int)0)==v
	     )
       ||    (\exists Node n, Node n1 ; ls(n,n1) && ls(n1,nilNode) && len(n)>=1 && len(n1)>=1 &&
                     label(x,n) && label(\result,n1) && \separated(sloc(n),sloc(n1)) &&
		     data(n1,(int)0)==v
	     )
       ||    (\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
                     label(x,n) && label(\result,nilNode) &&
		     data(n,(int)0)>=v+1 && 
		     (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
		     (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2)))
             )
       ||    (\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
                     label(x,n) && label(\result,nilNode) &&
		     data(n,(int)0)<=v-1 && 
		     (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
		     (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
		     (\forall int y ; Glst(y,n) ==> data(n,y)<=v-1)
	     )
       ||    (\exists Node n, Node n1 ; ls(n,n1) && ls(n1,nilNode) && len(n)>=1 && len(n1)>=1 &&
                     label(x,n) && label(\result,nilNode) && \separated(sloc(n),sloc(n1)) &&
		     (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
		     (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
		     (\forall int y ; Gall(y,n1) ==> (data(n1,(int)0) <= data(n1,y))) &&
		     (\forall int y1, int y2 ; Gle2(y1,y2,n1) ==> (data(n1,y1) <= data(n1,y2))) &&
		     data(n,(int)0)<=v-1 && data(n1,(int)0)>=v+1 &&
		     (\forall int y; Gall(y,n) ==> ((data(n,y)<=v-1) && (data(n,y)>=data(n,(int)0)) && (data(n,y)<=data(n1,(int)0)) )) &&
		     (\forall int y; Gall(y,n1) ==> ((data(n1,y)>=v+1) && (data(n1,y)>=data(n1,(int)0)) ))
	     )
       ;
 */
intlist
sorted_find(intlist x, int v) {
    intlist xi, tmp;
    tmp = xi = NULL;
    tmp = xi = x;
    /*@ loop invariant
             (\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
                    label(x,n) && label(xi,n) && label(tmp,n) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) 
             )
          || (\exists Node n, Node n1 ; ls(n,n1) && ls(n1,nilNode) && len(n)>=1 && len(n1)>=1 &&
                    label(x,n) && label(xi,n1) && label(tmp,n1) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
                    (\forall int y ; Gall(y,n1) ==> (data(n1,(int)0) <= data(n1,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n1) ==> (data(n1,y1) <= data(n1,y2))) &&
                    data(n,(int)0)<=(v-1) && data(n,(int)0)<=data(n1,(int)0) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,y)<= v-1 && data(n,y) <= data(n1,(int)0)))
              )
          || (\exists Node n ; ls(n,nilNode) && len(n)>=1 &&
                    label(x,n) && label(xi,nilNode) && label(tmp,nilNode) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
                    (data(n,(int)0)<=v-1) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,y)<=v-1))
             )
           ;
     */
    while (xi != NULL) {
        if (xi->data < v) {
	    tmp = NULL; /* to simplify the invariants and the post-condition */
            tmp = xi->next;
            xi = NULL;
            xi = tmp;
        }
        else break;
    }
    if (xi) {
        if (xi->data > v)
            xi = NULL;
    }
    return xi;
}
