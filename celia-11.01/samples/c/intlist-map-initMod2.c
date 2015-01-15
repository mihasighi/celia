
#include "intlist.h"

// Initializes head with the sequence of modulo 2.
// Iterative version.

/*@ requires acyclic(head);
 */
void map_initMod2(intlist head) {
    intlist xi, tmp;
    int k = 0;
    xi = tmp = NULL;
    xi = head;
    while (xi != NULL) {
        xi->data = k;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        k = 1 - k;
    }
}


