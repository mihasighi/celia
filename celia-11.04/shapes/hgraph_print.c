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


#include "hgraph.h"
#include "hgraph_internal.h"
#include "apron2shape.h"


/* ============================================================ */
/* Printing */
/* ============================================================ */

/* Get more informations about the htable */
void
hgraph_fdump_internal(FILE * stream, hgraph_internal_t * pr, hgraph_t * a) {
    /* print some hash informations */
    fprintf(stream, "\n\tHGraphs table of size %d\n",
            HASH_CNT(hh, pr->hgraphs));
    hgraph_fdump(stream, pr->man, a);
}

inline void
hgraph_fprint_dot(FILE * stream, ap_manager_t * man,
        hgraph_t * a, char **name_of_dim, char *name) {
    size_t i, j;
    /* print graph name */
    fprintf(stream, "digraph %s {\n", name);
    /*
     * print cut nodes (in boxed) with label information, print edges for
     * each node
     */
    if (!name_of_dim) shape_init_name_of_dim(a->datadim, a->ptrdim);
    for (i = 0; i < a->size; i++) {
        fprintf(stream, "n%zu -> n%zu;\n", i, NODE_NEXT(a, i));
        if (NODE_VAR_NEXT(a, i) == 0 && NODE_VAR(a, i) < a->ptrdim) {
            fprintf(stream, "n%zu [shape=box, label=\"", i);
            /* print all labels */
            for (j = 0; j < a->ptrdim; j++)
                if (VAR2NODE(a, j) == i) {
                    if (name_of_dim)
                        fprintf(stream, "p%s ", name_of_dim[a->datadim + j]);
                    else
                        fprintf(stream, "x%zu ", a->datadim + j);
 	        }
            fprintf(stream, "\"]\n");
        }
    }
    fprintf(stream, "}\n");
}

void
hgraph_fprint_reach(FILE * stream, ap_manager_t * man,
        hgraph_t * a, char **name_of_dim, char *name) {
    size_t i, j;
    unsigned int bs;

    fprintf(stream, " of size %zu, ptrdim %zu, datadim %zu, %sclosed\" ;\n",
            a->size, a->ptrdim, a->datadim, (a->closed) ? " " : "not ");

    if (!a->info) {
        fprintf(stream, "\t/* empty info */\n");
        return;
    }

    /* collect variable labeling */
    unsigned int * ptrvar = (unsigned int *) malloc(a->size * sizeof (unsigned int));
    memset(ptrvar, 0, a->size * sizeof (unsigned int));
    for (i = 0; i < a->ptrdim; i++) {
        size_t n = VAR2NODE(a, i);
#ifndef NDEBUG2
        fprintf (stream, "\t/* x%zu(n%zu) */\n", i+a->datadim, n);
#endif
        if (n < a->size)
            ptrvar[n] = ptrvar[n] | (1 << i);
    }

    /* print all nodes and their labels */
    fprintf(stream, "\t/* nodes and their labels */\n");

    for (i = 0; i < a->size; i++) {
        size_t v = NODE_VAR(a,i);
        fprintf(stream, "\th_%zu_n%zu [label=\" n%zu%s | ",
                ushape_number, i, i, (i == 0) ? "(#)" : "");
        if (i==0)
            fprintf(stream, " NULL | [ ");
        else if (v>=a->ptrdim)
            fprintf(stream, " TOP | [ ");
        else if (name_of_dim)
            fprintf(stream, " %s^(%zu) | [ ",
                name_of_dim[a->datadim + v],
                NODE_VAR_NEXT(a, i));
        else
            fprintf(stream, " x%zu^(%zu) | [ ",
                a->datadim + v, NODE_VAR_NEXT(a, i));
        for (j = 0, bs = ptrvar[i]; bs && j < a->ptrdim; j++) {
            if (bs & 0x1) {
                if (name_of_dim)
                    fprintf(stream, "p%s ", name_of_dim[a->datadim + j]);
                else
                    fprintf(stream, "x%zu ", a->datadim + j);
            }
            bs = bs >> 1;
        }
        fprintf(stream, "]\" ] ;\n");
    }

    /* print successor matrix */
    fprintf(stream, "\t/* succ matrix */\n");
    for (i = 0; i < a->size; i++)
        fprintf(stream, "\th_%zu_n%zu -> h_%zu_n%zu;\n",
            ushape_number, i, ushape_number, NODE_NEXT(a, i));
    
}

void
hgraph_fprint(FILE * stream, ap_manager_t * man,
        hgraph_t * a, char **name_of_dim) {
    hgraph_internal_t *pr = hgraph_init_from_manager(man, AP_FUNID_FPRINT, 0);
    fprintf(stream, "\tnode [shape=Mrecord] ;\n");
    fprintf(stream, "\tlabel=\"hgraph %zu", ushape_number);
    if (hgraph_is_bottom(man, a))
        fprintf(stream, " bottom\" ;\n");
    else if (hgraph_is_top(man, a))
        fprintf(stream, " top\" ;\n");
    else
        hgraph_fprint_reach(stream, man, a, name_of_dim, "hgraph");
}

void
hgraph_fprintdiff(FILE * stream, ap_manager_t * man,
        hgraph_t * a1, hgraph_t * a2, char **name_of_dim) {
    hgraph_internal_t *pr =
            hgraph_init_from_manager(man, AP_FUNID_FPRINTDIFF, 0);
    hgraph_fprint(stream, man, a1, name_of_dim);
    hgraph_fprint(stream, man, a2, name_of_dim);
}

void
hgraph_fdump(FILE * stream, ap_manager_t * man, hgraph_t * a) {
    fprintf(stream, "\tnode [shape=Mrecord] ;\n");
    fprintf(stream, "\tlabel=\"hgraph %zu", ushape_number);
    hgraph_fprint_reach(stream, man, a, NULL, "hgraph");
}

void
hgraph_array_fdump(FILE * stream, ap_manager_t * man, hgraph_array_t * a) {
    if (!a)
        fprintf(stream, "EMPTY ARRAY");
    else {
        size_t i;
        fprintf(stream, "[");
        for (i = 0; i < a->size; i++) {
            fprintf(stream, "\n******%zu******\n", i);
            hgraph_fdump(stream, man, a->p[i]);
        }
        fprintf(stream, "\n]\n");
    }
}

/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* TODO: priority 0 */

/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
hgraph_serialize_raw(ap_manager_t * man, hgraph_t * a) {
    hgraph_internal_t *pr =
            hgraph_init_from_manager(man, AP_FUNID_SERIALIZE_RAW, 0);
    ap_membuf_t buf;
    buf.size = 0;
    buf.ptr = NULL;
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return buf;
}

/* TODO: priority 0 */

/* NOT IMPLEMENTED: do nothing */
hgraph_t *
hgraph_deserialize_raw(ap_manager_t * man, void *ptr, size_t * size) {
    hgraph_internal_t *pr =
            hgraph_init_from_manager(man, AP_FUNID_DESERIALIZE_RAW, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return NULL;
}
