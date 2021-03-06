/**************************************************************************/
/*                                                                        */
/*  This file is part of CINV.                                            */
/*                                                                        */
/*  Copyright (C) 2009-2011                                               */
/*    LIAFA (University of Paris Diderot and CNRS)                        */
/*                                                                        */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 3.                                                */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 3.                  */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#include "shape.h"
#include "shape_fun.h"
#include "shape_internal.h"
#include "apron2shape.h"
#include "shape_macros.h"


/*********************************************************/
/* Program for the verification of intlist-map-initMod2.c */
/*********************************************************/

/* ********************* */
/* Globals vars          */
/* ********************* */
ap_manager_t* ms; /* shape manager */
shape_internal_t* pr; /* internal manager */

size_t ptrdim = 4; /* vars r, tmp, x, xi */
size_t intdim = 0; /* vars k */
char* names_dim[4] = {"r", "tmp", "x", "xi"};

#define DIM_R 0
#define DIM_TMP 1
#define DIM_X 2
#define DIM_XI 3


/* ********************* */
/* General functions     */

/* ********************* */
void print_shape(const char* msg, shape_t* a) {
    fprintf(stdout, "%s = ", msg);
    shape_fprint(stdout, ms, a, names_dim);
    fprintf(stdout, "\n");
}


/* ********************** */
/* Target program         */

/* ********************** */

