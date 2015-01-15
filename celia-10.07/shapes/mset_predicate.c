/*
 * mset_predicate.c
 *
 * Predicates on multisets, extraction functions
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "mset.h"
#include "mset_internal.h"
#include "ap_generic.h"
#include "apron2shape.h"



/* ============================================================ */
/* Tests */
/* ============================================================ */

/* The bottom value is bot on one constraint. */
bool
mset_is_bottom (ap_manager_t * man, mset_t * a)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_IS_BOTTOM, 0);
  if (!a)
    return true;
  bool is_bot = ap_abstract0_is_bottom (pr->man_mscons, a->mscons);
  is_bot = is_bot || ap_abstract0_is_bottom (pr->man_dcons, a->dcons);
#ifndef MSET_REL
  is_bot = is_bot || ap_abstract0_is_bottom (pr->man_lcons, a->lcons);
#endif
  return is_bot;
}

/* The top value is top in all constraints. */
bool
mset_is_top (ap_manager_t * man, mset_t * a)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_IS_TOP, 0);
  if (!a)
    return false;
  bool is_top = ap_abstract0_is_top (pr->man_mscons, a->mscons);
  is_top = is_top && ap_abstract0_is_top (pr->man_dcons, a->dcons);
#ifndef MSET_REL
  is_top = is_top && ap_abstract0_is_top (pr->man_lcons, a->lcons);
#endif
  return is_top;
}

bool
mset_is_leq (ap_manager_t * man, mset_t * a1, mset_t * a2)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_IS_LEQ, 0);
  if (mset_is_bottom (man, a1))
    return true;
  if (mset_is_bottom (man, a2))
    return false;
  if (a1->datadim != a2->datadim || a1->segmdim != a2->segmdim)
    return false;
  bool is_leq = ap_abstract0_is_leq (pr->man_mscons, a1->mscons, a2->mscons);
  is_leq = is_leq
    && ap_abstract0_is_leq (pr->man_dcons, a1->dcons, a2->dcons);
#ifndef MSET_REL
  is_leq = is_leq
    && ap_abstract0_is_leq (pr->man_lcons, a1->lcons, a2->lcons);
#endif
  return is_leq;
}

bool
mset_is_eq (ap_manager_t * man, mset_t * a1, mset_t * a2)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_IS_EQ, 0);
  if ((mset_is_bottom (man, a1) && mset_is_bottom (man, a2)) ||
      (mset_is_top (man, a1) && mset_is_top (man, a2)))
    return true;
  if (a1->datadim != a2->datadim || a1->segmdim != a2->segmdim)
    return false;
  bool is_eq = ap_abstract0_is_eq (pr->man_mscons, a1->mscons, a2->mscons);
  is_eq = is_eq && ap_abstract0_is_eq (pr->man_dcons, a1->dcons, a2->dcons);
#ifndef MSET_REL
  is_eq = is_eq && ap_abstract0_is_eq (pr->man_lcons, a1->lcons, a2->lcons);
#endif
  return is_eq;
}

/* TODO: priority 1
 * Checking linear constraints between
 * program variables (pointer or scalar) in the assert statements.
 */
bool
mset_sat_lincons (ap_manager_t * man, mset_t * a, ap_lincons0_t * lincons)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_SAT_LINCONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return true;
}

/* TODO: priority 1
 * Checking constraints between expressions on pointer
 * program variables in the assert statements.
 */
bool
mset_sat_tcons (ap_manager_t * man, mset_t * a, ap_tcons0_t * cons)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_SAT_TCONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return true;
}

/* TODO: priority 1 */
/* Interval constraints are only between non-pointer variables */
bool
mset_sat_interval (ap_manager_t * man, mset_t * a,
		   ap_dim_t dim, ap_interval_t * i)
{
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_SAT_INTERVAL, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return true;
}

