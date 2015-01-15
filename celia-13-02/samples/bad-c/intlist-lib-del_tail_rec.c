#include "intlist.h"

/*@ requires acyclic(head);
  @
 */
intlist del_tail_rec(intlist head) {
    if (head == NULL)
        return NULL;
    if (head->next == NULL) {
        free(head);
        return NULL;
    }
    intlist curr;
    curr = NULL;
    curr = head->next;
    head->next = NULL;
    head->next = del_tail_rec(curr);
    return head;
}

