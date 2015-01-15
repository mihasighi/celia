#include <stdlib.h>

typedef int list_node_type;

typedef struct list_Node {
  // list_node_type info;
  int data;
  struct list_Node * next;
  //struct list_Node * prev;
}list;

// - acyclic predicate
/*@ predicate acyclic(list* l) = 
  @    l!=\null;
 */

/*@ requires acyclic(head);
 */
int Count_Node(list* head) {
  list* p = head;
  int i = 0;
  while (p != NULL) {
    list* tmp = NULL;
    tmp = p->next;
    p = NULL;
    p = tmp;
    tmp = NULL;
    i = i + 1; 
  }
  return i;
}

