#include "linked_list.h"

/*@ requires acyclic(x) && acyclic(y) && acyclic(z)  && disjoint3(x,y,z) ; */
void
initSum (intlist x, intlist y, intlist z)
{
  intlist xi = x;
  intlist yi = y;
  intlist zi = z;
  while (xi != NULL && yi != NULL && zi != NULL)
    {
      zi->data = xi->data + yi->data;
      xi = xi->next;
      yi = yi->next;
      zi = zi->next;
    }
}
