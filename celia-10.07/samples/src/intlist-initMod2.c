#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void initMod2(intlist x) {
  intlist xi = x;
  bool k = true;
  while (xi != NULL) {
    if (k) xi->data = 1;
    else   xi->data = 0;
    xi = xi->next;
    k = not(k);
  }
}