bool
mset_is_dimension_unconstrained (ap_manager_t * man, mset_t * a, ap_dim_t dim)
{
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_IS_DIMENSION_UNCONSTRAINED, 0);
  if (!a)
    return false;
  if (mset_is_top (man, a))
    return true;
  bool is_uncons = true;
  if (dim < LEN_DIM(a->datadim,0))
    { // length program variables are constrained by lcons or dcons
#ifdef MSET_REL
      is_uncons = is_uncons
        && ap_abstract0_is_dimension_unconstrained (pr->man_dcons, a->dcons, dim);
#else
      is_uncons = is_uncons
        && ap_abstract0_is_dimension_unconstrained (pr->man_lcons, a->lcons, dim);
#endif
    }
  else if (dim < a->datadim)
    { // data program variables are constrained by dcons and mscons
#ifdef MSET_REL
      is_uncons = is_uncons
          && ap_abstract0_is_dimension_unconstrained (pr->man_dcons, a->dcons, dim);
#else
      is_uncons = is_uncons
        && ap_abstract0_is_dimension_unconstrained (pr->man_dcons, a->dcons, dim-LEN_DIM(a->datadim,0));
#endif
      is_uncons = is_uncons
          && ap_abstract0_is_dimension_unconstrained (pr->man_mscons, a->mscons, dim-LEN_DIM(a->datadim,0));
    } else { // a->datadim <= dim < a->datadim+a->segmdim
      // segments are constrained by length, data and multiset constraints
#ifdef MSET_REL
      // data of the segment
      is_uncons = is_uncons
      && ap_abstract0_is_dimension_unconstrained (pr->man_dcons, a->dcons, dim);
      // length of the segment
      is_uncons = is_uncons
      && ap_abstract0_is_dimension_unconstrained (pr->man_dcons, a->dcons, a->segmdim+dim);
#else
      // data of the segment
      is_uncons = is_uncons
      && ap_abstract0_is_dimension_unconstrained (pr->man_dcons, a->dcons, dim-LEN_DIM(a->datadim,0));
      // length of the segment
      is_uncons = is_uncons
      && ap_abstract0_is_dimension_unconstrained (pr->man_lcons, a->lcons, dim - DATA_DIM(a->datadim,0));
#endif
      // multiset of the segment
      is_uncons = is_uncons
      && ap_abstract0_is_dimension_unconstrained (pr->man_mscons, a->mscons, a->segmdim+dim-DATA_DIM(a->datadim,0));
   }

  return is_uncons;
}

/* ============================================================ */
/* Extraction of properties */
/* ============================================================ */

/* NOT IMPLEMENTED */
ap_interval_t *
mset_bound_linexpr (ap_manager_t * man, mset_t * a, ap_linexpr0_t * expr)
{
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_BOUND_LINEXPR, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}

/* NOT IMPLEMENTED */
ap_interval_t *
mset_bound_texpr (ap_manager_t * man, mset_t * a, ap_texpr0_t * expr)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_BOUND_TEXPR, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}

/* NOT IMPLEMENTED */
ap_interval_t *
mset_bound_dimension (ap_manager_t * man, mset_t * a, ap_dim_t dim)
{
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_BOUND_DIMENSION, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}

/* NOT IMPLEMENTED */
ap_lincons0_array_t
mset_to_lincons_array (ap_manager_t * man, mset_t * a)
{
  ap_lincons0_array_t ar;
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_TO_LINCONS_ARRAY, 0);
  ar = ap_lincons0_array_make (1);
  ar.p[0] = ap_lincons0_make_unsat ();
  return ar;
}

/* NOT IMPLEMENTED */
ap_tcons0_array_t
mset_to_tcons_array (ap_manager_t * man, mset_t * a)
{
  return ap_generic_to_tcons_array (man, a);
}

/* NOT IMPLEMENTED */
ap_interval_t **
mset_to_box (ap_manager_t * man, mset_t * a)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_TO_BOX, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return NULL;
}

/* NOT IMPLEMENTED */
ap_generator0_array_t
mset_to_generator_array (ap_manager_t * man, mset_t * a)
{
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_TO_GENERATOR_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return ap_generator0_array_make (0);
}
