#include "linked_list.h"

/* acyclic(x) and 
 * l[x]+l[y]==_l and data(xy) and
 * reach(x,y) */

/*@ requires acyclic(x); 
  @ requires reach(x,y);
 */
void
partialInit (intlist x, intlist y)
{
  intlist yi = y;
  while (yi != NULL)
    {
      yi->data = 0;
      yi = yi->next;
    }
}

void
main ()
{
  intlist head, head1;


  head = head1 = NULL;

  head = allocate (head, 10);	//list of 10 to 1 


  printf ("list before calling partial init \n");
  printlist (head);

  partialInit (head, head->next->next);


  printf ("List after calling partialInit after 2 elements \n");
  printlist (head);

}