void reverse(void) {

    /* values computed for each sid */
    shape_t *top;
    shape_t *shp_pre; /* acyclic(x) && \length(x)>=1 && r(#) && xi(#) && tmp(#) */
    shape_t *sid_4; /* xi = x */
    shape_t *sid_5; /* while (xi != NULL) */
    shape_t *sid_6; /* tmp = xi->next; */
    shape_t *sid_7; /* xi->next = NULL; */
    shape_t *sid_8; /* xi->next = r; */
    shape_t *sid_9; /* r = NULL; */
    shape_t *sid_10; /* r = xi; */
    shape_t *sid_11; /* xi = NULL; */
    shape_t *sid_12; /* xi = tmp; */
    shape_t *sid_13; /* tmp = NULL; */
    shape_t *shp_inv, *shp_inv1, *shp_inv2, *shp_inv3, *shp_inv4, *shp_inv5;
    ap_tcons0_array_t pre, whileCond, notWhileCond, inv1, inv2, inv3, inv4, inv5;
    ap_texpr0_t *tnull, *tr, *ttmp, *tx, *txi, *t6, *t7, *t8;
    int algo;
    ap_dim_t r, tmp, xi; // for assignments

    printf("\nvvvvvvvvvv reverse (ok expected) vvvvvvvvvv\n");

    top = shape_top(ms, intdim, ptrdim);
#ifndef NDEBUG
    printf("\nvvvvv top:");
    shape_fdump(stdout, ms, top);
    printf("\n");
#endif

    /* init parameters: 0 1 P_1 */
    pr->max_anon = 0;
    pr->segm_anon = 1;
    algo = pr->max_anon | (pr->segm_anon << 4) | (1 << 8);
    shape_approximate(ms, top, algo);

    /*
     * init assertions
     */
    /* requires: acyclic(x) && \length(x)>=1 && r(#) && xi(#) && tmp(#) */
    pre = ap_tcons0_array_make(1);
    // ls(n,#) && x(n) && \length(n1) >= 1
    pre.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    shp_pre = shape_meet_tcons_array(ms, false, top, &pre);
    // the ptr variables not constrained BY THE ARRAY are set to #

    printf("\nvvvvv pre:");
    shape_fdump(stdout, ms, shp_pre);
    printf("\n");

    /* inv1: \exists Node n. ls(n,#) && x(n) && xi(n) && r(#) && tmp(#)
                         && \length(n)>=1 */
    inv1 = ap_tcons0_array_make(2);
    // ls(n,#) && x(n)
    inv1.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // x == xi
    inv1.p[1] = shape_tcons_x_y(AP_CONS_EQ, DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    shp_inv1 = shape_meet_tcons_array(ms, false, top, &inv1);

    /* inv2: \exists Node n. ls(n,#) && x(n) && r(n) && xi(#) && tmp(#)
                         && \length(n) == 1
     */
    inv2 = ap_tcons0_array_make(3);
    // ls(n,#) && x(n) && \length(n) >= 1
    inv2.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // r == x
    inv2.p[1] = shape_tcons_x_y(AP_CONS_EQ, DIM_X, 0, DIM_R, 0, intdim, ptrdim);
    // \length(n) == 1
    inv2.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // \length(n)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv2 = shape_meet_tcons_array(ms, false, top, &inv2);

    /* inv3: \exists Node n1, Node n, Node n2. ls(n,#) && ls(n1,n) && ls(n2,#)
                         && x(n) && r(n1) && xi(n2) && tmp(#)
                         && \length(n) == 1 && \length(n1)>=1 && \length(n2)>=1

     */
    inv3 = ap_tcons0_array_make(4);
    // ls(n,#) && x(n)
    inv3.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n1,n) && r(n2)
    inv3.p[1] = shape_tcons_reach_x_y(DIM_R, 0, DIM_X, 0, intdim, ptrdim);
    // ls(n2) && xi(n2)
    inv3.p[2] = shape_tcons_reach_x_y(DIM_XI, 0, NULL_DIM, 0, intdim, ptrdim);
    // \length(n) == 1
    inv3.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // \length(n)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv3 = shape_meet_tcons_array(ms, false, top, &inv3);

    /* inv4: \exists Node n1, Node n. ls(n,#) && ls(n1,#)
                         && x(n) && r(n1) && xi(#) && tmp(#)
                         && \length(n) == 1 && \length(n1)>=1
     */
    inv4 = ap_tcons0_array_make(3);
    // ls(n,#) && x(n)
    inv4.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n1,n) && r(n1)
    inv4.p[1] = shape_tcons_reach_x_y(DIM_R, 0, DIM_X, 0, intdim, ptrdim);
    // \length(n) == 1
    inv4.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // \length(n)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv4 = shape_meet_tcons_array(ms, false, top, &inv4);
    // all unconstrained pointer vars are set to #

    /* inv5: \exists Node n, Node n2. ls(n,#)
     *           && x(n) && r(n) && xi(n2)
     *           && \length(n) == 1 && \length(n2) >= 1
     */
    inv5 = ap_tcons0_array_make(4);
    // ls(n,#) && x(n)
    inv5.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // r == x
    inv5.p[1] = shape_tcons_x_y(AP_CONS_EQ, DIM_X, 0, DIM_R, 0, intdim, ptrdim);
    // ls(n2,#) && xi(n2)
    inv5.p[2] = shape_tcons_reach_x_y(DIM_XI, 0, NULL_DIM, 0, intdim, ptrdim);
    // \length(n) == 1
    inv5.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // \length(n)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv5 = shape_meet_tcons_array(ms, false, top, &inv5);
    // all unconstrained pointer vars are set to #

    shp_inv = shape_join(ms, true, shp_inv1, shp_inv2);
    shp_inv = shape_join(ms, true, shp_inv, shp_inv3);
    shp_inv = shape_join(ms, true, shp_inv, shp_inv4);
    shp_inv = shape_join(ms, true, shp_inv, shp_inv5);

    printf("\nvvvvv inv:");
    shape_fdump(stdout, ms, shp_inv);
    printf("\n");

    /*
     * init program
     */
    printf("\nvvvvv init program\n");
    r = DIM_R;
    xi = DIM_XI;
    tmp = DIM_TMP;
    tnull = shape_texpr_null(intdim, ptrdim); /* NULL */
    tr = shape_texpr_x_n_next(DIM_R, 0, intdim, ptrdim); /* r */
    ttmp = shape_texpr_x_n_next(DIM_TMP, 0, intdim, ptrdim); /* tmp */
    tx = shape_texpr_x_n_next(DIM_X, 0, intdim, ptrdim); /* x */
    txi = shape_texpr_x_n_next(DIM_XI, 0, intdim, ptrdim); /* xi */
    whileCond = ap_tcons0_array_make(1);
    whileCond.p[0] = shape_tcons_diff_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    notWhileCond = ap_tcons0_array_make(1);
    notWhileCond.p[0] = shape_tcons_same_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    t6 = shape_texpr_x_n_next(DIM_XI, 1, intdim, ptrdim); /* xi->next */
    t7 = shape_texpr_deref_next(shape_texpr_null(intdim, ptrdim)); /* NULL / next */
    t8 = shape_texpr_deref_next(shape_texpr_x_n_next(DIM_R, 0, intdim, ptrdim)); /* r / next */

    /* do verification */
    sid_4 = shape_assign_texpr_array(ms, true, shp_pre, &xi, &tx, 1, NULL); /* xi = x */

    if (!shape_is_leq(ms, sid_4, shp_inv)) {
        printf("\nvvvvv init => inv: not satisfied (ERROR)\n");
        pr->error_ ++;
    }
    else
        printf("\nvvvvv init => inv: satisfied\n");

    /* compute post(inv) */
    sid_5 = shape_meet_tcons_array(ms, false, shp_inv, &whileCond); /* while (xi != NULL) */
    sid_6 = shape_assign_texpr_array(ms, true, sid_5, &tmp, &t6, 1, NULL); /* tmp = xi->next; */
    sid_7 = shape_assign_texpr_array(ms, true, sid_6, &xi, &t7, 1, NULL); /* xi->next = NULL; */
    sid_8 = shape_assign_texpr_array(ms, true, sid_7, &xi, &t8, 1, NULL); /* xi->next = r;  */
    sid_9 = shape_assign_texpr_array(ms, true, sid_8, &r, &tnull, 1, NULL); /* r = NULL; */
    sid_10 = shape_assign_texpr_array(ms, true, sid_9, &r, &txi, 1, NULL); /* r = xi; */
    sid_11 = shape_assign_texpr_array(ms, true, sid_10, &xi, &tnull, 1, NULL); /* xi = NULL; */
    sid_12 = shape_assign_texpr_array(ms, true, sid_11, &xi, &ttmp, 1, NULL); /* xi = tmp; */
    sid_13 = shape_assign_texpr_array(ms, true, sid_12, &tmp, &tnull, 1, NULL); /* tmp = NULL; */

    printf("\nvvvvv post(inv):");
    shape_fdump(stdout, ms, sid_13);
    printf("\n");

    if (!shape_is_leq(ms, sid_13, shp_inv)) {
        printf("\nvvvvv post(inv) => inv: not satisfied (ERROR)\n");
        pr->error_ ++;
    }
    else
        printf("\nvvvvv post(inv) => inv: satisfied\n");

}


/* ********************************* */
/*           main                    */
/* ********************************* */

int main(int argc, const char** argv) {
    int i;

    /* init */
    ms = shape_manager_alloc();
    if (!ms) return 1;
    for (i = 0; i < AP_EXC_SIZE; i++) {
        ms->option.abort_if_exception[i] = true;
    }
    pr = shape_init_from_manager(ms, 0, 0);

    /* test */
    reverse();

    /* quit */
    if (pr->error_) printf("\n%i error(s)!\n", pr->error_);
    else printf("\nall tests passed\n");
    ap_manager_free(ms);
    return 0;
}
