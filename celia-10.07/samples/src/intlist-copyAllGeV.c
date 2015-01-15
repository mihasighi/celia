#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void copyAllGeV(intlist x, int v) {
  intlist z;
  intlist y = null;
  intlist xi = x;
  while (xi != NULL) {
    if (xi->data >= v) {
      z = new();
      z->data = xi->data;
      z->next = y;
      y = z;
    }
    xi = xi->next;
  }
}
