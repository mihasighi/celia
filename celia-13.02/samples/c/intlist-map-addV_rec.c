
#include "intlist.h"

// Adds to each element of head the value v.
// Recursive version.

/*@ requires acyclic(head);
 */
void map_rec_addV(intlist head, int v) {
    if (head != NULL) {
      head->data = head->data + v;
      intlist curr;
      curr = NULL;
      curr = head->next;
      map_rec_addV(curr,v);
    }
}

