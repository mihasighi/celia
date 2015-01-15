#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
intlist sentinel(intlist x) {
  intlist xi = x;
  while (xi != NULL && xi->data <= 2) {
    xi = xi->next;
  }
  return xi;
}
