
#include "linked_list.h"

/*@ requires acyclic(x) && acyclic(y) 
    && len(y)+1<=len(x)
    && disjoint(x,y); 
 */
void
add2copy_neq (intlist x, intlist y)
{
  intlist xi = x;
  intlist yi = y;
  while (xi != NULL && yi != NULL)
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
  head1 = allocate (head1, 8);
  add2copy_neq (head, head1);
  printf
    ("Output after Adding 2 and copying to new list of unequal length\n");
  printlist (head1);

}
