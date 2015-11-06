#include "linked_list.h"

/*@ requires acyclic(x); */
void
initFibo (intlist x)
{
  int m1 = 1;
  int m2 = 0;
  intlist xi = x;
  while (xi != NULL)
    {
      xi->data = m1 + m2;
      m1 = m2;
      m2 = xi->data;
      xi = xi->next;
    }
}

void
main ()
{
  intlist head;


  head = NULL;

  head = allocate_value (head, 10, 0);	//list of 10 to 1 

  initFibo (head);

  printf ("Creating Fibonacci list  \n");
  printlist (head);

}
