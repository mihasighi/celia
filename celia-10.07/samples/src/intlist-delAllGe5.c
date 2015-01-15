#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void delAllGe5(intlist x) {
  intlist z;
  intlist y = null;
  intlist xi = x;
  while (xi != NULL) {
    if (xi->data >= 5) {
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
