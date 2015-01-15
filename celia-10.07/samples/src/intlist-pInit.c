#include "intlist.h"

/* acyclic(x) and 
 * l[x]+l[y]==_l and data(xy) and
 * reach(x,y) */
void partialInit(intlist x, 
                 intlist y) {
  intlist yi = y;
  while (yi != NULL) {
    yi->data = 0;
    yi = yi->next;
  }
}
