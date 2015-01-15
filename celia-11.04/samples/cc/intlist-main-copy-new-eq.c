#include "linked_list.h"


/*@ requires acyclic(h); */

void initlist(intlist h,int val) {
    intlist curr, curr2;
    int i;

    curr = NULL;
    curr = h;
    curr2 = NULL;

   while (curr != NULL){
       curr->data = val;
       curr2 = curr->next;
       curr = NULL;
       curr = curr2;
       curr2 = NULL;
   }
    //return head;
}

/*@ requires acyclic(x); */
intlist
listcopy (intlist x)
{
  
  intlist xi = x;
  intlist y = NULL;
  intlist yi = NULL;;
  intlist z = NULL;
  intlist s = NULL;
  intlist a = NULL;
  while (xi != NULL)
    {
      z= NULL;
      z = (intlist) malloc (sizeof (struct intlist_));
      z->data = xi->data;
      z->next = NULL;
      if (y == NULL)
    	  y = z;
      else{
    	  yi->next = NULL;
    	  yi->next = z;
      }
      yi = NULL;
      yi = z;
      a = xi;
      xi = NULL;
      xi = a->next;
      a = NULL;
    }
  yi = NULL;
  z = NULL;
  return y;
}


/*@ requires acyclic(head); */

void
amain (intlist head, int val)
{

  intlist head1, hd,tmp;

  head1 = NULL;
  hd = NULL;
  tmp = NULL;

  initlist (head,val);	//list of 1 to 10
 
  head1 = listcopy (head);

//  hd = listCopy(head);

  printf ("Output after copying every node of list\n");
}


