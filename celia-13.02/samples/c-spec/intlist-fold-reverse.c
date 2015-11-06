
#include "intlist.h"
#include "sl3.h"

/**
 * Reverse in place the list @code{x2} and return the head of the new list
 * (which is the last element of @code{x2}).
 */

/*@ requires \exists Node n ; ls(n,nilNode) && label(x2,n) && len(n)>=1;
  @ ensures  (\exists Node n1, Node n ; ls(n1,n) &&  ls(n,nilNode) &&
                 \separated(sloc(n1),sloc(n)) &&
                 label(\result,n1) && label(x2,n) && 
                 len(n1)>=1 && len(n)==1)
          || (\exists Node n ; ls(n,nilNode) &&
                 label(\result,n) && label(x2,n) && len(n)==1)
             ;
 */
intlist
fold_reverse(intlist x2) {
  intlist x0, x3, x1;
  x0 = x3 = x1 = NULL;
  x3 = x2;
  /*@ loop invariant
         (\exists Node n; ls(n,nilNode) 
			 && label(x2,n) && label(x3,n) && label(x0,nilNode) && label(x1,nilNode)
                         && len(n)>=1)
      || (\exists Node n; ls(n,nilNode) 
			 && label(x2,n) && label(x0,n) && label(x3,nilNode) && label(x1,nilNode)
                         && len(n)==1)
      || (\exists Node n, n2; ls(n,nilNode) && ls(n2,nilNode)
                         && \separated(sloc(n), sloc(n2))
                         && label(x2,n) && label(x0,n) && label(x3,n2) && label(x1,nilNode)
                         && len(n)==1 && len(n2)>=1)
      || (\exists Node n1, n; ls(n,nilNode) && ls(n1,n)
                         && \separated(sloc(n), sloc(n1))
                         && label(x2,n) && label(x0,n1) && label(x3,nilNode) && label(x1,nilNode)
                         && len(n)==1 && len(n1)>=1)
      || (\exists Node n1, n, n2; ls(n,nilNode) && ls(n1,n) && ls(n2,nilNode) 
                         && \separated(sloc(n), sloc(n1), sloc(n2))
                         && label(x2,n) && label(x0,n1) && label(x3,n2) && label(x1,nilNode)
                         && len(n)==1 && len(n1)>=1 && len(n2)>=1)
         ;
   */
  while(x3 != NULL) {
    x1 = x3->next;
    x3->next = NULL;
    x3->next = x0;
    x0 = NULL;
    x0 = x3;
    x3 = NULL;
    x3 = x1;
    x1 = NULL;
  }
  return x0;
}
