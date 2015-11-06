
#include "intlist.h"

// Concatenates three lists in the result.

/*@ requires acyclic(x) && acyclic(y) && acyclic(z) && disjoint(x,y) && disjoint(x,z) && disjoint(y,z);
 */
intlist fold3_concat(intlist x, intlist y, intlist z) {
    intlist r, u, ui, tmp, li;
    r = u = ui = tmp = li = NULL;
    // Copy x
    li = x;
    while (li != NULL) {
        ui = (intlist) malloc(sizeof (struct intlist_));
        ui->data = li->data;
        ui->next = NULL;
        if (r == NULL) {
            r = ui;
            u = ui;
        } else {
            u->next = ui;
            u = NULL;
            u = ui;
        }
        ui = NULL;
        ui = li->next;
        li = NULL;
        li = ui;
        ui = NULL;
    }
    // Copy y after u
    li = y;
    while (li != NULL) {
        ui = (intlist) malloc(sizeof (struct intlist_));
        ui->data = li->data;
        ui->next = NULL;
        if (r == NULL) {
            r = ui;
            u = ui;
        } else {
            u->next = ui;
            u = NULL;
            u = ui;
        }
        ui = NULL;
        ui = li->next;
        li = NULL;
        li = ui;
        ui = NULL;
    }
    // Copy z after u
    li = z;
    while (li != NULL) {
        ui = (intlist) malloc(sizeof (struct intlist_));
        ui->data = li->data;
        ui->next = NULL;
        if (r == NULL) {
            r = ui;
            u = ui;
        } else {
            u->next = ui;
            u = NULL;
            u = ui;
        }
        ui = NULL;
        ui = li->next;
        li = NULL;
        li = ui;
        ui = NULL;
    }
    u = NULL;

    return r;
}
