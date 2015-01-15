#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
intlist insertSortLst(intlist x) {
  intlist xi, y, yi, z, r;
  z = xi = yi = y = NULL;
  r = z = x;
  xi = x->next;
  while (xi != NULL) {
    yi = NULL;
    y = r;
    while (y != xi && y->data < xi->data) {
      yi = y;
      y = y->next;
    }
    if (yi == NULL) {
      z->next = xi->next;
      xi->next = r;
      r = xi;
    }
    else {
      z->next = xi->next;
      yi->next = xi;
      xi->next = y;
    }
    xi = NULL;
    xi = z->next;
  }
  return r;
}
