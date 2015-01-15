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


#include "ap_passign0.h"
#include "ap_linexpr0.h"

/* To use only when htable of passigns is freed */
void
shape_passign0_clear(passign0_t * c) {
    if (c->type == DATA_MODIFIER) {
        /* TODO: also if assigned vars is data prg var */
        ap_linexpr0_free(c->info.data.expr);
        if (c->info.data.offsets) free(c->info.data.offsets);
    }
}

void
shape_passign0_array_clear(passign0_array_t * array) {
    size_t i;

    if (array->p != NULL) {
        for (i = 0; i < array->size; i++)
            if (array->p[i] != NULL) {
                //Warning:all passign0_t values are hashed, so not to be freed !
                //shape_passign0_clear(array->p[i]);
                //free(array->p[i]);
                array->p[i] = NULL;
            }
        free(array->p);
        array->p = NULL;
    }
}

void
shape_passign_fdump(FILE * stream, passign0_t * a,
        size_t intdim, size_t realdim) {
    size_t i;
    if (!a)
        fprintf(stream, "[NULL ptr assignement]");
    else {
        if (a->type == DATA_MODIFIER)
            shape_offset_fprint(stream, a->nextx, intdim, a->x);
        else {
            fprintf(stream, "x%u", a->x);

            if (a->type == NEXT_MODIFIER) {
                if (a->nextx != 1)
                    fprintf(stream, "Internal Error");
                else
                    fprintf(stream, "->next");
            }
        }
        fprintf(stream, " := ");
        if ((a->type == NO_MODIFIER && a->x < (unsigned int) intdim)
                || a->type == DATA_MODIFIER) {
            ap_linexpr0_fprint(stream, a->info.data.expr, NULL);
            shape_offsets_fprint(stream, a->info.data.offsets, intdim, realdim);
        } else if (a->op == PA_ALLOC || a->op == PA_ALLOC_N) {
            fprintf(stream, "new");
        } else if (a->op == PA_FREE) {
            fprintf(stream, "free");
        } else {
            fprintf(stream, "x%u", a->info.ptr.y);
            for (i = 0; i < a->info.ptr.nexty; i++)
                fprintf(stream, "->next");
        }
    }
    fflush(stream);
}

void
shape_passign_array_fdump(FILE * stream, passign0_array_t * array,
        size_t intdim, size_t realdim) {
    if (!array || array->size == 0 || array->p == NULL)
        fprintf(stream, "[empty array]");
    else {
        size_t i;
        fprintf(stream, "[");
        for (i = 0; i < array->size; i++) {
            shape_passign_fdump(stream, array->p[i], intdim, realdim);
            if (i % 4 == 3)
                fprintf(stream, ",\n\t");
            else
                fprintf(stream, ", ");

        }
        fprintf(stream, "]");
    }
    fflush(stream);

}
