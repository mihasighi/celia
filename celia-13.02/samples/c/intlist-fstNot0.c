#include "linked_list.h"


/*@ requires acyclic(x); */
intlist
fstNot0 (intlist x)
{
  intlist xi = x;
  while (xi != NULL && xi->data == 0)
    {
      xi = xi->next;
    }
  return xi;
}

void
main ()
{
  intlist head, head1;
  int i;

  head = head1 = NULL;

  head = allocate (head, 10);	//list of 10 to 1 


  printf ("List 1\n");
  printlist (head);
  head = fstNot0 (head);
  printf ("Output after removing nodes with value 0 \n");
  printlist (head);

  head1 = allocate_value (head1, 4, 0);


  printf ("list 2 \n");
  printlist (head1);
  head1 = fstNot0 (head1);
  printf ("Output from not zero node \n");
  if (head1 == NULL)
    printf ("Empty list\n");
  else
    printlist (head1);


}
