#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void initN(intlist x) {
  intlist xi = x;
  int m = 0;
  while (xi != NULL) {
    xi->data = m;
    xi = xi->next;
    m = m+1;
  }
}
