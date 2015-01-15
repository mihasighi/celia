#include "linked_list.h"

// Insertion sort.
// Version moving data like in arrays.

/*@ requires acyclic(x);
 */
intlist 
insertsortA(intlist x)
{

  intlist xi;
  intlist y;
  intlist yi;
  int m;
  int n;

  xi = NULL;
  y = NULL;
  yi = NULL;
  xi = x->next;
  while (xi != NULL)
    {
      y = x;
      while (y != xi) {
        if (y->data <= xi->data)
	{
	  yi = y->next;
	  y = NULL;
	  y = yi;
	  yi = NULL;
	}
        else break;
      }
      m = xi->data;
      while (y != xi)
	{
	  n = y->data;
	  y->data = m;
	  m = n;
	  yi = y->next;
	  y = NULL;
	  y = yi;
	  yi = NULL;
	}
      y = NULL;
      xi->data = m;
      yi = xi->next;
      xi = NULL;
      xi = yi;
      yi = NULL;
    }
   xi = NULL;
   yi = NULL;
   y = NULL;
}
