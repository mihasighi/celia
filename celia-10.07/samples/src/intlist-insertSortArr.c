#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void insertSortArr(intlist x) {
  intlist xi, y;
  int m, n;
  xi = y = NULL;
  xi = x->next;
  while (xi != NULL) {
    y = x;
    while (y != xi && y->data <= xi->data) {
      y = y->next;
    }
    m = xi->data;
    while (y != xi) {
      n = y->data;
      y->data = m;
      m = n;
      y = y->next;
    }
    xi->data = m;
    xi = xi->next;
  }
}
