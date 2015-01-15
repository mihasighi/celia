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


#ifndef AP_PASSIGN0_H_
#define AP_PASSIGN0_H_

#include "ap_dimension.h"
#include "ap_expr0.h"
#include "uthash.h"
#include "ap_pcons0.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

  /**
   * Internal representation of assignments. Assignments allowed are:
   *
   * - pointer x[->next]* = y[->next]*   [but usually * = 0 or 1] [before
   * assigning a pointer, first is put to null]
   *
   * - pointer data x->data = expr with normal variables and y->data
   * variables
   *
   * - data and length variables v = expr with normal variables and
   * y->data variables
   */
typedef struct
{
  enum                         /* kind of assignment */
  {
    PA_ASSIGN = 0,
    PA_ALLOC,
    PA_ALLOC_N,
    PA_FREE,
    PA_OTHER
  } op;

  /* left hand side */
  enum
  {
    NO_MODIFIER = 0,		/* no attribute to lhs (maybe pointer or
				 * other) */
    DATA_MODIFIER,		/* data attribute to lhs (a ptr) */
    NEXT_MODIFIER,		/* next attribute to lhs (a ptr) */
    OTHER_MODIFIER
  } type;

  ap_dim_t x;			/* lhs in [0..datadim+ptrdim-1] */
  int nextx;			/* number of next dereferences for x */
                                /* or the offset information for data modifier */

  /* right hand side */
  union
  {
    struct
    {
      ap_dim_t y;		/* rhs in [datadim..datadim+ptrdim-1] or NULL_DIM */
      size_t nexty;		/* number of next dereferences for y */
    } ptr;

    struct
    {
      ap_linexpr0_t *expr;	/* data/length expression */
      int* offsets;
    } data;

    struct
    {
      bool cst;                 /* alloc with a constant */
      union
      {
        size_t   c;
        ap_dim_t l;
      } size;
    } alloc;

  } info;

  ap_dim_t lhs;
  ap_linexpr0_t *lexpr;
  ap_texpr0_t *texpr;
  UT_hash_handle hh;		/* make structure hashable */
					        /* keys are lhs--texpr */
} passign0_t;

  /* Array of assignments */
typedef struct passign0_array_t
{
  passign0_t **p;
  size_t size;
} passign0_array_t;


void shape_passign0_clear (passign0_t * c);
  /* Clear the data constraint in c */
void shape_passign0_array_clear (passign0_array_t * array);
  /* Clear the constraints of the array, and then the array itself */

void shape_passign_fdump (FILE * stream, passign0_t * c, size_t intdim,
			  size_t realdim);
void shape_passign_array_fdump (FILE * stream, passign0_array_t * a,
				size_t intdim, size_t realdim);
  /* Printing */

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* AP_PASSIGN0_H_ */
