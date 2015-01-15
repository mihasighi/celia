#include "linked_list.h"

/*@ requires acyclic (x); 
 */
void
bubbleSort(intlist x) {
    intlist xi, xin;
    int v;
    int k = 1;
    xi = xin = NULL;
    while (k == 1) {
        k = 0;
        xi = x;
        xin = x->next;
        while (xi != NULL) {
            if (xin == NULL)
                break;
            if (xi->data > xin->data) {
                v = xi->data;
                xi->data = xin->data;
                xin->data = v;
                k = 1;
            }
            xi = NULL;
            xi = xin;
            xin = NULL;
            xin = xi->next;
        }
        xi = xin = NULL;
    }
}

