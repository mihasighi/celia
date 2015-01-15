#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
intlist add2new(intlist x) {
  intlist xi = x;
  intlist yi, y, z;
  yi = y = z = NULL;
  while (xi != NULL) {
    z = new();
    z->data = xi->data + 2;
    if (yi == NULL)
      y = z;
    else {
      yi->next = z;
      yi = NULL;
    }
    yi = z;
    z = NULL;
    xi = xi->next;
  }
  return y;
}
