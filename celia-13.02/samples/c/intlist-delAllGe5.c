#include "linked_list.h"

/*@ requires acyclic(x); */
intlist
delAllGe5 (intlist x)
{

  intlist z;
  intlist y = x;
  intlist xi = x;

  while (xi != NULL)
    {
      if (xi->data >= 5)
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

  head = delAllGe5 (head);
  printf ("Output after deleting values > 5 \n");
  printlist (head);
}
