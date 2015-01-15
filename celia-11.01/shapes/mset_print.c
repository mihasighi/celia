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


#include "mset.h"
#include "mset_fun.h"
#include "mset_internal.h"
#include "apron2shape.h"


/* ============================================================ */
/* Printing */
/* ============================================================ */

#ifndef MSET_REL

/* Only first LEN_DIM elements of name_of_dim are good,
 * Then from a->datadim, take the name of nodes. */
void
mset_fprint_lcons(FILE * stream, mset_internal_t * pr, mset_t * a,
        char **name_of_dim) {
    if (!a->lcons)
        fprintf(stream, "\tmset_lcons_%zu [label=\"bot\"] ;\n",
            ushape_number);
    else {
        size_t i;
        size_t size = LEN_DIM(a->datadim, 0) + a->segmdim;
        char **lname = (char **) malloc(size * sizeof (char *));
        for (i = 0; i < LEN_DIM(a->datadim, 0); i++)
            lname[i] = (name_of_dim) ? name_of_dim[i] : shape_name_of_dim[i];
        for (i = 0; i < a->segmdim; i++) {
            size_t size = (4 + strlen(shape_name_of_dim[a->datadim + i]));
            lname[LEN_DIM(a->datadim, 0) + i] =
                    (char *) malloc(size * sizeof (char));
            snprintf(lname[LEN_DIM(a->datadim, 0) + i], size, "l[%s]",
                    shape_name_of_dim[a->datadim + i]);
        }
        fprintf(stream, "\tmset_lcons_%zu [label=<<table><tr><td>lcons: ", ushape_number);
        ap_abstract0_fprint(stream, pr->man_lcons, a->lcons, lname);
        fprintf(stream, "</td></tr></table>> ] ; \n");
        for (i = 0; i < LEN_DIM(a->datadim, 0); i++)
            lname[i] = NULL;
        for (i = 0; i < a->segmdim; i++) {
            free(lname[LEN_DIM(a->datadim, 0) + i]);
            lname[LEN_DIM(a->datadim, 0) + i] = NULL;
        }
        free(lname);
    }
}
#endif

/* Print dcons by giving names to the additional dimensions
 * The first LEN_DIM elements of name_of_dim are not good,
 * Then from a->datadim, take the name of nodes. */
void
mset_fprint_dcons(FILE * stream, mset_internal_t * pr, mset_t * a,
        char **name_of_dim) {
    if (!a->dcons)
        fprintf(stream, "\tmset_dcons_%zu [label=\"bot\"] ;\n", ushape_number);
    else {
        char **dname;
        size_t i, size;
#ifdef MSET_REL
        size = (a->datadim + 2 * a->segmdim);
        dname = (char **) malloc(size * sizeof (char *));
        for (i = 0; i < a->datadim; i++)
            dname[i] = (name_of_dim) ? name_of_dim[i] : shape_name_of_dim(i);
        for (i = 0; i < a->segmdim; i++) {
            char *name = shape_name_of_dim(a->datadim + i);
            size_t lsize = (7 + strlen(name));
            dname[a->datadim + i] =
                    (char *) malloc(lsize * sizeof (char));
            snprintf(dname[a->datadim + i], lsize, "data(%s)", name);
            dname[a->datadim + a->segmdim + i] =
                    (char *) malloc(lsize * sizeof (char));
            snprintf(dname[a->datadim + a->segmdim + i], lsize,
                    "len(%s)", name);
        }
#else
        size = (DATA_DIM(a->datadim, 0) + a->segmdim);
        dname = (char **) malloc(size * sizeof (char *));
        for (i = 0; i < DATA_DIM(a->datadim, 0); i++)
            dname[i] =
                (name_of_dim) ? name_of_dim[LEN_DIM(a->datadim, 0) +
            i] : shape_name_of_dim(LEN_DIM(a->
                datadim,
                0) + i);
        for (i = 0; i < a->segmdim; i++) {
            char* name = shape_name_of_dim[a->datadim + i];
            size_t lsize = (7 + strlen(name));
            dname[DATA_DIM(a->datadim, 0) + i] =
                    (char *) malloc(lsize * sizeof (char));
            snprintf(dname[DATA_DIM(a->datadim, 0) + i], lsize, "data(%s)", name);
        }
#endif
        fprintf(stream, "\tmset_dcons_%zu [label=<<table><tr><td>dcons: ", ushape_number);
        ap_abstract0_fprint(stream, pr->man_dcons, a->dcons, dname);
        fprintf(stream, "</td></tr></table>> ] ;\n");
        memset(dname, 0, size * sizeof (char*));
        free(dname);
    }
}

