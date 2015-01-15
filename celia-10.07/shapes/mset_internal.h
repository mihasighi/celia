/*
 * mset_internal.h
 *
 * Private definitions, access to internal structures and algorithms.
 * Use with care.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef __MSET_INTERNAL_H_
#define __MSET_INTERNAL_H_

#include "shape_options.h"
#include "mset_fun.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

  /* ********************************************************************** */
  /* Manager */
  /* ********************************************************************** */

  /* ============================================================ */
  /* Internal Representation */
  /* ============================================================ */

  /* manager-local data specific to universal constraints */
struct _mset_internal_t
{
  /* current function */
  ap_funid_t funid;

  /* local parameters for current function */
  ap_funopt_t *funopt;

  /* manager of the length domain */
  ap_manager_t *man_lcons;

  /* manager of the data domain */
  ap_manager_t *man_dcons;

  /* manager of the linear equations domain */
  ap_manager_t *man_mscons;

  /* count errors */
  int error_;

  /* back-pointer */
  ap_manager_t *man;
};


  /* ============================================================ */
  /* Basic management. */
  /* ============================================================ */

  /* options for setting basic abstract domains */
/* For the moment, the abstract domains used are:
 * - if relational then lcons: null, dcons: polyhedra
 * - if non relational then lcons: octagons or polyhedra, dcons: polyhedra
 * In any case, mscons is given by the polyhedra with equalities domain (polkaeq).
 */

  /* called by each function to setup and get manager-local data */
static inline mset_internal_t *
mset_init_from_manager (ap_manager_t * man, ap_funid_t id, size_t size)
{
  mset_internal_t *pr = (mset_internal_t *) man->internal;
  pr->funid = id;
  pr->funopt = man->option.funopt + id;
  man->result.flag_exact = man->result.flag_best = true;

  return pr;
}


  /* ********************************************************************** */
  /* List multiset constraints */
  /* ********************************************************************** */

  /* ============================================================ */
  /* Internal Representation */
  /* ============================================================ */

  /** List multiset constraints maintain relations between the multisets
   * of elements of list segments. For this, it also represents constraints
   * on data variables, length variables, and data of first nodes in
   * segments.
   * So we have a product domain on lcons /\ dcons /\ mscons
   */

struct _mset_t
{
  size_t datadim;		/* number of scalar data variables (length and data) */
  size_t segmdim;		/* number of segments represented */
#ifndef MSET_REL
  void *lcons;			/* constraint on lengths of dimension
				 * LEN_DIM + segmdim */
#endif
  void *dcons;			/* constraint on data of dimension
				 * DATA_DIM + 1 segmdim ifndef MSET_REL
				 * datadim + 2 segmdim (data then length) ifdef MSET_REL */
  void *mscons;                 /* constraint on mumtisets
                                 * DATA_DIM + 2 segmdim (data then multiset) */
};


/* ============================================================ */
/* Internal Management */
/* ============================================================ */

mset_t *mset_alloc_internal (mset_internal_t * pr, size_t intdim,
			     size_t realdim);
mset_t *mset_alloc_top (mset_internal_t * pr, size_t intdim, size_t realdim);
void mset_free_internal (mset_internal_t * pr, mset_t * a);
mset_t *mset_copy_internal (mset_internal_t * pr, mset_t * a);


  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */




#endif /* __MSET_INTERNAL_H_ */
