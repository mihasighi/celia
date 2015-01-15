#include "intlist.h"

/*@ requires acyclic(head);
  @
 */
intlist del_tail_rec(intlist head) {
    intlist curr, tmp;
    curr = tmp = NULL;
    if (head == NULL)
        return NULL;
    curr = head->next;
    if (curr == NULL) {
        free(head);
        return NULL;
    }
    tmp = del_tail_rec(curr);
    head->next = NULL;
    head->next = tmp;
    curr = tmp = NULL; /* no more used */
    return head;
}

