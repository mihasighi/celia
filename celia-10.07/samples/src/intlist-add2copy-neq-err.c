#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) and
 * acyclic(y) and l[y]+1<=_l and data(y) and
 * disjoint(x,y) */
void add2copy_neq(intlist x, intlist y) {
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL /* error */ ) {
    yi->data = xi->data + 2;
    xi = xi->next;
    yi = yi->next;
  }
}
