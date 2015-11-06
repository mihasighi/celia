#include "linked_list.h"

/*@ requires acyclic(x);
*/
void
dispatch5 (intlist x, intlist * xgtv, intlist * xlev)
{
  intlist xi = x;
  intlist y;
  *xgtv = NULL;
  *xlev = NULL;
  while (xi != NULL)
    {
      y = xi;
      xi = xi->next;
      if (y->data <= 5)
	{
	  y->next = *xlev;
	  *xlev = y;
	}
      else
	{
	  y->next = *xgtv;
	  *xgtv = y;
	}
    }
}


void
main ()
{

  intlist head, head1, head2;
  int i;

  head = head1 = head2 = NULL;


  head = allocate (head, 10);	//list of 1 to 10 
  printlist (head);

  dispatch5 (head, &head1, &head2);

  printf ("Output after splitting the list on value 5  \n");
  printlist (head1);
  printf ("Output after splitting the list on value 5 \n");
  printlist (head2);



}
