/**************************************************************************/
/*                                                                        */
/*  This file is part of the SHAD Library.                                */
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

#ifndef SH_PEXPR0_H
#define	SH_PEXPR0_H

#include "ap_dimension.h"
#include "ap_expr0.h"
#include "uthash.h"

#ifdef	__cplusplus
extern "C" {
#endif

    /**
     * Pointer expressions are (for the moment) linear expressions
     * with offsets associated with pointer variables.
     * Since a pointer variable may appear with several offsets, a pointer
     * expression is an array of tuples (ap_linexpr0_t, sh_offset_t*).
     */
    typedef enum sh_offset_kind_t {
        SH_OFFSET_NONE,
        SH_OFFSET_FIELD,
        SH_OFFSET_INDEX,
        SH_OFFSET_CONSTANT,
        SH_OFFSET_LEN, /* for length function */
        SH_OFFSET_SUM, /* for sum function */
        SH_OFFSET_MSET, /* for mset function */
        SH_OFFSET_UCONS, /* for ucons function */
        SH_OFFSET_OTHER /* not to be used */
    } sh_offset_kind_t;

    typedef struct {
        sh_offset_kind_t kind;

        union {
            size_t field;
            ap_dim_t index;
            size_t cst;
        } info;
    } sh_offset_t;

    typedef struct {
        ap_linexpr0_t** expr; /* data/length expression */
        sh_offset_t** offs; /* array of offsets of the same size that the expression */
        size_t size; /* number of expressions, usually 1 */
    } sh_pexpr0_t;

    /* =====================================================================
     * Offsets
     * ===================================================================== */
    void sh_offsets_fprint(FILE * stream, sh_offset_t *offs, size_t size, 
            size_t intdim, size_t ptrdim, char** nameofdim);
    void sh_offsets_fdump(FILE * stream, sh_offset_t *offs, size_t size,
            size_t intdim, size_t ptrdim);
    /* Printing */

    /* =====================================================================
     * Expressions
     * ===================================================================== */
    void sh_pexpr0_clear(sh_pexpr0_t * e);
    /* Clear the expressions in c */

    bool sh_pexpr0_is_empty(sh_pexpr0_t * e);
    /* test is an expression is empty */

    void sh_pexpr0_fprint(FILE * stream, sh_pexpr0_t * c, size_t intdim,
            size_t ptrdim, char** nameofdim);
    void sh_pexpr0_fdump(FILE * stream, sh_pexpr0_t * c, size_t intdim,
            size_t ptrdim);
    /* Printing */

#ifdef __cplusplus
}
#endif

#endif	/* SH_PEXPR0_H */

