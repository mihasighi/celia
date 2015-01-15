#include "intlist.h"

/* acyclic(x) and l[x]==_l and data(x) */
void bubbleSortArr(intlist x) {
  intlist xi, xin;
  int v;
  int k = 1;
  while (k==1) {
    k = 0;
    xi = x;
    xin = x->next;
    while (xi != NULL && xin != NULL) {
      if (xi->data >= xin->data+1) {
        v = xi->data;
	xi->data = xin->data;
        xin->data = v;
        k = 1;
      }
      xi = xin;
      xin = xin->next;
    }
  }
} 
