#include "linked_list.h"


/*@ requires acyclic(x);
    requires acyclic(y);
    requires disjoint(x,y); 
 */
int
equal (intlist x, intlist y)
{
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL && yi != NULL && xi->data == yi->data)
    {
      xi = xi->next;
      yi = yi->next;
    }
  return (xi == NULL && yi == NULL) ? 1 : 0;
}

void
main ()
{
  intlist head, head1;
  int i;

  head = NULL;
  head1 = NULL;

  head = allocate (head, 10);	//list of 10 to 1 
  head1 = allocate (head1, 10);
  i = equal (head, head1);
  if (i)
    printf ("The two lists are equal  \n");
  else
    printf ("The two lists are unequal \n");

  printf ("List 1 \n");
  printlist (head);
  printf ("List 2 \n");
  printlist (head1);

}
