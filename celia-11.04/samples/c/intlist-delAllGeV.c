#include "linked_list.h"

/*@ requires acyclic(x);
*/
intlist
delAllGeV (intlist x, int v)
{
  intlist z;
  intlist y = NULL;
  intlist xi = x;
  while (xi != NULL)
    {
      if (xi->data >= v)
	{
	  z = xi;
	  xi = xi->next;
	  free (z);
	  if (y == NULL)
	    x = xi;
	  else
	    y->next = xi;
	}
      else
	{
	  y = xi;
	  xi = xi->next;
	}
    }
  return x;
}

void
main ()
{
  intlist head;
  int i;

  head = NULL;

  head = allocate (head, 10);	//list of 10 to 1 

  head = delAllGeV (head, 4);
  printf ("Output after deleting values > value V here 4.. \n");
  printlist (head);
}
