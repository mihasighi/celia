#include "linked_list.h"

/*@ requires acyclic(x); */

void
setFlag (intlist x)
{
  intlist xi = x;
  while (xi != NULL)
    {
      if (!xi->data)
	{
	  xi->data = 1;
	}
      xi = xi->next;
    }
}

void
main ()
{
  intlist head;


  head = NULL;

  head = allocate_value (head, 10, 0);	//list of 10 with values 0

  printf ("List 1\n");
  printlist (head);
  setFlag (head);

  printf ("List after calling setFlag  \n");
  printlist (head);
}
