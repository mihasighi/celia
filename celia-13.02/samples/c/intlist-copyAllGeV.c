#include "linked_list.h"

/*@ requires acyclic(x);
*/
intlist
copyAllGeV (intlist x, int v)
{
  intlist z;
  intlist y = NULL;
  intlist xi = x;
  while (xi != NULL)
    {
      if (xi->data >= v)
	{
	  z = (intlist) malloc (sizeof (struct intlist_));
	  z->data = xi->data;
	  z->next = y;
	  y = z;
	}
      xi = xi->next;
    }
  return z;
}

void
main ()
{
  intlist head, head1;
  int i;

  head = head1 = NULL;

  head = allocate (head, 10);	//list of 10 to 1 

  head1 = copyAllGeV (head, 4);
  printf
    ("Output after copying all  values > = value V here 4..,  into new list  \n");
  printlist (head1);
}
