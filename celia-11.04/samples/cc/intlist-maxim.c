#include "linked_list.h"

/*@ requires acyclic (x); 
 */
void
maxim(intlist x) {
    intlist xi, xin,tmp;
    int v;
    xi = xin = NULL;
    tmp = NULL;
        xi = x;
        xin = x->next;
        while (xin != NULL) {
            if (xi->data > xin->data) {
                v = xi->data;
                xi->data = xin->data;
                xin->data = v;
            }
            tmp = xin->next;
	    xin = NULL;
            xin = tmp;
            tmp = NULL;
        }
        xi = NULL;
        xin = NULL;
}

