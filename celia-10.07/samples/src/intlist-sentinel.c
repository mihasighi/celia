#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
intlist sentinel(intlist x, int m) {
  intlist xi = x;
  while (xi != NULL && xi->data <= m) {
    xi = xi->next;
  }
  return xi;
}