/* Print multiset cons by giving names to the additional dimensions
 * The first LEN_DIM elements of name_of_dim are not good,
 * Then from a->datadim, take the name of nodes. */
void
mset_fprint_mscons(FILE * stream, mset_internal_t * pr, mset_t * a,
        char **name_of_dim) {
    if (!a->mscons)
        fprintf(stream, "\tmset_mscons_%zu [label=\"bot\"] ;\n", ushape_number);
    else {
        char **dname;
        size_t i, size;
        size = (DATA_DIM(a->datadim, 0) + 2 * a->segmdim);
        dname = (char **) malloc(size * sizeof (char *));
#ifndef NDEBUG1
        //        printf("\nMSET size = %zu, DATA_DIM=%zu, LEN_DIM=%zu\n",
        //                size, DATA_DIM(a->datadim,0), LEN_DIM(a->datadim,0));
#endif
        for (i = 0; i < DATA_DIM(a->datadim, 0); i++)
            dname[i] =
                (name_of_dim) ? name_of_dim[LEN_DIM(a->datadim, 0) +
            i] : shape_name_of_dim(LEN_DIM(a->
                datadim,
                0) + i);
        for (i = 0; i < a->segmdim; i++) {
            char* name = shape_name_of_dim(a->datadim + i);
            size_t lsize = (7 + strlen(name));
            dname[DATA_DIM(a->datadim, 0) + i] =
                    (char *) malloc(lsize * sizeof (char));
            snprintf(dname[DATA_DIM(a->datadim, 0) + i], lsize, "mshd(%s)", name);
            dname[DATA_DIM(a->datadim, 0) + a->segmdim + i] =
                    (char *) malloc(lsize * sizeof (char));
            snprintf(dname[DATA_DIM(a->datadim, 0) + a->segmdim + i], lsize, "mstl(%s)",
                    name);
        }
        fprintf(stream, "\tmset_mscons_%zu [label=<<table><tr><td>mscons: ", ushape_number);
        ap_abstract0_fprint(stream, pr->man_mscons, a->mscons, dname);
        fprintf(stream, "</td></tr></table>> ] ;\n");
        memset(dname, 0, size * sizeof (char*));
        free(dname);
    }
}

void
mset_fprint(FILE * stream, ap_manager_t * man, mset_t * a,
        char **name_of_dim) {
    mset_internal_t *pr = mset_init_from_manager(man, AP_FUNID_FPRINT, 0);
    fprintf(stream, "\n\tsubgraph cluster_mset_%zu {\n\tnode [shape=record] ;\n",
            ushape_number);
    if (!a)
        fprintf(stream, "\tlabel=\"mset %zu EMPTY\" ;\n}\n", ushape_number);
    else {
        fprintf(stream, "\tlabel=\"mset %zu of (datadim=%zu, segmdim=%zu)\" ;\n",
                ushape_number,
                a->datadim,
                a->segmdim);
        if (!name_of_dim) shape_init_name_of_dim(a->datadim, a->segmdim);
#ifndef MSET_REL
        mset_fprint_lcons(stream, pr, a, name_of_dim);
#endif
        mset_fprint_dcons(stream, pr, a, name_of_dim);
        mset_fprint_mscons(stream, pr, a, name_of_dim);
        fprintf(stream, "\t}\n");
    }
}

void
mset_fprintdiff(FILE * stream, ap_manager_t * man,
        mset_t * a1, mset_t * a2, char **name_of_dim) {
    mset_internal_t *pr = mset_init_from_manager(man, AP_FUNID_FPRINTDIFF, 0);
    fprintf(stream, "mset1 (size %zu) and mset2 (size %zu)\n",
            a1->segmdim, a2->segmdim);
    /* TODO: priority 1 */
}

void
mset_fdump(FILE * stream, ap_manager_t * man, mset_t * a) {
    mset_fprint(stream, man, a, NULL);
}


/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* TODO: priority 0 */

/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
mset_serialize_raw(ap_manager_t * man, mset_t * a) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_SERIALIZE_RAW, 0);
    ap_membuf_t buf;
    buf.size = 0;
    buf.ptr = NULL;
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return buf;
}

/* Do the building of a value from a code!
 * The ptr is an array of 3 size_t [datadim, segmdim, codeop]
 */
