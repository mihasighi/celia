#include "linked_list.h"

/*@ requires acyclic (x); 
 */
void
maxim(intlist x) {
    intlist xi, xin,tmp;
    int val,v;
    xi = xin = NULL;
    tmp = NULL;
        xi = x;
        val = xi->data;
        xin = x;
        while (xin != NULL) {
            tmp = xin->next;
	    xin = NULL;
            xin = tmp;
            tmp = NULL;
	    if (xin != NULL) {
            if (xi->data > xin->data) {
                val  = xin->data;
		v = xi->data;
                xi->data = xin->data;
                xin->data = v;
		}	
	}
        }
        xi = xin = NULL;
}

