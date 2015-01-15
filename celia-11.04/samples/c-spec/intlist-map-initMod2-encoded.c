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

size_t ptrdim = 3; /* vars tmp, x, xi */
size_t intdim = 1; /* vars k */
char* names_dim[4] = {"k", "tmp", "x", "xi"};

#define DIM_K 0
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

void initMod2(void) {

    /* values computed for each sid */
    shape_t *top;
    shape_t *shp_pre; /* acyclic(x) && len(x)>=1 && xi == tmp == NULL */
    shape_t *sid_4; /* xi = x */
    shape_t *sid_5; /* while (xi != NULL) */
    shape_t *sid_6; /* xi->data = k; */
    shape_t *sid_7; /* tmp = xi->next */
    shape_t *sid_8; /* xi = NULL; */
    shape_t *sid_9; /* xi = tmp; */
    shape_t *sid_10; /* tmp = NULL; */
    shape_t *sid_11; /* k = 1-k; */
    shape_t *shp_inv, *shp_inv1, *shp_inv2, *shp_inv3;
    ap_tcons0_array_t pre, whileCond, notWhileCond, inv1, inv2, inv3;
    ap_texpr0_t *tnull, *ttmp, *tx, *t6, *t7, *t11;
    int algo;
    ap_dim_t k, xi, tmp; // for assignments

    printf("\nvvvvvvvvvv initMod2 (ok expected) vvvvvvvvvv\n");

    top = shape_top(ms, intdim, ptrdim);
#ifndef NDEBUG
    printf("\nvvvvv top:");
    shape_fdump(stdout, ms, top);
    printf("\n");
#endif

    /* init parameters: 0 1 P_<1 */
    pr->max_anon = 0;
    pr->segm_anon = 1;
    algo = pr->max_anon | (pr->segm_anon << 4) | (17 << 8);
    shape_approximate(ms, top, algo);

    /*
     * init assertions
     */
    /* requires: ls(n,#) && x(n) && \length(n)>=1 && k = 0 */
    pre = ap_tcons0_array_make(2);
    // ls(n,#) && x(n) && \length(n1) >= 1
    pre.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // v = 0
    pre.p[1] = ap_tcons0_make(AP_CONS_EQ, ap_texpr0_dim(DIM_K), NULL);
    shp_pre = shape_meet_tcons_array(ms, false, top, &pre);
    // the ptr variables not constrained BY THE ARRAY are set to #
#ifndef NDEBUG
    printf("\nvvvvv pre:");
    shape_fdump(stdout, ms, shp_pre);
    printf("\n");
#endif

    /* inv1: ls(n,#) && x(n) && xi(n) && k = 0 */
    inv1 = ap_tcons0_array_make(3);
    // ls(n,#) && x(n)
    inv1.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // x == xi
    inv1.p[1] = shape_tcons_x_y(AP_CONS_EQ, DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    inv1.p[2] = ap_tcons0_make(AP_CONS_EQ, ap_texpr0_dim(DIM_K), NULL);
    // k = 0
    shp_inv1 = shape_meet_tcons_array(ms, false, top, &inv1);

    /* inv2: ls(n2,n1) * ls(n1,#) && x(n2) && xi(n1) && tmp(#)
     *       \data(n2) = 0 && 0 <= k <= 1 &&
     *       && \forall y1, y2. 1<= y1 <_1 y2 < \length(n2) ==> \data(y1)+data(y2)=1
     *       && \forall y. 1<= y < \length(n2) && y = \length(n2)-1 ==> \data(y)=1-k
     */
    inv2 = ap_tcons0_array_make(7);
    // ls(n2,n1) && x(n2) && xi(n1)
    inv2.p[0] = shape_tcons_reach_x_y(DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    // ls(n1,#)
    inv2.p[1] = shape_tcons_reach_x_y(DIM_XI, 0, NULL_DIM, 0, intdim, ptrdim);
    // \data(n2) = 0
    inv2.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_INT, AP_RDIR_NEAREST), // \data(n2)
            NULL);
    // \forall y1, y2. 1<= y1 <_1 y2 < \length(n2) ==> \data(y1)+data(y2)=1
    // \ucons(n2) + 2 = 0
    inv2.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n2)
            ap_texpr0_cst_scalar_int(2),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \forall y. 1<= y < \length(n2) && y = \length(n2)-1 ==> \data(y)=1-k
    // \ucons(n2) -k + 3 = 0
    inv2.p[4] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN),// \ucons(n2)
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(3),
            ap_texpr0_dim(DIM_K),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    inv2.p[5] = ap_tcons0_make(AP_CONS_SUPEQ, ap_texpr0_dim(DIM_K), NULL);
    // k >= 0
    inv2.p[6] = ap_tcons0_make(AP_CONS_SUPEQ,
    		ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(1),
            ap_texpr0_dim(DIM_K), AP_RTYPE_INT, AP_RDIR_RND), NULL);
    // 1 - k >=0


    shp_inv2 = shape_meet_tcons_array(ms, false, top, &inv2);

    /* inv3: ls(n,#) && x(n) && xi(#) && tmp(#) &&
     *       \data(n) = 0 && 0 <= k <= 1 &&
     *       && \forall y1, y2. 1<= y1 <_1 y2 < \length(n) ==> \data(y1)+data(y2)=1
     *       && \forall y. 1<= y < \length(n) && y = \length(n)-1 ==> \data(y)=1-k
     */
    inv3 = ap_tcons0_array_make(6);
    // ls(n,#) && x(n)
    inv3.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // \data(n) = 0
    inv3.p[1] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_INT, AP_RDIR_NEAREST), // \data(n)
            NULL);
    // \forall y1, y2. 1<= y1 <_1 y2 < \length(n) ==> \data(y1)+data(y2)=1
    // \ucons(n) + 2 = 0
    inv3.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_cst_scalar_int(2),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \forall y. 1<= y < \length(n) && y = \length(n)-1 ==> \data(y)=1-k
    // \ucons(n) - k + 3 = 0
    inv3.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(3),
            ap_texpr0_dim(DIM_K),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    inv3.p[4] = ap_tcons0_make(AP_CONS_SUPEQ, ap_texpr0_dim(DIM_K), NULL);
    // k >= 0
    inv3.p[5] = ap_tcons0_make(AP_CONS_SUPEQ,
    		ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(1),
            ap_texpr0_dim(DIM_K), AP_RTYPE_INT, AP_RDIR_RND), NULL);
    // 1 - k >=0
    shp_inv3 = shape_meet_tcons_array(ms, false, top, &inv3);
    // all unconstrained pointer vars are set to #

    shp_inv = shape_join(ms, true, shp_inv1, shp_inv2);
    shp_inv = shape_join(ms, true, shp_inv, shp_inv3);

