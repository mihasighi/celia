
#include "intlist.h"
#include "sl3.h"

/** 
 * Concatenates two sorted lists with in order data.
 */

/*@ requires (\exists Node n1, Node n2 ; ls(n1,nilNode) && ls(n2,nilNode) && len(n1)>=1 && len(n2)>=1 &&
                  label(x,n1) && label(z,n2) && \separated(sloc(n1),sloc(n2)) &&
                  (\forall int y ; Gall(y,n1) ==> ((data(n1,y) <= data(n2,(int)0)) && (data(n1,y) >= data(n1,(int)0)))) &&
                  (\forall int y1, int y2; Gle2(y1,y2,n1) ==> (data(n1,y1)<=data(n1,y2))) &&
                  (\forall int y; Gfst(y,n2) ==> (data(n2,(int)0)+1==data(n2,y))) &&
                  (\forall int y1, int y2; Gsucc2(y1,y2,n2) ==> (data(n2,y1)+1==data(n2,y2)))
             );
  @ ensures  (\exists Node n1, Node n2 ; ls(n1,n2) && ls(n2,nilNode) && len(n1)>=1 && len(n2)>=1 &&
                  label(\result,n1) && label(x,n1) && label(z,n2) && \separated(sloc(n1),sloc(n2)) &&
                  (\forall int y ; Gall(y,n1) ==> ((data(n1,y) >= data(n1,(int)0)) && (data(n1,y) <= data(n2,(int)0)))) &&
                  (\forall int y1, int y2; Gle2(y1,y2,n1) ==> (data(n1,y1)<=data(n1,y2))) &&
                  (\forall int y ; Gall(y,n2) ==> (data(n2,y) >= data(n2,(int)0))) &&
                  (\forall int y1, int y2; Gle2(y1,y2,n1) ==> (data(n1,y1)<=data(n1,y2)))
	     )
	  ;
 */
intlist
sorted_concat(intlist x, intlist z) {
    intlist lst, tmp;
    lst = tmp = NULL;
    lst = x;
    tmp = lst->next;
    /*@ loop invariant
             (\exists Node n1, Node n2 ; ls(n1,nilNode) && ls(n2,nilNode) && len(n1)==1 && len(n2)>=1 &&
                    label(x,n1) && label(lst,n1) && label(tmp,nilNode) && label(z,n2) &&
                    \separated(sloc(n1),sloc(n2)) &&
                    data(n1,(int)0) <= data(n2,(int)0) &&
                    (\forall int y; Gfst(y,n2) ==> ((data(n2,(int)0)+1)==data(n2,y))) &&
                    (\forall int y1, int y2; Gsucc2(y1,y2,n2) ==> ((data(n2,y1)+1)==data(n2,y1)))
             )
          || (\exists Node n1, Node n1p, Node n2 ; ls(n1,n1p) && ls(n1p,nilNode) && ls(n2,nilNode) && 
                    len(n1)==1 && len(n1p)>=1 && len(n2)>=1 &&
                    label(x,n1) && label(lst,n1) && label(tmp,n1p) && label(z,n2) &&
                    \separated(sloc(n1),sloc(n1p),sloc(n2)) &&
                    data(n1,(int)0)<=data(n1p,(int)0) && data(n1p,(int)0) <= data(n2,(int)0) &&
                    (\forall int y ; Gall(y,n1p) ==> ((data(n1p,y) >= data(n1p,(int)0)))) && 
                    (\forall int y1, int y2; Gle2(y1,y2,n1p) ==> (data(n1p,y1)<=data(n1p,y2))) &&
                    (\forall int y ; Glst(y,n1p) ==> (data(n1p,y)<=data(n2,(int)0))) &&
                    (\forall int y ; Gfst(y,n2) ==> (data(n2,(int)0)+1==data(n2,y))) &&
                    (\forall int y1, int y2 ; Gsucc2(y1,y2,n2) ==> (data(n2,y1)+1==data(n2,y1)))
             )
          || (\exists Node n1, Node n1p, Node n1pp, Node n2 ; ls(n1,n1p) && ls(n1p,n1pp) && ls(n1pp,nilNode) && ls(n2,nilNode) &&
                    len(n1)>=1 && len(n1p)==1 && len(n1pp)>=1 && len(n2)>=1 &&
                    \separated(sloc(n1),sloc(n1p),sloc(n1pp),sloc(n2)) &&
                    label(x,n1) && label(lst,n1p) && label(tmp,n1pp) && label(z,n2) &&
                    data(n1,(int)0) <= data(n1p,(int)0) && data(n1p,(int)0)<=data(n1pp,(int)0) &&
                    (\forall int y ; Gall(y,n1) ==> ((data(n1,y) >= data(n1,(int)0)))) && 
                    (\forall int y1, int y2; Gle2(y1,y2,n1) ==> (data(n1,y1)<=data(n1,y2))) &&
                    (\forall int y ; Glst(y,n1) ==> (data(n1,y)<=data(n1p,(int)0))) &&
                    (\forall int y ; Gall(y,n1pp) ==> ((data(n1pp,y) >= data(n1pp,(int)0)))) && 
                    (\forall int y1, int y2; Gle2(y1,y2,n1pp) ==> (data(n1pp,y1)<=data(n1pp,y2))) &&
                    (\forall int y ; Glst(y,n1pp) ==> (data(n1,y)<=data(n2,(int)0))) &&
                    (\forall int y ; Gfst(y,n2) ==> (data(n2,(int)0)+1==data(n2,y))) &&
                    (\forall int y1, int y2 ; Gsucc2(y1,y2,n2) ==> (data(n2,y1)+1==data(n2,y1)))
             )
          || (\exists Node n1, Node n1p, Node n2 ; ls(n1,n1p) && ls(n1p,nilNode) && ls(n2,nilNode) &&
                    len(n1)>=1 && len(n1p)==1 && len(n2)>=1 &&
                    \separated(sloc(n1),sloc(n1p),sloc(n2)) &&
                    label(x,n1) && label(lst,n1p) && label(tmp,nilNode) && label(z,n2) &&
                    data(n1,(int)0) <= data(n1p,(int)0) && data(n1p,(int)0)<=data(n2,(int)0) &&
                    (\forall int y ; Gall(y,n1) ==> ((data(n1,y) >= data(n1,(int)0)))) && 
                    (\forall int y1, int y2; Gle2(y1,y2,n1) ==> (data(n1,y1)<=data(n1,y2))) &&
                    (\forall int y ; Glst(y,n1) ==> (data(n1,y)<=data(n1p,(int)0))) &&
                    (\forall int y ; Gfst(y,n2) ==> (data(n2,(int)0)+1==data(n2,y))) &&
                    (\forall int y1, int y2 ; Gsucc2(y1,y2,n2) ==> (data(n2,y1)+1==data(n2,y1)))
             )
           ;
      */
    while (tmp != NULL) {
      lst = NULL;
      lst = tmp;
      tmp = NULL;
      tmp = lst->next;
    }
    lst->next = NULL;
    lst->next = z;
    lst = NULL;
    return x;
}
