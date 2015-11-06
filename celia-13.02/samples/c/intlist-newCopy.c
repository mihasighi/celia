#include "linked_list.h"

/*@ requires acyclic(x); */
intlist
listCopy (intlist x)
{
  intlist xi = x;
  intlist y = NULL;
  intlist yi = NULL;;
  intlist z = NULL;
  while (xi != NULL)
    {
      z = (intlist) malloc (sizeof (struct intlist_));
      z->data = xi->data;
      z->next = NULL;
      if (y == NULL)
	y = z;
      else
	yi->next = z;
      yi = z;
      xi = xi->next;
    }
  return y;
}


void
main ()
{
  intlist head, head1;


  head = head1 = NULL;

  head = allocate (head, 10);	//list of 10 to 1 
  head1 = listCopy (head);

  printf ("List after calling ListCopy  \n");
  printlist (head1);

}
