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


#include "shape.h"
#include "shape_internal.h"


/* ============================================================ */
/* Printing */

/* ============================================================ */


FILE *
shape_fdump_new_file(FILE * stream, shape_internal_t * pr) {
    FILE *r = NULL;
    char filename[20];
    memset(filename, 0, 20 * sizeof (char));
    snprintf(filename, 20, "f_%ld.shp", pr->filenum++);
    r = fopen(filename, "w");
    assert(r);
#ifndef NDEBUG1
    fprintf (stream, "<file:%s>\n", filename);
#endif
    return r;
}

void
shape_fprint(FILE * stream, ap_manager_t * man, shape_t * a,
        char **name_of_dim) {
    shape_internal_t *pr = shape_init_from_manager(man, AP_FUNID_FPRINT, 0);
    size_t i;
    FILE *shape_stream = shape_fdump_new_file(stream, pr);
    fprintf(shape_stream, "digraph shape {\n");
    fprintf(shape_stream, "\tgraph [rankdir=\"LR\"];\n");
    if (!a) {
        fprintf(shape_stream, "label=\"shape EMPTY\";\n}\n");
        return;
    }
    fprintf(shape_stream, "\tlabel=\"shape size %zu ",
            a->msize);
    if (name_of_dim) {
        fprintf(shape_stream, " over variables rev[");
        for (i = a->intdim + a->realdim; i > 0; i--)
            fprintf(shape_stream, "%s ", name_of_dim[i]);
        fprintf(shape_stream, "]");
    }
    fprintf(shape_stream, "\" ;\n");
    for (i = 0; i < a->msize; i++) {
        fprintf(shape_stream, "\nsubgraph cluster_%zd {\n", i);
        ushape_number = i;
        ushape_fprint(shape_stream, man, a->m.p[i], name_of_dim);
        fprintf(shape_stream, "}\n");
    }
    ushape_number = 0;
    fprintf(shape_stream, "}\n");
    fflush(shape_stream);
    fclose(shape_stream);
    fflush(stream);
}

void
shape_fprintdiff(FILE * stream, ap_manager_t * man,
        shape_t * a1, shape_t * a2, char **name_of_dim) {
    shape_internal_t *pr =
            shape_init_from_manager(man, AP_FUNID_FPRINTDIFF, 0);
    fprintf(stream, "shape1 (size %zu) and shape2 (%zu)\n",
            a1->msize, a2->msize);
    shape_fprint(stream, man, a1, name_of_dim);
    fprintf(stream, "\n<------>\n");
    shape_fprint(stream, man, a2, name_of_dim);
    fflush(stream);
}

void
shape_fdump(FILE * stream, ap_manager_t * man, shape_t * a) {
    shape_fprint(stream, man, a, NULL);
}


/* ============================================================ */
/* Serialization */
/* ============================================================ */

/* TODO: priority 0 */

/* NOT IMPLEMENTED: do nothing */
ap_membuf_t
shape_serialize_raw(ap_manager_t * man, shape_t * a) {
    shape_internal_t *pr =
            shape_init_from_manager(man, AP_FUNID_SERIALIZE_RAW, 0);
    ap_membuf_t buf;
    buf.size = 0;
    buf.ptr = NULL;
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return buf;
}

/* TODO: priority 0 */

/* NOT IMPLEMENTED: do nothing */
shape_t *
shape_deserialize_raw(ap_manager_t * man, void *ptr, size_t * size) {
    shape_internal_t *pr =
            shape_init_from_manager(man, AP_FUNID_DESERIALIZE_RAW, 0);
    ap_manager_raise_exception(man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
            "not implemented");
    return NULL;
}
