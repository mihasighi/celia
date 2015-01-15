#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
intlist copyRevList(intlist x) {
  intlist xi = x;
  intlist y, z = NULL;
  while (xi != NULL) {
    z = new();
    z->data = xi->data;
    z->next = y;
    y = z;
    xi = xi->next;
  }
  return y;
}
