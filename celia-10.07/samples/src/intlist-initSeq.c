#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void seqInit(intlist x, int m) {
  int mp = m;
  intlist xi = x;
  while (xi != NULL) {
    xi->data = mp;
    mp = mp+1;
  }
}
