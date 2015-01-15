#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) and
 * acyclic(y) and l[y]+1<=_l and data(y) and
 * disjoint(x,y) */
void listCopy(intlist x, intlist y) {
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL && yi != NULL) {
    yi->data = xi->data;
    xi = xi->next;
    yi = yi->next;
  }
}
