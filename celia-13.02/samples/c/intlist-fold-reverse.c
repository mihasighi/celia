
#include "intlist.h"

// Reverse in place the list x and return the head (last element) of the new list
// Iterative version

/*@ requires acyclic(x);
 */
intlist
fold_reverse(intlist x) {
  intlist r, xi, tmp;
  r = xi = tmp = NULL;
  xi = x;
  while(xi != NULL) {
    tmp = xi->next;
    xi->next = NULL;
    xi->next = r;
    r = NULL;
    r = xi;
    xi = NULL;
    xi = tmp;
    tmp = NULL;
  }
  return r;
}
