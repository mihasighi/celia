/*
 * ap_passign0.h
 *
 * Pointer assignments of level 0.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef AP_PASSIGN0_H_
#define AP_PASSIGN0_H_

#include "ap_dimension.h"
#include "ap_expr0.h"
#include "uthash.h"

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
  enum
  {
    NO_MODIFIER,		/* no attribute to lhs (maybe pointer or
				 * other) */
    DATA_MODIFIER,		/* data attribute to lhs (a ptr) */
    NEXT_MODIFIER,		/* next attribute to lhs (a ptr) */
    OTHER_MODIFIER,
  } type;

  ap_dim_t x;			/* lhs in [0..datadim+ptrdim-1] */
  size_t nextx;			/* number of next/data deferences for x */
  union
  {
    struct
    {
      ap_dim_t y;		/* rhs in [datadim..datadim+ptrdim-1] */
      size_t nexty;		/* number of next deferences for y */
    } ptr;
    ap_linexpr0_t *data;	/* data/length expression */
  } info;

  ap_dim_t lhs;
  ap_linexpr0_t *lexpr;
  ap_texpr0_t *texpr;
  UT_hash_handle hh;		/* make structure hashable keys are
				 * lhs--texpr */
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
