
#include "intlist.h"

// Quick sort: version creating new list

#define delLtFst fold_delLtFst
#define concat fold2_concat1

/*@ requires acyclic(a);
 */
intlist quicksort(intlist a) {
    intlist left, right, res, tmp;
    res = NULL;
    if (a != NULL) {
        tmp = a->next;
        if (tmp == NULL) {
            res = a;
        } else {
            tmp = NULL;
            left = delLtFst(a); /* remove left from a */
            tmp = quicksort(left);
            left = NULL;
            left = tmp;
            tmp = NULL;
            right = a->next;
            tmp = quicksort(right);
            right = NULL;
            right = tmp;
            tmp = NULL;
	    a->next = NULL;
	    a->next = right;
            res = concat(left, a);
            tmp = left = right = NULL;
        }
    }
    return res;
}
