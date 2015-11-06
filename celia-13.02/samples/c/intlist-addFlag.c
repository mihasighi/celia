#include "linked_list.h"

/*@ requires acyclic(x);
 */
void
addFlag (intlist x, int v)
{
  intlist xi = x;
  while (xi != NULL)
    {
      if (!xi->f)
	{
	  xi->data = xi->data + v;
	  xi->f = 1;
	}
      xi = xi->next;
    }
}
