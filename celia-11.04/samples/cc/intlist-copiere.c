#include "linked_list.h"

/*@ requires len >= 1;
 */
intlist init(int len) {
    intlist curr, head,head2,curr2;
    int i;

    head = NULL;
    curr = NULL;
   
 //   head2 = NULL;
 //   curr2 = NULL; 

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

/*@ requires acyclic(h); */

void initlist(intlist h) {
    intlist curr, curr2;
    int i;

    curr = NULL;
    curr = h;
    curr2 = NULL;

   while (curr != NULL){
       curr->data = 0;
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
  return y;
}

/*@ requires acyclic(head);
  @
 */
void
printlist(intlist head) {
    intlist tmp;
    intlist curr = head;

    while (curr) {
        printf("%d ", curr->data);
        tmp = curr->next;
        curr = NULL;
        curr = tmp;
        tmp = NULL;
    }
    printf("\n");
}

/*@ requires acyclic(head); */

void
amain (intlist head)
{

  intlist head1, hd,tmp;

  head1 = NULL;
  hd = NULL;
  tmp = NULL;

  initlist (head);	//list of 1 to 10
 
  head1 = listcopy (head);

//  hd = listCopy(head);

  printf ("Output after copying every node of list\n");
//  printlist (head1);
}


