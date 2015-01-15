
#include "linked_list.h"

/* =================================================================== */
/* MAP2 class: Iterates over two lists to initialize their elements */
/* =================================================================== */

// Initializes each element of y with the corresponding element of x
// plus v.
// Iterative and recursive  versions.

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_addV_eq(intlist x, intlist y, int v) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL) {
        yi->data = xi->data + v;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_rec_addV_eq(intlist x, intlist y, int v) {
    if (x != NULL) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + v;
        map2_rec_addV_eq(xi, yi, v);
    }
}

// Instatiate v with 2 to obtain relations between sums and lengths

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_add2_eq(intlist x, intlist y) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL) {
        yi->data = xi->data + 2;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_rec_add2_eq(intlist x, intlist y) {
    if (x != NULL) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + 2;
        map2_rec_add2_eq(xi, yi);
    }
}

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_addV(intlist x, intlist y, int v) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL && yi != NULL) {
        yi->data = xi->data + v;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_rec_addV(intlist x, intlist y, int v) {
    if (x != NULL && y != NULL) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + v;
        map2_rec_addV(xi, yi, v);
    }
}

// Does not test the length of the list.

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_addV_err(intlist x, intlist y, int v) {
    intlist xi, yi, tmp;
    xi = yi = tmp = NULL;
    xi = x;
    yi = y;
    while (xi != NULL /* && yi != NULL */) {
        yi->data = xi->data + v;
        tmp = xi->next;
        xi = NULL;
        xi = tmp;
        tmp = NULL;
        tmp = yi->next;
        yi = NULL;
        yi = tmp;
        tmp = NULL;
    }
}

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_rec_addV_err(intlist x, intlist y, int v) {
    if (x != NULL /* && y != NULL */) {
        intlist xi, yi;
        xi = yi = NULL;
        xi = x->next;
        yi = y->next;
        y->data = x->data + v;
        map2_rec_addV(xi, yi, v);
    }
}

// Copies all elements of x in y.
// Iterative and recursive versions.

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_copy(intlist x, intlist y) {
    int v;
    v = 0;
    map2_addV(x, y, v);
}

/*@ requires acyclic(x) && acyclic(y);
 */
void map2_rec_copy(intlist x, intlist y) {
    int v;
    v = 0;
    map2_rec_addV(x, y, v);
}

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_copy_eq(intlist x, intlist y) {
    int v;
    v = 0;
    map2_addV_eq(x, y, v);
}

/*@ requires acyclic(x) && acyclic(y);
  @ requires len(x) == len(y);
 */
void map2_rec_copy_eq(intlist x, intlist y) {
    int v;
    v = 0;
    map2_rec_addV_eq(x, y, v);
}
