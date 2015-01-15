#include "linked_list.h"

/*@ requires acyclic(x);
    requires acyclic(y);
    requires disjoint(x,y); 
 */
void
listCopy (intlist x, intlist y)
{
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL /* error */ )
    {
      yi->data = xi->data;
      xi = xi->next;
      yi = yi->next;
    }
}

void
main ()
{
  intlist curr, head, head1;
  int i;

  head = head1 = NULL;

  head = allocate (head, 10);	//list of 10 to 1
  head1 = allocate_value (head1, 8, 0);
  printf ("values in list 2 before copying values of list 1 are \n");
  printlist (head1);
  listCopy (head, head1);
  printf ("Output after copying to new list of not equal length\n");
  printlist (head1);
}
