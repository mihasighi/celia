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
/* Program for the verification of intlist-sorted-find.c */
/*********************************************************/

/* ********************* */
/* Globals vars          */
/* ********************* */
ap_manager_t* ms; /* shape manager */
shape_internal_t* pr; /* internal manager */

size_t ptrdim = 3; /* vars tmp, x, xi */
size_t intdim = 1; /* vars v */
char* names_dim[4] = {"v", "tmp", "x", "xi"};

#define DIM_V 0
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

void sorted_find(void) {

    /* values computed for each sid */
    shape_t *top;
    shape_t *shp_pre;
    shape_t *sid_4, *sid_5, *sid_6, *sid_7, *sid_8, *sid_9;
    shape_t *shp_inv, *shp_inv1, *shp_inv2, *shp_inv3;
    ap_tcons0_array_t pre, inv1, inv2, inv3;
    ap_tcons0_array_t whileCond, notWhileCond1, notWhileCond2;
    ap_texpr0_t *tnull, *tx, *txinext, *ttmp;
    int algo;
    ap_dim_t x, xi, tmp; // for assignments

    printf("\nvvvvvvvvvv sorted_find (ok expected) vvvvvvvvvv\n");

    top = shape_top(ms, intdim, ptrdim);
#ifndef NDEBUG
    printf("\nvvvvv top:");
    shape_fdump(stdout, ms, top);
    printf("\n");
#endif

    /* init parameters: 0 1 P_<= (et sa cloture) */
    pr->max_anon = 0;
    pr->segm_anon = 1;
    algo = pr->max_anon | (pr->segm_anon << 4) | (5 << 8);
    shape_approximate(ms, top, algo);

    /*
     * init assertions
     */
    /* requires: ls(n,#) && x(n) && \length(n)>=1 && sorted(x) */
    pre = ap_tcons0_array_make(2);
    // ls(n,#) && x(n) && len(n1) >= 1
    pre.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // \ucons(n) + 20 = 0
    // sorted(x)
    pre.p[1] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n2)
            ap_texpr0_cst_scalar_int(20),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_pre = shape_meet_tcons_array(ms, false, top, &pre);
    // the ptr variables not constrained BY THE ARRAY are set to #

    printf("\nvvvvv pre:");
    shape_fdump(stdout, ms, shp_pre);
    printf("\n");

    /* inv1: \exists Node n ; ls(n,#) && x(n) && xi(n) && tmp(#)
     *              && \length(n) >= 1 && sorted(n)
     */
    inv1 = ap_tcons0_array_make(3);
    // ls(n,#) && x(n) && len(n) >= 1
    inv1.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // xi == x
    inv1.p[1] = shape_tcons_x_y(AP_CONS_EQ, DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    // \ucons(n) + 20 = 0
    // sorted(x)
    inv1.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n2)
            ap_texpr0_cst_scalar_int(20),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv1 = shape_meet_tcons_array(ms, false, top, &inv1);

    /* inv2: \exists Node n, Node n1 ; ls(n,n1) * ls(n1,#)
     *              && x(n) && xi(n1) && tmp(#)
     *              && \length(n) >= 1 && \length(n1) >= 1
     *              && \data(n) <= \data(n1)
     *              && sorted(n) && sorted(n1)
     *              && \forall in y ; 1<= y < \length(n) ==> d(y)<= v-1 && d(y) <= d(n1)
     */
    inv2 = ap_tcons0_array_make(5);
    // ls(n1,#) && xi(n1)
    inv2.p[0] = shape_tcons_reach_x_y(DIM_XI, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n,n1) && x(n)
    inv2.p[1] = shape_tcons_reach_x_y(DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    // \data(n1) - \data(n) >= 0
    inv2.p[2] = ap_tcons0_make(AP_CONS_SUPEQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_XI), AP_RTYPE_INT, AP_RDIR_NEAREST), // \data(n)
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_INT, AP_RDIR_NEAREST), // \data(n)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // sorted(n) && \forall int y ; 1<= y < \length(n) ==> n[y]<=\data(n1)
    // \ucons(n) - \ucons(n1) + 20 = 0
    inv2.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(20),
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_XI), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n1)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \forall int y ; 1<= y < \length(n) ==> n[y]<=v-1
    // \ucons(n) - v + 21 = 0
    inv2.p[4] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_dim(DIM_V), 
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            ap_texpr0_cst_scalar_int(21),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv2 = shape_meet_tcons_array(ms, false, top, &inv2);
    // all unconstrained pointer vars are set to #

    /* inv3: ; ls(n,#) && x(n) && xi(#) && tmp(#)
     *              && \length(n) >= 1 && sorted(n)
     *              && \forall int y ; 1<= y < \length(n) ==> n[y]<=v-1
     */
    inv3 = ap_tcons0_array_make(3);
    // ls(n,#) && succ(n)
    inv3.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // sorted(n) 
    // \ucons(n) + 20 = 0
    inv3.p[1] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_cst_scalar_int(20),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \forall int y ; 1<= y < \length(n) ==> n[y]<=v-1
    // \ucons(n) - v + 21 = 0
    inv3.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(21),
            ap_texpr0_dim(DIM_V),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv3 = shape_meet_tcons_array(ms, false, top, &inv3);
    // all unconstrained pointer vars are set to #

    shp_inv = shape_join(ms, true, shp_inv1, shp_inv2);
    shp_inv = shape_join(ms, true, shp_inv, shp_inv3);

    printf("\nvvvvv inv:");
    shape_fdump(stdout, ms, shp_inv);
    printf("\n");

    /*
     * init program
     */
    printf("\nvvvvv init program\n");
    xi = DIM_XI;
    tmp = DIM_TMP;
    x = DIM_X;
    ttmp = shape_texpr_x_n_next(DIM_TMP, 0, intdim, ptrdim); /* tmp */
    tx = shape_texpr_x_n_next(DIM_X, 0, intdim, ptrdim); /* x */
    txinext = shape_texpr_x_n_next(DIM_XI, 1, intdim, ptrdim); /* xi->next */
    tnull = shape_texpr_null(intdim, ptrdim); /* null */
    whileCond = ap_tcons0_array_make(2);
    // xi != NULL
    whileCond.p[0] = shape_tcons_diff_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    // v - xi->data > 0
    whileCond.p[1] = ap_tcons0_make(AP_CONS_SUP,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_dim(DIM_V),
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_XI), AP_RTYPE_INT, AP_RDIR_NEAREST), // succ->data
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    notWhileCond1 = ap_tcons0_array_make(1);
    // xi == NULL
    notWhileCond1.p[0] = shape_tcons_same_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    notWhileCond2 = ap_tcons0_array_make(1);
    // succ->data - v >= 0
    notWhileCond2.p[0] = ap_tcons0_make(AP_CONS_SUPEQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_XI), AP_RTYPE_INT, AP_RDIR_NEAREST), // succ->data
            ap_texpr0_dim(DIM_V),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    
    /* do verification: invariant checking */
    sid_4 = shape_assign_texpr_array(ms, true, shp_pre, &xi, &tx, 1, NULL); /* xi = x; */

    printf("\nvvvvv init:");
    shape_fdump(stdout, ms, sid_4);
    printf("\n");

    if (!shape_is_leq(ms, sid_4, shp_inv))
        printf("\nvvvvv init => inv: not satisfied (ERROR)\n");
    else
        printf("\nvvvvv init => inv: satisfied\n");


    sid_5 = shape_meet_tcons_array(ms, false, shp_inv, &whileCond); /* while (xi != NULL && xi->data < v) */
    sid_6 = shape_assign_texpr_array(ms, true, sid_5, &tmp, &txinext, 1, NULL); /* tmp = xi->next; */
    sid_7 = shape_assign_texpr_array(ms, true, sid_6, &xi, &tnull, 1, NULL); /* xi = NULL; */
    sid_8 = shape_assign_texpr_array(ms, true, sid_7, &xi, &ttmp, 1, NULL); /* xi = tmp; */
    sid_9 = shape_assign_texpr_array(ms, true, sid_8, &tmp, &tnull, 1, NULL); /* tmp = NULL; */
    
    printf("\nvvvvv post(inv):");
    shape_fdump(stdout, ms, sid_9);
    printf("\n");

    if (!shape_is_leq(ms, sid_9, shp_inv))
        printf("\nvvvvv post(inv) => inv: not satisfied (ERROR)\n");
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
    sorted_find();

    /* quit */
    if (pr->error_) printf("\n%i error(s)!\n", pr->error_);
    else printf("\nall tests passed\n");
    ap_manager_free(ms);
    return 0;
}
