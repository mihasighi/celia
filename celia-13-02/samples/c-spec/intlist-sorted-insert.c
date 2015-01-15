
#include "intlist.h"
#include "sl3.h"

/** 
 * Inserts the integer @code{v} into a sorted list @code{x}.
 */

/*@ requires (\exists Node n ; ls(n,nilNode) && len(n)>=1 && label(x,n) && 
	          (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                  (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2)))
	     );
  @ ensures  (\exists Node n, Node n1 ; ls(n,n1) && ls(n1,nilNode) && len(n)>=1 && len(n1)>=1 &&
                  label(\result,n) && label(x,n) && \separated(sloc(n1),sloc(n)) &&
		  data(n,(int)0)<=v && data(n1,(int)0)==v &&
	          (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                  (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
	          (\forall int y ; Gall(y,n1) ==> (data(n1,(int)0) <= data(n1,y))) &&
                  (\forall int y1, int y2 ; Gle2(y1,y2,n1) ==> (data(n1,y1) <= data(n1,y2)))
	     ) 
          || (\exists Node n, Node n1 ; ls(n,n1) && ls(n1,nilNode) && len(n)==1 && len(n1)>=1 &&
                  label(\result,n) && label(x,n1) && \separated(sloc(n1),sloc(n)) &&
		  data(n,(int)0)==v && data(n1,(int)0)<=v &&
	          (\forall int y ; Gall(y,n1) ==> (data(n1,(int)0) <= data(n1,y))) &&
                  (\forall int y1, int y2 ; Gle2(y1,y2,n1) ==> (data(n1,y1) <= data(n1,y2)))
	     )
	  ;
 */
intlist
sorted_insert(intlist x, int v) {
    intlist curr, succ, item;
    curr = succ = item = NULL;
    curr = x;
    succ = x->next;
    /*@ loop invariant
             (\exists Node n ; ls(n,nilNode) && len(n)==1 &&
                    label(x,n) && label(curr,n) && label(succ,nilNode) && label(item,nilNode) 
             )
          || (\exists Node n, Node n2 ; ls(n,n2) && ls(n2,nilNode) && len(n)==1 && len(n2)>=1 &&
                    label(x,n) && label(curr,n) && label(succ,n2) && label(item,nilNode) &&
                    data(n,(int)0) <= data(n2,(int)0) &&
                    (\forall int y ; Gall(y,n2) ==> (data(n2,(int)0) <= data(n2,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n2) ==> (data(n2,y1) <= data(n2,y2)))
             )
          || (\exists Node n, Node n1 ; ls(n,n1) && ls(n1,nilNode) && len(n)>=1 && len(n1)==1 &&
                    label(x,n) && label(curr,n1) && label(succ,nilNode) && label(item,nilNode) &&
                    data(n,(int)0) <= data(n1,(int)0) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,y)<=data(n1,(int)0)) )
             )
          || (\exists Node n, Node n1, Node n2 ; ls(n,n1) && ls(n1,n2) && ls(n2,nilNode) && 
                    len(n)>=1 && len(n1)==1 && len(n2)>=1 &&
                    label(x,n) && label(curr,n1) && label(succ,n2) && label(item,nilNode) &&
                    data(n,(int)0) <= data(n1,(int)0) && data(n1,(int)0) <= data(n2,(int)0) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,(int)0) <= data(n,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n) ==> (data(n,y1) <= data(n,y2))) &&
                    (\forall int y ; Gall(y,n2) ==> (data(n2,(int)0) <= data(n2,y))) &&
                    (\forall int y1, int y2 ; Gle2(y1,y2,n2) ==> (data(n2,y1) <= data(n2,y2))) &&
                    (\forall int y ; Gall(y,n) ==> (data(n,y)<=data(n1,(int)0)))
             )
           ;
      */
    while (succ != NULL) {
        if (succ->data < v) {
            curr = NULL;
            curr = succ;
            succ = NULL;
            succ = curr->next;
        }
        else break;
    }
    item = (intlist) malloc(sizeof(struct intlist_));
    item->data = v;
    item->next = NULL;
    if (x->data > v) {
        item->next = x;
        x = NULL;
        x = item;
    }
    else {
        item->next = succ;
        curr->next = NULL;
        curr->next = item;
    }
    item = succ = curr = NULL; // no more used
    return x;
}
