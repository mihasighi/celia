
#include "intlist.h"

// Initializes head with the Fibonacci sequence.
// Iterative version.

/*@ requires acyclic(head);
 */
void map_initFibo(intlist head) {
    int m1 = 1;
    int m2 = 0;
    intlist xi, tmp;
    xi = tmp = NULL;
    xi = head;
    while (xi != NULL) {
        xi->data = m1 + m2;
        m1 = m2;
        m2 = xi->data;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
    }
}


