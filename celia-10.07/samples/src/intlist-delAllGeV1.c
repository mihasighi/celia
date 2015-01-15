#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void delAllGeV(intlist x, int v) {
  intlist z;
  intlist y = x;
  intlist xi = x->next;
  while (xi != NULL) {
    if (xi->data >= v) {
      z = xi;
      xi = xi->next;
      y->next = xi;
      free (z);
    }
    else {
      xi = xi->next;
      y = y->next;
    }
  }
}
