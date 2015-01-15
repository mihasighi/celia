#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x)  */
int listSum(intlist x) {
  intlist xi = x;
  int sum = 0;
  while (xi != NULL) {
    sum = sum + xi->data; 
    xi = xi->next;
  }
  return sum;
}
