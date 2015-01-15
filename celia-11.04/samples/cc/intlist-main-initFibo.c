#include "linked_list.h"

/*@ requires len >= 1;
 */
intlist init(int len) {
    intlist curr, head;
    int i;

    head = NULL;
    curr = NULL;

    for (i = 1; i <= len; i++) {
        curr = (intlist) malloc(sizeof (struct intlist_));
        curr->data = 0;
        curr->next = head;
        head = NULL;
        head = curr;
        curr = NULL;
    }
    return head;
}

/*@ requires acyclic(x); */
void
initfibo (intlist x)
{
  int m1 = 1;
  int m2 = 0;
  intlist xi ,tmp; 
  xi= NULL;
  xi =x;
  tmp = NULL;
  while (xi != NULL)
    {
      xi->data = m1 ;
      xi->data = xi->data + m2;
      m1 = m2;
      m2 = xi->data;
      tmp = xi->next;
      xi = NULL;
      xi = tmp;
      tmp = NULL;
    }
}

void
main ()
{
  intlist head;


  head = NULL;
int i = 10;
  head = init(i);	//list of 10 to 1 
i=i+1;
  initfibo (head);
i = i+1;
  printf ("Creating Fibonacci list  \n");
  //printlist (head);

}
