
#include "intlist.h"

// Reverse in place the list x2 and return the head (last element) of the new list

/*@ requires \exists Node n. ls(n,#) && x2(n) && \length(n)>=1;
  @ ensures  \exists Node n1, Node n. ls(n1,n) * ls(n,#) &&
                 x0(n1) && x2(n) && \length(n1)>=1 && \length(n) == 1
          || \exists Node n. ls(n,#) &&
                 x0(n) && x2(n) && \length(n) == 1
             ;
 */
intlist
fold_reverse(intlist x2) {
  intlist x0, x3, x1;
  x0 = x3 = x1 = NULL;
  x3 = x2;
  /*@ loop invariant
         \exists Node n. ls(n,#) && x2(n) && x3(n) && x0(#) && x1(#)
                         && \length(n)>=1
      || \exists Node n. ls(n,#) && x2(n) && x0(n) && x3(#) && x1(#)
                         && \length(n) == 1
      || \exists Node n1, Node n, Node n2. ls(n,#) * ls(n1,n) * ls(n2,#)
                         && x2(n) && x0(n1) && x3(n2) && x1(#)
                         && \length(n) == 1 && \length(n1)>=1 && \length(n2)>=1
      || \exists Node n1, Node n. ls(n,#) * ls(n1,#)
                         && x2(n) && x0(n1) && x3(#) && x1(#)
                         && \length(n) == 1 && \length(n1)>=1
      || \exists Node n, Node n2. ls(n,#) * ls(n2,#)
     		         && x2(n) && x0(n) && x3(n2) && x1(#)
                         && \length(n) == 1 && \length(n2) >= 1
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
