#include "linked_list.h"

/*@ requires acyclic(x);
    requires acyclic(y);
    requires disjoint(x,y); 
 */
void
listCopy (intlist x, intlist y)
{
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL)
    {
      yi->data = xi->data;
      xi = xi->next;
      yi = yi->next;
    }
}
