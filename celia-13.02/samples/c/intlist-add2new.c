#include "linked_list.h"

/*@ requires acyclic (x); */
intlist
add2new (intlist x)
{
  intlist xi = x;
  intlist yi, y, z;
  yi = y = z = NULL;
  while (xi != NULL)
    {
      z = (intlist) malloc (sizeof (struct intlist_));
      z->data = xi->data + 2;
      if (yi == NULL)
	y = z;
      else
	{
	  yi->next = z;
	  yi = NULL;
	}
      yi = z;
      z = NULL;
      xi = xi->next;
    }
  return y;
}

void
main ()
{

  intlist curr, head, head1;
  int i;

  head = head1 = NULL;

  head = allocate (head, 10);	//list of 1 to 10 
  head1 = add2new (head);
  printf ("Output after Adding 2 and creating  to new list \n");
  printlist (head1);
}
