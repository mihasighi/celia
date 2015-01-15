#include "linked_list.h"

/*@ requires acyclic(x); */
void
initMod2 (intlist x)
{
  intlist xi = x;
  int k = 0;
  while (xi != NULL)
    {
      xi->data = k;
      xi = xi->next;
      k = 1-k;
    }
}

void
main ()
{
  intlist head;


  head = NULL;

  head = allocate (head, 10);	//list of 10 to 1 
  printf ("List 1 is  \n");
  printlist (head);


  initMod2 (head);

  printf ("Mod 2  \n");
  printlist (head);

}
