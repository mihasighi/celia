
#include "linked_list.h"

/*@ requires acyclic(x);
    requires acyclic(y);
    requires disjoint(x,y); 
    requires len(x)==len(y);
 */
void
add2copy_eq (intlist x, intlist y)
{
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL)
    {
      yi->data = xi->data + 2;
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

  head = allocate (head, 10);	//list of 1 to 10 
  head1 = allocate (head1, 10);
  add2copy_eq (head, head1);
  printf ("Output after Adding 2 and copying to new list of equal length\n");
  printlist (head1);

}