#ifndef NDEBUG
    printf("\nvvvvv inv:");
    shape_fdump(stdout, ms, shp_inv);
    printf("\n");
#endif

    /*
     * init program
     */
    printf("\nvvvvv init program\n");
    k = DIM_K;
    xi = DIM_XI;
    tmp = DIM_TMP;
    tnull = shape_texpr_null(intdim, ptrdim); /* NULL */
    ttmp = shape_texpr_x_n_next(DIM_TMP, 0, intdim, ptrdim); /* tmp */
    tx = shape_texpr_x_n_next(DIM_X, 0, intdim, ptrdim); /* x */
    whileCond = ap_tcons0_array_make(1);
    whileCond.p[0] = shape_tcons_diff_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    notWhileCond = ap_tcons0_array_make(1);
    notWhileCond.p[0] = shape_tcons_same_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    t6 = shape_texpr_deref_data(ap_texpr0_dim(DIM_K)); /* k / data */
    t7 = shape_texpr_x_n_next(DIM_XI, 1, intdim, ptrdim); /* h->next */
    t11 = ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_cst_scalar_int(1),
            ap_texpr0_dim(DIM_K), AP_RTYPE_INT, AP_RDIR_RND); /* 1 - k */

    /* do verification */
    sid_4 = shape_assign_texpr_array(ms, true, shp_pre, &xi, &tx, 1, NULL); /* xi = x */

    if (!shape_is_leq(ms, sid_4, shp_inv))
        printf("\nvvvvv init => inv: not satisfied (ERROR)\n");
    else
        printf("\nvvvvv init => inv: satisfied\n");


    sid_5 = shape_meet_tcons_array(ms, false, shp_inv, &whileCond); /* while (xi != NULL) */
    sid_6 = shape_assign_texpr_array(ms, true, sid_5, &xi, &t6, 1, NULL); /* xi->data = k; */
    sid_7 = shape_assign_texpr_array(ms, true, sid_6, &k, &t11, 1, NULL); /* k = 1-k; */
    sid_8 = shape_assign_texpr_array(ms, true, sid_7, &tmp, &t7, 1, NULL); /* tmp = xi->next;  */
    sid_9 = shape_assign_texpr_array(ms, true, sid_8, &xi, &tnull, 1, NULL); /* xi = NULL; */
    sid_10 = shape_assign_texpr_array(ms, true, sid_9, &xi, &ttmp, 1, NULL); /* xi = tmp; */
    sid_11 = shape_assign_texpr_array(ms, true, sid_10, &tmp, &tnull, 1, NULL); /* tmp = null; */


#ifndef NDEBUG
    printf("\nvvvvv post(inv):");
    shape_fdump(stdout, ms, sid_11);
    printf("\n");
#endif

    if (!shape_is_leq(ms, sid_11, shp_inv))
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
    initMod2();

    /* quit */
    if (pr->error_) printf("\n%i error(s)!\n", pr->error_);
    else printf("\nall tests passed\n");
    ap_manager_free(ms);
    return 0;
}
