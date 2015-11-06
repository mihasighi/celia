#include "linked_list.h"

/*@ requires acyclic(x);
    requires acyclic(y);
    requires acyclic(z);
    requires disjoint3(x,y,z); 
 */
void
initSum (intlist x, intlist y, intlist z)
{
  intlist xi = x;
  intlist yi = y;
  intlist zi = z;
  while (xi != NULL && yi != NULL && zi != NULL)
    {
      zi->data = xi->data + yi->data;
      xi = xi->next;
      yi = yi->next;
      zi = zi->next;
    }
}

void
main ()
{
  intlist head, head1, head2;


  head = head1 = head2 = NULL;

  head = allocate (head, 10);	//list of 10 to 1 
  head1 = allocate (head1, 10);
  head2 = allocate_value (head2, 10, 0);
  printf ("List 1 is  \n");
  printlist (head);
  printf ("List 2 is  \n");
  printlist (head1);


  initSum (head, head1, head2);

  printf ("List 3 after adding list1, list2 \n");
  printlist (head2);

}
