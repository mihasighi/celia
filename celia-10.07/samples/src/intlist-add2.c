#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void add2(intlist x) {
  intlist xi = x;
  while (xi != NULL) {
    xi->data = xi->data + 2;
    xi = xi->next;
  }
}
