#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void setFlag(intlist x) {
  intlist xi = x;
  while (xi != NULL) {
    if (!xi->data) {
      xi->data = 1;
    }
    xi = xi->next;
}