mset_t *
mset_deserialize_raw(ap_manager_t * man, void *ptr, size_t * size) {
    mset_internal_t *pr =
            mset_init_from_manager(man, AP_FUNID_DESERIALIZE_RAW, 0);

    size_t *mset_raw = (size_t *) ptr;
    size_t datadim = mset_raw[0];
    size_t segmdim = mset_raw[1];
    size_t code = mset_raw[2];
    ap_dim_t l = 0;
    ap_dim_t S = 2; // TODO: take care of collapse with the sum domain
    ap_dim_t T = 3;
    size_t nodex = 1;
    size_t nodey = 2;
    size_t nodez = 3;
    mset_t *r = mset_top(man, datadim, segmdim);

    /* common constraints l[x]==_l>=1 and M[x]==S */
    if (code != 1) {
        assert(segmdim >= 2);
        ap_lincons0_array_t arr = ap_lincons0_array_make(3);
        arr.p[0] =
                shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_LEN, 1,
                datadim + nodex, 0, 0, -1, l, 0, datadim,
                segmdim);
        arr.p[1] =
                shape_lincons_x_y_v_cst(AP_CONS_SUPEQ, OFFSET_LEN, 1,
                datadim + nodex, 0, 0, 0, 0, -1, datadim,
                segmdim);
        arr.p[2] =
                shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_MSET, 1, datadim + nodex,
                0, 0, -1, S, 0, datadim, segmdim);
        r = mset_meet_lincons_array(man, true, r, &arr);
        ap_lincons0_array_clear(&arr);
    }
    /* common constraint l[x]==l[y] and l[y]>=1 */
    if (code == 2 || code == 4) {
        assert(segmdim >= 3);
        ap_lincons0_array_t arr = ap_lincons0_array_make(2);
        arr.p[0] =
                shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_LEN, 1,
                datadim + nodex, -1, datadim + nodey, 0, 0, 0, datadim,
                segmdim);
        arr.p[1] =
                shape_lincons_x_y_v_cst(AP_CONS_SUPEQ, OFFSET_LEN, 1,
                datadim + nodey, 0, 0, 0, 0, -1, datadim,
                segmdim);
        r = mset_meet_lincons_array(man, true, r, &arr);
        ap_lincons0_array_clear(&arr);
    }
    /* specific constraints */
    switch (code) {
        case 1: /* l[x]+l[y]==_l and l[x]>=1 and l[y]>=1 and M[x]+M[y]==S */
        {
            assert(segmdim >= 2);
            ap_lincons0_array_t arr = ap_lincons0_array_make(4);
            arr.p[0] =
                    shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_LEN, 1,
                    datadim + nodex, 1, datadim + nodey, -1, l,
                    0, datadim, segmdim);
            arr.p[1] =
                    shape_lincons_x_y_v_cst(AP_CONS_SUPEQ, OFFSET_LEN, 1,
                    datadim + nodex, 0, 0, 0, 0, -1, datadim,
                    segmdim);
            arr.p[2] =
                    shape_lincons_x_y_v_cst(AP_CONS_SUPEQ, OFFSET_LEN, 1,
                    datadim + nodey, 0, 0, 0, 0, -1, datadim,
                    segmdim);
            arr.p[3] =
                    shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_MSET, 1,
                    datadim + nodex, 1, datadim + nodey, -1, S, 0, datadim,
                    segmdim);
            r = mset_meet_lincons_array(man, true, r, &arr);
            ap_lincons0_array_clear(&arr);
            break;
        }
        case 3: /* non-equal length lists: l[y]+1<=_l and l[y]>=1 */
        {
            assert(segmdim >= 3);
            ap_lincons0_array_t arr = ap_lincons0_array_make(2);
            arr.p[0] =
                    shape_lincons_x_y_v_cst(AP_CONS_SUPEQ, OFFSET_LEN, -1,
                    datadim + nodey, 0, 0, 1, l, -1, datadim,
                    segmdim);
            arr.p[1] =
                    shape_lincons_x_y_v_cst(AP_CONS_SUPEQ, OFFSET_LEN, 1,
                    datadim + nodey, 0, 0, 0, 0, -1, datadim,
                    segmdim);
            r = mset_meet_lincons_array(man, true, r, &arr);
            ap_lincons0_array_clear(&arr);
            break;
        }
        case 4: /* equal length lists: l[z]=l[x] and M[y]=T */
        {
            assert(segmdim >= 4 && datadim >= 4);
            ap_lincons0_array_t arr = ap_lincons0_array_make(2);
            arr.p[0] =
                    shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_LEN, -1,
                    datadim + nodex, 1, datadim + nodez, 0, 0, 0, datadim,
                    segmdim);
            arr.p[1] =
                    shape_lincons_x_y_v_cst(AP_CONS_EQ, OFFSET_MSET, 1,
                    datadim + nodey, 0, 0, -1, T, 0, datadim,
                    segmdim);
            r = mset_meet_lincons_array(man, true, r, &arr);
            ap_lincons0_array_clear(&arr);
            break;
        }
        default:
            break;
    }
    return r;
}
