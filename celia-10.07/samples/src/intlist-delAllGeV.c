#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void delAllGeV(intlist x, int v) {
  intlist z;
  intlist y = null;
  intlist xi = x;
  while (xi != NULL) {
    if (xi->data >= v) {
      z = xi;
      xi = xi->next;
      free (z);
      if (y==NULL)
	x = xi;
      else
	y->next = xi;
    }
    else {
      y = xi;
      xi = xi->next;
    }
  }
}
