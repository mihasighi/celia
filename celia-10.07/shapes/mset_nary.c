/*
 * mset_nary.c
 *
 * N-ary multiset functions: meet, join, widening, narrowing & related.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 *
 */

/* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution.
 */

#include "mset.h"
#include "mset_internal.h"
#include "shape_macros.h"
#include "ap_generic.h"
#include "ap_abstract0.h"

/* ============================================================ */
/* Meet and Join */
/* ============================================================ */

mset_t *
mset_meet (ap_manager_t * man, bool destructive, mset_t * a1, mset_t * a2)
{
  if (mset_is_bottom (man, a1) || mset_is_bottom (man, a2))
    return NULL;
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_MEET, 0);
  arg_assert (a1->datadim == a2->datadim
	      && a1->segmdim == a2->segmdim, return NULL;);
  mset_t *r = mset_alloc_internal (pr, a1->datadim, a1->segmdim);
  r->mscons =
    ap_abstract0_meet (pr->man_mscons, destructive, a1->mscons, a2->mscons);
  r->dcons =
    ap_abstract0_meet (pr->man_dcons, destructive, a1->dcons, a2->dcons);
#ifndef MSET_REL
  r->lcons =
    ap_abstract0_meet (pr->man_lcons, destructive, a1->lcons, a2->lcons);
#endif
  // TODO: put some normalization/saturation to obtain equalities
  if (destructive)
    {
      a1->mscons = a2->mscons = NULL;
      a1->dcons = a2->dcons = NULL;
#ifndef MSET_REL
      a1->lcons = a2->lcons = NULL;
#endif
      free (a1);
      free (a2);
    }
  return r;
}

mset_t *
mset_join (ap_manager_t * man, bool destructive, mset_t * a1, mset_t * a2)
{
  if (mset_is_bottom (man, a1) || mset_is_bottom (man, a2))
    return NULL;
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_JOIN, 0);
  arg_assert (a1->datadim == a2->datadim
	      && a1->segmdim == a2->segmdim, return NULL;);
  mset_t *r = mset_alloc_internal (pr, a1->datadim, a1->segmdim);
  r->mscons =
    ap_abstract0_join (pr->man_mscons, destructive, a1->mscons, a2->mscons);
  r->dcons =
    ap_abstract0_join (pr->man_dcons, destructive, a1->dcons, a2->dcons);
#ifndef MSET_REL
  r->lcons =
    ap_abstract0_join (pr->man_lcons, destructive, a1->lcons, a2->lcons);
  // TODO: put some normalization/saturation to obtain equalities
#endif
  if (destructive)
    {
      a1->mscons = a2->mscons = NULL;
      a1->dcons = a2->dcons = NULL;
#ifndef MSET_REL
      a1->lcons = a2->lcons = NULL;
#endif
      free (a1);
      free (a2);
    }
  return r;
}

mset_t *
mset_meet_array (ap_manager_t * man, mset_t ** tab, size_t size)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_MEET_ARRAY, 0);
  arg_assert (size > 0, return NULL;);
  mset_t *r = mset_copy_internal (pr, tab[0]);
  size_t i;
  for (i = 1; i < size && !mset_is_bottom (man, r); i++)
    {
      mset_t *rr = mset_meet (man, false, tab[i], r);
      mset_free (man, r);
      r = rr;
    }
  return r;
}

mset_t *
mset_join_array (ap_manager_t * man, mset_t ** tab, size_t size)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_JOIN_ARRAY, 0);
  arg_assert (size > 0, return NULL;);
  mset_t *r = mset_copy_internal (pr, tab[0]);
  size_t i;
  for (i = 1; i < size; i++)
    {
      mset_t *rr = mset_join (man, false, tab[i], r);
      mset_free (man, r);
      r = rr;
    }
  return r;
}


/* ============================================================ */
/* Widening, Narrowing */
/* ============================================================ */

mset_t *
mset_widening (ap_manager_t * man, mset_t * a1, mset_t * a2)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_WIDENING, 0);
  arg_assert (a1 && a2 && a1->datadim == a2->datadim
	      && a1->segmdim == a2->segmdim, return NULL;);
  mset_t *r = mset_alloc_internal (pr, a1->datadim, a1->segmdim);
  r->mscons = ap_abstract0_widening (pr->man_mscons, a1->mscons, a2->mscons);
  r->dcons = ap_abstract0_widening (pr->man_dcons, a1->dcons, a2->dcons);
#ifndef MSET_REL
  r->lcons = ap_abstract0_widening (pr->man_lcons, a1->lcons, a2->lcons);
#endif
  // TODO: put some normalization/saturation to obtain equalities
  return r;
}

mset_t *
mset_widening_threshold (ap_manager_t * man,
			 mset_t * a1, mset_t * a2,
			 ap_lincons0_array_t * array)
{
  mset_internal_t *pr =
    mset_init_from_manager (man, AP_FUNID_WIDENING, array->size + 1);
  arg_assert (a1 && a2 && a1->datadim == a2->datadim
	      && a1->segmdim == a2->segmdim, return NULL;);
  mset_t *r = mset_alloc_internal (pr, a1->datadim, a1->segmdim);
  r->mscons = ap_abstract0_widening_threshold (pr->man_mscons, a1->mscons, a2->mscons,array);
  r->dcons =
    ap_abstract0_widening_threshold (pr->man_dcons, a1->dcons, a2->dcons,
				     array);
#ifndef MSET_REL
  /* TODO: not correct, select constraints depending on lengths/data */
  r->lcons =
    ap_abstract0_widening_threshold (pr->man_lcons, a1->lcons, a2->lcons,
				     array);
#endif
  // TODO: put some normalization/saturation to obtain equalities
  return r;
}

/* NOT IMPLEMENTED */
mset_t *
mset_narrowing (ap_manager_t * man, mset_t * a1, mset_t * a2)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_WIDENING, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a2;
}
