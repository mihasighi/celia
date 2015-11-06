
#include "intlist.h"

// Merge sort: version creating new list

#define clone fold_clone
#define merge fold2_merge
#define split split

/*@ requires acyclic(x);
 */
intlist mergesort(intlist x) {
    intlist r, tmp;
    r = tmp = NULL;
    if (x != NULL) {
      tmp = x->next;
      if (tmp == NULL)
        r = clone(x);
      else {
        intlist *l1, *l2;
        tmp = NULL;
        *l1 = NULL;
        *l2 = NULL;
        split (x, l1, l2);
        tmp = mergesort (*l1);
        *l1 = NULL; *l1 = tmp; tmp = NULL;
        tmp = mergesort (*l2);
        *l2 = NULL; *l2 = tmp; tmp = NULL;
        tmp = merge (*l1, *l2);
        *l1 = NULL; *l2 = NULL;
        r = tmp; tmp = NULL;
      }
    }
    return r;
} // end merge_sort function


