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
/* Program for the verification of intlist-map2-add2_eq.c */
/*********************************************************/

/* ********************* */
/* Globals vars          */
/* ********************* */
ap_manager_t* ms; /* shape manager */
shape_internal_t* pr; /* internal manager */

size_t ptrdim = 5; /* vars tmp, x, xi, y, yi */
size_t intdim = 0; /* vars  */
char* names_dim[5] = {"tmp", "x", "xi", "y", "yi"};


#define DIM_TMP 0
#define DIM_X 1
#define DIM_XI 2
#define DIM_Y 3
#define DIM_YI 4



/* ********************** */
/* Target program         */

/* ********************** */

void add2_eq(void) {

    /* values computed for each sid */
    shape_t *top;
    shape_t *shp_pre; /* acyclic(x) && acyclic(y) && len(x)=len(y)>=1 && xi = yi = tmp = NULL */
    shape_t *sid_3; /* xi = x */
    shape_t *sid_4; /* yi = y */
    shape_t *sid_5; /* while (xi != NULL) */
    shape_t *sid_6; /* yi->data = xi->data + 2; */
    shape_t *sid_7; /* tmp = xi->next; */
    shape_t *sid_8; /* xi = NULL */
    shape_t *sid_9; /* xi = tmp; */
    shape_t *sid_10; /* tmp = NULL; */
    shape_t *sid_11; /* tmp = yi->next; */
    shape_t *sid_12; /* yi = NULL; */
    shape_t *sid_13; /* yi = tmp; */
    shape_t *sid_14; /* tmp = NULL; */
    shape_t *shp_inv, *shp_inv1, *shp_inv2, *shp_inv3;
    ap_tcons0_array_t pre, whileCond, notWhileCond, inv1, inv2, inv3;
    ap_texpr0_t* tnull, *tx, *ty, *ttmp, *t6, *t7, *t11;
    int algo;
    ap_dim_t tmp, xi, yi; // for assignments

    printf("\nvvvvvvvvvv add2_eq (ok expected) vvvvvvvvvv\n");

    top = shape_top(ms, intdim, ptrdim);
#ifndef NDEBUG
    printf("\nvvvvv top:");
    shape_fdump(stdout, ms, top);
    printf("\n");
#endif

    /* init parameters: 0 2 P_= */
    algo = pr->max_anon | (pr->segm_anon << 4) | (2 << 8);
    shape_approximate(ms, top, algo);

    /*
     * init assertions
     */
    /* requires: ls(n1,#) * ls(n2,#) /\ x(n1) /\ y(n2) /\ \len(n1)=\len(n2)>=1 */
    pre = ap_tcons0_array_make(3);
    // ls(n1,#) /\ x(n1) /\ \len(n1) >= 1
    pre.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n2,#) /\ y(n2) /\ \len(n2) >= 1
    pre.p[1] = shape_tcons_reach_x_y(DIM_Y, 0, NULL_DIM, 0, intdim, ptrdim);
    // \len(n1) = \len(n2)
    pre.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // len(x)
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_Y), AP_RTYPE_QUAD, AP_RDIR_UP), // len(y)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_pre = shape_meet_tcons_array(ms, false, top, &pre);
    // the ptr variables not constrained BY THE ARRAY are set to #
#ifndef NDEBUG
    printf("\nvvvvv pre:");
    shape_fdump(stdout, ms, shp_pre);
    printf("\n");
