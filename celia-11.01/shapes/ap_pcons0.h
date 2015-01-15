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


#ifndef AP_PCONS0_H_
#define AP_PCONS0_H_


#include "ap_dimension.h"
#include "ap_expr0.h"
#include "uthash.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern "C" {
#endif
    /* *INDENT-ON* */

    /**
     * Pointer constraints.
     *
     * Constraints allowed are:
     * - pointer constraints
     *     acyclic(x)    encoded by x > 0.0
     *     cyclic(x)     encoded by x->next > x
     *     x == NULL     encoded by x(->next) == 0.0
     *     x != NULL     encoded by x(->next) <> 0.0
     *     disjoint(x,y) encoded by x(->next) mod_1 y(->next)
     *     same(x,y)     encoded by x(->next) mod_0 y(->next)
     *     reach(x,y)    encoded by x(->next) >  y(->next)
     * - data/length constraints
     *                   encoded by
     *   a linear constraint with variables with dimension intdim+realdim,
     *                  i.e., each ptr var (realdim) has an attached data
     *  and a vector of offsets representing the function to be applied to ptr
     *  dimensions
     *
     */

/* Offsets shall be negative because dimensions are positive */
    typedef enum {
        OFFSET_NEXT = -1, /* for next field */
        OFFSET_DATA = -2, /* for data field */
        OFFSET_LEN = -3, /* for length function */
        OFFSET_SUM = -4, /* for sum function */
        OFFSET_MSET = -5, /* for mset function */
        OFFSET_OTHER = -6, /* not to be used */
    } offset_t;

    typedef struct {

        enum {
            DATA_CONS, /* only constraint on data vars or data
				 * fields */
            ACYCLIC_CONS, /* unary segment description */
            CYCLIC_CONS,
            ISO_CONS, /* segment isomorphism */
            SEP_CONS, /* segment separation */
            EQ_CONS, /* ptr equality */
            NE_CONS, /* ptr difference */
            REACH_CONS, /* binary reachability */
            OTHER_CONS,
        } type;

        size_t datadim;
        size_t ptrdim;
        union {
            
            struct {
                ap_dim_t x, y; /* in [datadim..datadim+ptrdim-1] or NULL_DIM */
                size_t nextx, nexty; /* number of next dereferences */
                ap_dim_t l;
            } ptr;

            /* TODO: add here ptr dim in order to support all the offsets in a constraint */
            struct {
                ap_lincons0_t cons; /* over dimensions datadim + ptrdim */
                int* offsets; /* over dimension ptrdim with values in
				   offset_t or 0..datadim-1 */
            } data;

        } info;

        ap_lincons0_t *lcons;
        ap_tcons0_t *tcons;
        UT_hash_handle hh; /* make structure hashable */
        /* keys are lcons--tcons */
    } pcons0_t;

/* Array of constraints */
    typedef struct pcons0_array_t {
        pcons0_t **p;
        size_t size;
    } pcons0_array_t;

    void shape_pcons0_clear(pcons0_t * c);
    /* Clear the data constraint in c */
    void shape_pcons0_array_clear(pcons0_array_t * array);
    /* Clear the constraints of the array, and then the array itself */


    void shape_offset_fprint(FILE * stream, int ofs, size_t dim);

    void shape_offsets_fprint(FILE * stream, int* offsets, size_t dim);

    void shape_pcons_fdump(FILE * stream, pcons0_t * c);
    void shape_pcons_array_fdump(FILE * stream, pcons0_array_t * a);
    /* Printing */

    /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* AP_PCONS0_H_ */
