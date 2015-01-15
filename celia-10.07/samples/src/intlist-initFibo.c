#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void initFibo(intlist x) {
  int m1 = 1;
  int m2 = 0;
  intlist xi = x;
  while (xi != NULL) {
    xi->data = m1+m2;
    m1 = m2;
    m2 = xi->data;
    xi = xi->next;
  }
}
