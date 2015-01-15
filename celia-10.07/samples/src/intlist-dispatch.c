#include "intlist.h"

/* acyclic(x) and _l==l[x] and data(x) */
void dispatch(intlist x, 
             intlist xgtv,
             intlist xlev,
             int v) {
  intlist xi = x;
  intlist y;
  xgtv=NULL;  xlev=NULL;
  while (xi != NULL) {
    y=xi;
    xi=xi->next;
    if (y->data<=v) {
      y->next = xlev;
      xlev = y;
    }else {
      y->next = xgtv;
      xgtv = y;
    }
  }
}
