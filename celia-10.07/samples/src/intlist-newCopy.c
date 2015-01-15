#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x)  */
intlist listCopy(intlist x) {
  intlist xi = x;
  intlist y = NULL;
  intlist yi = NULL;;
  intlist z = NULL;
  while (xi != NULL) {
    z = new();
    z->data = xi->data;
    z->next = NULL;
    if (y == NULL) 
      y = z;
    else
      yi->next = z;
    yi = z;
    xi = xi->next;
  }
  return y;
}
