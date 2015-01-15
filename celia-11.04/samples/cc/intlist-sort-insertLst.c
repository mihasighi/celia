#include "linked_list.h"

/*@ requires acyclic(x); */

intlist
insertSortLst (intlist x)
{
  intlist xi, xip, y, yi, r;
  xi = xip = yi = y = NULL;
  r = xip = x;
  xi = x->next;
  while (xi != NULL)
    {
      y = r;
      while (y != xi && y->data < xi->data)
	{
          yi = NULL;
	  yi = y;
          y = NULL;
	  y = yi->next;
	}
      if (yi == NULL) /* insert fst position */
	{
	  xip->next = NULL;
	  xip->next = xi->next;
	  xi->next = NULL;
	  xi->next = r;
	  r = NULL;
	  r = xi;
	}
      else if (y != xi) /* insert between yi and y */
	{
	  xip->next = NULL;
	  xip->next = xi->next;
	  yi->next = NULL;
	  yi->next = xi;
	  xi->next = NULL;
	  xi->next = y;
	}
      else /* nothing to do */
        {
          xip = NULL;
          xip = xi;
        }
      y = yi = NULL; 
      xi = NULL;
      xi = xip->next;
    }
  return r;
}

