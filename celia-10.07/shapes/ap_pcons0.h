/*
 * ap_pcons0.h
 *
 * Pointer constraints of level 0.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef AP_PCONS0_H_
#define AP_PCONS0_H_


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
   * Pointer constraints.
   *
   * Constraints allowed are:
   *
   * - pointer constraints x[->next] == y[->next] or
   *                       x[->next] <> y[->next] or
   *                       x ---> y or x --l-> y
   *
   * - data/length constraints = linear constraints with variables with dimension
   * intdim+realdim, i.e., each ptr var (realdim) has an attached data
   */
typedef struct
{
  enum
  {
    DATA_CONS,			/* only constraint on data vars or data
				 * fields */
    EQ_CONS,			/* ptr equality */
    NE_CONS,			/* ptr disequality */
    REACH_CONS,			/* binary reachability */
    REACHL_CONS,		/* reachability with length of the path */
    OTHER_CONS,
  } type;

  union
  {
    struct
    {
      ap_dim_t x, y;		/* in [datadim..datadim+ptrdim-1] */
      size_t nextx, nexty;	/* number of next deferences */
      ap_dim_t l;
    } ptr;
    ap_lincons0_t data;
  } info;

  ap_lincons0_t *lcons;
  ap_tcons0_t *tcons;
  UT_hash_handle hh;		/* make structure hashable keys are
				 * lcons--tcons */
} pcons0_t;

  /* Array of constraints */
typedef struct pcons0_array_t
{
  pcons0_t **p;
  size_t size;
} pcons0_array_t;

void shape_pcons0_clear (pcons0_t * c);
  /* Clear the data constraint in c */
void shape_pcons0_array_clear (pcons0_array_t * array);
  /* Clear the constraints of the array, and then the array itself */

void shape_pcons_fdump (FILE * stream, pcons0_t * c, size_t intdim,
			size_t realdim);
void shape_pcons_array_fdump (FILE * stream, pcons0_array_t * a,
			      size_t intdim, size_t realdim);
  /* Printing */

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* AP_PCONS0_H_ */
