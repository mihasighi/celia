#include "linked_list.h"

/*@ requires acyclic(x);
*/
intlist
delAllGeV (intlist x, int v)
{
  intlist z;
  intlist y = x;
  intlist xi = x;

  while (xi != NULL)
    {
      if (xi->data >= v)
	{
	  if (xi == x)
	    x = xi->next;
	  z = xi;
	  xi = xi->next;
	  y->next = xi;

	  free (z);

	}
      else
	{
	  xi = xi->next;
	  y = y->next;
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
