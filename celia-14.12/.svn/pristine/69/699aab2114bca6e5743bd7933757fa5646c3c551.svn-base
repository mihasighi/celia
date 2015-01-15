#include "linked_list.h"

/*@ requires acyclic(x); */

intlist
sentinel (intlist x, int m)
{
  intlist xi = x;
  while (xi != NULL && xi->data <= m)
    {
      xi = xi->next;
    }
  return xi;
}


void
main ()
{
  intlist head;
  int m = 10;

  head = NULL;

  head = allocate (head, 10);	//list of 10 to 1 


  printf ("list before calling sentinel \n");
  printlist (head);

  head = sentinel (head, m);


  printf ("List after calling sentinel with %d value  \n", m);
  if (head == NULL)
    printf ("Empty List \n");
  else
    printlist (head);

}
