#include "linked_list.h"

/*@ requires acyclic(x);
 */
void
initN (intlist x)
{
  intlist xi = x;
  int m = 0;
  while (xi != NULL)
    {
      xi->data = m;
      xi = xi->next;
      m = m + 1;
    }
}

void
main ()
{
  intlist head;


  head = NULL;

  head = allocate_value (head, 10, 0);	//list of 10 to 1 
  initN (head);

  printf ("List after calling initN  \n");
  printlist (head);

}
