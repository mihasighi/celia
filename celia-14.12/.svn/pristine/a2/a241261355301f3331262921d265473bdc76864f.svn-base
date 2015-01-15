#include "intlist.h"

// Deletes the last element of head.
// Iterative version.

/*@ requires acyclic(head);
  @
 */
intlist del_tail(intlist head) {
    intlist h, hp, tmp;
    h = hp = tmp = NULL;
    if (head == NULL)
        return NULL;
    h = head;
    tmp = h->next;
    if (tmp == NULL) {
        free(head);
        return NULL;
    }
    while (tmp != NULL) {
         hp = NULL;
         hp = h;
         h = NULL;
         h = tmp;
         tmp = NULL;
         tmp = h->next;
    }
    free(h);
    hp->next = NULL;
    hp = NULL; /* no more used */
    return head;
}