#endif

    /* inv1: pre /\ xi = x /\ yi = y  */
    inv1 = ap_tcons0_array_make(5);
    // ls(n1,#) /\ x(n1) /\ \len(n1) >= 1
    inv1.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n2,#) /\ y(n2) /\ \len(n2) >= 1
    inv1.p[1] = shape_tcons_reach_x_y(DIM_Y, 0, NULL_DIM, 0, intdim, ptrdim);
    // \len(n1) = \len(n2)
    inv1.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // len(x)
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_Y), AP_RTYPE_QUAD, AP_RDIR_UP), // len(y)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // x == xi
    inv1.p[3] = shape_tcons_x_y(AP_CONS_EQ, DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    // y == yi
    inv1.p[4] = shape_tcons_x_y(AP_CONS_EQ, DIM_Y, 0, DIM_YI, 0, intdim, ptrdim);
    shp_inv1 = shape_meet_tcons_array(ms, false, top, &inv1);

    /* inv2: ls(n1,n1i) * ls(n1i,#) * ls(n2,n2i) * ls(n2i,#) /\
     *       x(n1) /\ xi(n1i) /\ y(n2) /\ yi(n2i) /\ tmp(#)
     *       \len(n2) = \len(n1) >= 1 /\ \len(n2i) = \len(n1i) >= 1 /\
     *       forall y1, y2. 1<=y1<\len(n1) /\ 1<=y2<\len(n2) /\ y1=y2 ==> \data(y1)=\data(y2)+2
     */
    inv2 = ap_tcons0_array_make(7);
    // ls(n1,n1i) /\ x(n1) /\ xi(n1i) /\ \len(n1)>=1
    inv2.p[0] = shape_tcons_reach_x_y(DIM_X, 0, DIM_XI, 0, intdim, ptrdim);
    // ls(n2,n2i) /\ y(n2) /\ yi(n2i) /\ \len(n2)>=1
    inv2.p[1] = shape_tcons_reach_x_y(DIM_Y, 0, DIM_YI, 0, intdim, ptrdim);
    // ls(n1i,#) /\ xi(n1i) /\ \len(n1i)>=1
    inv2.p[2] = shape_tcons_reach_x_y(DIM_XI, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n2i,#) /\ yi(n2i) /\ \len(n2i)>=1
    inv2.p[3] = shape_tcons_reach_x_y(DIM_YI, 0, NULL_DIM, 0, intdim, ptrdim);
   // \len(n1) = \len(n2)
    inv2.p[4] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // len(x)
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_Y), AP_RTYPE_QUAD, AP_RDIR_UP), // len(y)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \len(n1i) = \len(n2i)
    inv2.p[5] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_XI), AP_RTYPE_QUAD, AP_RDIR_UP), // len(xi)
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_YI), AP_RTYPE_QUAD, AP_RDIR_UP), // len(yi)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \ucons(n1) + \ucons(n2) + 1 = 0
    inv2.p[6] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(x)
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_Y), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(y)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv2 = shape_meet_tcons_array(ms, false, top, &inv2);

    /* inv3: ls(n1,#) * ls(n2,#) /\ x(n1) /\ xi(#) /\ y(n2) /\ yi(#) /\ tmp(#)
     *       \len(n2) = \len(n1) >= 1 /\
     *       forall y1, y2. 1<=y1<\len(n1) /\ 1<=y2<\len(n2) /\ y1=y2 ==> \data(y1)=\data(y2)+2
     */
    inv3 = ap_tcons0_array_make(4);
    // ls(n1,#) /\ x(n1) /\ \len(n1) >= 1
    inv3.p[0] = shape_tcons_reach_x_y(DIM_X, 0, NULL_DIM, 0, intdim, ptrdim);
    // ls(n2,#) /\ y(n2) /\ \len(n2) >= 1
    inv3.p[1] = shape_tcons_reach_x_y(DIM_Y, 0, NULL_DIM, 0, intdim, ptrdim);
    // \len(n1) = \len(n2)
    inv3.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_UP), // len(x)
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_Y), AP_RTYPE_QUAD, AP_RDIR_UP), // len(y)
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \ucons(n1) + \ucons(n2) + 1 = 0
    inv3.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_X), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(x)
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_Y), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(y)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
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
    xi = DIM_XI;
    yi = DIM_YI;
    tmp = DIM_TMP;
    tnull = shape_texpr_null(intdim, ptrdim); /* NULL */
    ttmp = shape_texpr_x_n_next(DIM_TMP, 0, intdim, ptrdim); /* tmp */
    tx = shape_texpr_x_n_next(DIM_X, 0, intdim, ptrdim); /* x */
    ty = shape_texpr_x_n_next(DIM_Y, 0, intdim, ptrdim); /* y */
    whileCond = ap_tcons0_array_make(1);
    whileCond.p[0] = shape_tcons_diff_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    notWhileCond = ap_tcons0_array_make(1);
    notWhileCond.p[0] = shape_tcons_same_x_y(DIM_XI, false, NULL_DIM, false, intdim, ptrdim);
    t6 = shape_texpr_deref_data(
            ap_texpr0_binop(AP_TEXPR_ADD,
            shape_texpr_x_data(DIM_XI, intdim, ptrdim),
            ap_texpr0_cst_scalar_int(2), AP_RTYPE_INT, AP_RDIR_RND)); /* xi->data+2 / data */
    t7 = shape_texpr_x_n_next(DIM_XI, 1, intdim, ptrdim); /* xi->next */
    t11 = shape_texpr_x_n_next(DIM_YI, 1, intdim, ptrdim); /* yi->next */
    
    /* do verification */
    sid_3 = shape_assign_texpr_array(ms, true, shp_pre, &xi, &tx, 1, NULL); /* x = xi */
    sid_4 = shape_assign_texpr_array(ms, true, sid_3, &yi, &ty, 1, NULL); /* y = yi */

    if (!shape_is_leq(ms, sid_4, shp_inv))
        printf("\nvvvvv init => inv: not satisfied (ERROR)\n");
    else
        printf("\nvvvvv init => inv: satisfied\n");


    sid_5 = shape_meet_tcons_array(ms, false, shp_inv, &whileCond); /* while (xi != NULL) */
    sid_6 = shape_assign_texpr_array(ms, true, sid_5, &yi, &t6, 1, NULL); /* yi->data = xi->data + 2; */
    sid_7 = shape_assign_texpr_array(ms, true, sid_6, &tmp, &t7, 1, NULL); /* tmp = xi->next;  */
    sid_8 = shape_assign_texpr_array(ms, true, sid_7, &xi, &tnull, 1, NULL); /* xi = NULL; */
    sid_9 = shape_assign_texpr_array(ms, true, sid_8, &xi, &ttmp, 1, NULL); /* xi = tmp; */
    sid_10 = shape_assign_texpr_array(ms, true, sid_9, &tmp, &tnull, 1, NULL); /* tmp = NULL; */
    sid_11 = shape_assign_texpr_array(ms, true, sid_10, &tmp, &t11, 1, NULL); /* tmp = yi->next; */
    sid_12 = shape_assign_texpr_array(ms, true, sid_11, &yi, &tnull, 1, NULL); /* yi = NULL; */
    sid_13 = shape_assign_texpr_array(ms, true, sid_12, &yi, &ttmp, 1, NULL); /* yi = tmp; */
    sid_14 = shape_assign_texpr_array(ms, true, sid_13, &tmp, &tnull, 1, NULL); /* tmp = NULL; */

#ifndef NDEBUG
    printf("\nvvvvv post(inv):");
    shape_fdump(stdout, ms, sid_14);
    printf("\n");
#endif

    if (!shape_is_leq(ms, sid_14, shp_inv))
        printf("\nvvvvv post(inv) => inv: not satisfied (ERROR)\n");
    else
        printf("\nvvvvv post(inv) => inv: satisfied\n");

    /* check also post? */

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
    add2_eq();

    /* quit */
    if (pr->error_) printf("\n%i error(s)!\n", pr->error_);
    else printf("\nall tests passed\n");
    ap_manager_free(ms);
    return 0;
}
