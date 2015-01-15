#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) and
 * acyclic(y) and l[y]==_l and data(y) and
 * disjoint(x,y) */
int equal(intlist x, intlist y) {
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL && yi != NULL &&
         xi->data == yi->data) {
    xi = xi->next;
    yi = yi->next;
  }
  if (xi==NULL && yi==NULL)
    return 1;
  else
    return 0;
}
