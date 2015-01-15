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



#include "ap_pcons0.h"

/* To use only when htable of pcons is freed */
void
shape_pcons0_clear(pcons0_t * c) {
    if (c->type == DATA_CONS) {
        ap_lincons0_clear(&c->info.data.cons);
        if (c->info.data.offsets) free(c->info.data.offsets);
    }
}

void
shape_pcons0_array_clear(pcons0_array_t * array) {
    size_t i;

    if (array->p != NULL) {
        for (i = 0; i < array->size; i++)
            if (array->p[i] != NULL) {
                //Warning:all pcons0_t values are hashed, so not to be freed !
                //shape_pcons0_clear(array->p[i]);
                //free(array->p[i]);
                array->p[i] = NULL;
            }
        free(array->p);
        array->p = NULL;
    }
}

void
shape_offset_fprint(FILE * stream, int ofs, size_t datadim, size_t dim) {
    size_t d = dim+datadim;
    switch (ofs) {
        case OFFSET_NEXT: fprintf(stream, " \\next(x%zu) ", d);
            break;
        case OFFSET_DATA: fprintf(stream, " \\data(x%zu) ", d);
            break;
        case OFFSET_LEN: fprintf(stream, " \\length(x%zu) ", d);
            break;
        case OFFSET_SUM: fprintf(stream, " \\sum(x%zu) ", d);
            break;
        case OFFSET_MSET: fprintf(stream, " \\mset(x%zu) ", d);
            break;
        case OFFSET_UCONS: fprintf(stream, " \\ucons(x%zu) ", d);
            break;
        default:
            if (ofs >= 0)
                fprintf(stream, " x%zu[x%d] ", d, ofs);
            else
                fprintf(stream, " x%zu ", d);
    }
}

void
shape_offsets_fprint(FILE * stream, int* offsets, size_t datadim, size_t realdim) {
#ifndef NDEBUG
    fprintf(stdout,"\n====shape_offsets_fprint: dim=(%d)\n", realdim);
#endif
    if (offsets == NULL)
        fprintf(stream, " [NULL offsets]\n");
    else {
        size_t i;
        fprintf(stream, " offsets = [");
        for (i = 0; i < realdim; i++)
            shape_offset_fprint(stream, offsets[i], datadim, i);
        fprintf(stream, "]\n");
    }
}

void
shape_pcons_fdump(FILE * stream, pcons0_t * c) {
    if (!c)
        fprintf(stream, "[NULL ptr constraint]\n");
    else {
#ifndef NDEBUG
        fprintf(stdout, "\n====shape_pcons_fdump: dim=(%zu,%zu)\n",c->datadim,c->ptrdim);
        fflush(stdout);
#endif
        if (c->type == DATA_CONS) {
            ap_lincons0_fprint(stream, &c->info.data.cons, NULL);
            shape_offsets_fprint(stream, c->info.data.offsets, c->datadim, c->ptrdim);
        } else {
            size_t i;
            fprintf(stream, "x%u", c->info.ptr.x);
            for (i = 0; i < c->info.ptr.nextx; i++)
                fprintf(stream, "->next");
            switch (c->type) {
                case EQ_CONS:
                    fprintf(stream, " == ");
                    break;
                case NE_CONS:
                    fprintf(stream, " != ");
                    break;
                case REACH_CONS:
                    fprintf(stream, " --> ");
                    break;
                case ISO_CONS:
                    fprintf(stream, " iso ");
                    break;
                case SEP_CONS:
                    fprintf(stream, " * ");
                    break;
                default:
                    fprintf(stream, " Internal Error ");
                    break;
            }
            fprintf(stream, "x%u", c->info.ptr.y);
            for (i = 0; i < c->info.ptr.nexty; i++)
                fprintf(stream, "->next");
        }
    }
    fflush(stream);
}

void
shape_pcons_array_fdump(FILE * stream, pcons0_array_t * array) {
    if (!array || array->size == 0 || array->p == NULL)
        fprintf(stream, "[empty array]");
    else {
        size_t i;
        fprintf(stream, "[ ");
        for (i = 0; i < array->size; i++) {
            shape_pcons_fdump(stream, array->p[i]);
            if (i % 4 == 3)
                fprintf(stream, ",\n\t");
            else
                fprintf(stream, ", ");
        }
        fprintf(stream, "]");
    }
    fflush(stream);

}
