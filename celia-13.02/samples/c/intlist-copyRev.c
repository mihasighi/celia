#include "linked_list.h"

/*@ requires acyclic(x); */
intlist
copyRevList (intlist x)
{
  intlist xi = x;
  intlist y, z = NULL;
  y = NULL;
  while (xi != NULL)
    {
      z = (intlist) malloc (sizeof (struct intlist_));
      z->data = xi->data;
      z->next = y;
      y = z;
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

  head = allocate (head, 10);	//list of 10 to 1 

  head1 = copyRevList (head);
  printf ("Output after reversing values\n");
  printlist (head1);
}
