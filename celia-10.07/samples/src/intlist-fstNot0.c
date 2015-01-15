#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
intlist fstNot0(intlist x) {
  intlist xi = x;
  while (xi != NULL && xi->data==0) {
    xi = xi->next;
  }
  return xi;
}
