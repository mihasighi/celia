#include "intlist.h"

/* acyclic(x) and _l==l[x] and data(x) and
 * acyclic(y) and _l==l[y] and data(y) and
 * acyclic(z) and _l==l[z] and data(z) and
 * disjoint(x,y,z) */
void initSum(intlist x, 
             intlist y,
             intlist z) {
  intlist xi = x;
  intlist yi = y;
  intlist zi = z;
  while (xi != NULL && yi != NULL && zi != NULL) {
    zi->data = xi->data + yi->data;
    xi = xi->next;
    yi = yi->next;
    zi = zi->next;
  }
}
