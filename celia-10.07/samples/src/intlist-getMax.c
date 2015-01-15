#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x)  */
int listMax(intlist x) {
  intlist xi = x;
  int max = x->data;
  while (xi != NULL) {
    if (max < xi->data) 
      max = xi->data;
    xi = xi->next;
  }
  return max;
}
