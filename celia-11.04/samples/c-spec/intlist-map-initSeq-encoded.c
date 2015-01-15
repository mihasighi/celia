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
/* Program for the verification of intlist-map-initSeq.c */
/*********************************************************/

/* ********************* */
/* Globals vars          */
/* ********************* */
ap_manager_t* ms; /* shape manager */
shape_internal_t* pr; /* internal manager */

size_t ptrdim = 3; /* vars h, head, tmp */
size_t intdim = 1; /* vars v */
char* names_dim[4] = {"v", "h", "head", "tmp"};

#define DIM_V 0
#define DIM_H 1
#define DIM_HEAD 2
#define DIM_TMP 3


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

void initSeq(void) {

    /* values computed for each sid */
    shape_t *top;
    shape_t *shp_pre; /* acyclic(head) && len(head)>=1 && h(#) && tmp(#) && v == 0 */
    shape_t *sid_4; /* h = head */
    shape_t *sid_5; /* while (h != NULL) */
    shape_t *sid_6; /* h->data = v; */
    shape_t *sid_7; /* v = v + 1 */
    shape_t *sid_8; /* tmp = h->next; */
    shape_t *sid_9; /* h = NULL; */
    shape_t *sid_10; /* h = tmp; */
    shape_t *sid_11; /* tmp = NULL; */
    // shape_t *sid_12; /* if (h==NULL) */
    shape_t *shp_inv, *shp_inv1, *shp_inv2, *shp_inv3;
    ap_tcons0_array_t pre, whileCond, notWhileCond, inv1, inv2, inv3;
    ap_texpr0_t* t4, *t6, *t7, *t8, *t9, *t10, *t11;
    int algo;
    ap_dim_t v, h, head, tmp; // for assignments

    printf("\nvvvvvvvvvv initSeq (ok expected) vvvvvvvvvv\n");

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
    /* requires: ls(n,#) /\ head(n) /\ \len(n)>=1 /\ v == 0 */
    pre = ap_tcons0_array_make(2);
    // ls(n,#) /\ head(n) /\ \len(n1) >= 1
    pre.p[0] = shape_tcons_reach_x_y(DIM_HEAD, 0, NULL_DIM, 0, intdim, ptrdim);
    // v = 0
    pre.p[1] = ap_tcons0_make(AP_CONS_EQ, ap_texpr0_dim(DIM_V), NULL);
    shp_pre = shape_meet_tcons_array(ms, false, top, &pre);
    // the ptr variables not constrained BY THE ARRAY are set to #

    printf("\nvvvvv pre:");
    shape_fdump(stdout, ms, shp_pre);
    printf("\n");

    /* inv1: ls(n,#) /\ head(n) /\ h(n) /\ v == 0 */
    inv1 = ap_tcons0_array_make(3);
    // ls(n,#) /\ head(n)
    inv1.p[0] = shape_tcons_reach_x_y(DIM_HEAD, 0, NULL_DIM, 0, intdim, ptrdim);
    // h == head
    inv1.p[1] = shape_tcons_x_y(AP_CONS_EQ, DIM_HEAD, 0, DIM_H, 0, intdim, ptrdim);
    // v = 0
    inv1.p[2] = ap_tcons0_make(AP_CONS_EQ, ap_texpr0_dim(DIM_V), NULL);
    shp_inv1 = shape_meet_tcons_array(ms, false, top, &inv1);

    /* inv2: ls(n2,n1) * ls(n1,#) /\ head(n2) /\ h(n1) /\ tmp(#)
     *       \len(n2) == v /\ \data(n2) == 0 /\
     *       forall y. 1<= y < \len(n2) ==> n2[y] == y
     */
    inv2 = ap_tcons0_array_make(5);
    // ls(n2,n1) /\ head(n2) /\ h(n1)
    inv2.p[0] = shape_tcons_reach_x_y(DIM_HEAD, 0, DIM_H, 0, intdim, ptrdim);
    // ls(n1,#)
    inv2.p[1] = shape_tcons_reach_x_y(DIM_H, 0, NULL_DIM, 0, intdim, ptrdim);
    // \len(n2) - v = 0
    inv2.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_HEAD), AP_RTYPE_QUAD, AP_RDIR_UP), // len(head)
            ap_texpr0_dim(DIM_V),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \data(n2) = 0
    inv2.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_HEAD), AP_RTYPE_INT, AP_RDIR_NEAREST), // \data(n2)
            NULL);
    // forall y. 1<= y < \len(n2) ==> n2[y] == y
    // \ucons(n2) + 1 = 0
    inv2.p[4] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_HEAD), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n2)
            ap_texpr0_cst_scalar_int(1),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    shp_inv2 = shape_meet_tcons_array(ms, false, top, &inv2);

    /* inv3: ls(n,#) /\ head(n) /\ h(#) /\
     *       \len(n1) == v /\ \data(n1) == 0 /\
     *       \forall y. 1<= y < \len(n) ==> n[y] == y
     */
    inv3 = ap_tcons0_array_make(4);
    // ls(n,#) /\ head(n) 
    inv3.p[0] = shape_tcons_reach_x_y(DIM_HEAD, 0, NULL_DIM, 0, intdim, ptrdim);
    // \len(n) - v = 0
    inv3.p[1] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_SUB,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_HEAD), AP_RTYPE_QUAD, AP_RDIR_UP), // \len(n)
            ap_texpr0_dim(DIM_V),
            AP_RTYPE_INT, AP_RDIR_NEAREST),
            NULL);
    // \data(n) = 0
    inv3.p[2] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_HEAD), AP_RTYPE_INT, AP_RDIR_NEAREST), // \data(n)
            NULL);
    // \forall y. 1<= y < \len(n2) ==> n2[y] == y
    // \ucons(n) + 1 = 0
    inv3.p[3] = ap_tcons0_make(AP_CONS_EQ,
            ap_texpr0_binop(AP_TEXPR_ADD,
            ap_texpr0_unop(AP_TEXPR_CAST, ap_texpr0_dim(DIM_HEAD), AP_RTYPE_QUAD, AP_RDIR_DOWN), // \ucons(n)
            ap_texpr0_cst_scalar_int(1),
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
    v = DIM_V;
    h = DIM_H;
    head = DIM_HEAD;
    tmp = DIM_TMP;
    t4 = shape_texpr_x_n_next(DIM_HEAD, 0, intdim, ptrdim); /* head */
    whileCond = ap_tcons0_array_make(1);
    whileCond.p[0] = shape_tcons_diff_x_y(DIM_H, false, NULL_DIM, false, intdim, ptrdim);
    notWhileCond = ap_tcons0_array_make(1);
    notWhileCond.p[0] = shape_tcons_same_x_y(DIM_H, false, NULL_DIM, false, intdim, ptrdim);
    t6 = shape_texpr_deref_data(ap_texpr0_dim(DIM_V)); /* v / data */
    t7 = ap_texpr0_binop(AP_TEXPR_ADD, ap_texpr0_dim(DIM_V),
            ap_texpr0_cst_scalar_int(1), AP_RTYPE_INT, AP_RDIR_RND); /* v + 1 */
    t8 = shape_texpr_x_n_next(DIM_H, 1, intdim, ptrdim); /* h->next */
    t9 = shape_texpr_null(intdim, ptrdim); /* NULL */
    t10 = shape_texpr_x_n_next(DIM_TMP, 0, intdim, ptrdim); /* tmp */
    t11 = t9; /* NULL */

    /* do verification */
    sid_4 = shape_assign_texpr_array(ms, true, shp_pre, &h, &t4, 1, NULL); /* h = head */

    if (!shape_is_leq(ms, sid_4, shp_inv))
        printf("\nvvvvv init => inv: not satisfied (ERROR)\n");
    else
        printf("\nvvvvv init => inv: satisfied\n");


    sid_5 = shape_meet_tcons_array(ms, false, shp_inv, &whileCond); /* while (h != NULL) */
    sid_6 = shape_assign_texpr_array(ms, true, sid_5, &h, &t6, 1, NULL); /* h->data = v; */
    sid_7 = shape_assign_texpr_array(ms, true, sid_6, &v, &t7, 1, NULL); /* v = v + 1;  */
    sid_8 = shape_assign_texpr_array(ms, true, sid_7, &tmp, &t8, 1, NULL); /* tmp = h->next; */
    sid_9 = shape_assign_texpr_array(ms, true, sid_8, &h, &t9, 1, NULL); /* h = null; */
    sid_10 = shape_assign_texpr_array(ms, true, sid_9, &h, &t10, 1, NULL); /* h = tmp; */
    sid_11 = shape_assign_texpr_array(ms, true, sid_10, &tmp, &t11, 1, NULL); /* tmp = null; */


    printf("\nvvvvv post(inv):");
    shape_fdump(stdout, ms, sid_11);
    printf("\n");


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
    initSeq();

    /* quit */
    if (pr->error_) printf("\n%i error(s)!\n", pr->error_);
    else printf("\nall tests passed\n");
    ap_manager_free(ms);
    return 0;
}
