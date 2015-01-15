/*
 * mset_closure.c
 *
 * Powerset domain of constrained shapes graphs - Closure algorithms
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 *
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include "mset.h"
#include "mset_internal.h"


/* All closures are in-place. */


/* ============================================================ */
/* Full Closure */
/* ============================================================ */


/*
 * TODO: priority 1 What it is here closure? - put in the set form - close
 * all the units
 */

bool
mset_close (mset_t * g, size_t dim)
{
  return false;
}


/* ============================================================ */
/* Incremental Closure */
/* ============================================================ */

/*
 * TODO: priority 1
 *
 * ?? time. ?? space.
 */

bool
mset_close_incremental (mset_t * m, size_t dim, size_t v)
{
  return false;
}


/* ============================================================ */
/* Sanity Check */
/* ============================================================ */

/* TODO: priority 1 */
bool
mset_check_closed (mset_t * m, size_t dim)
{
  return false;
}


/* ============================================================ */
/* Topological closure operation */
/* ============================================================ */

mset_t *
mset_closure (ap_manager_t * man, bool destructive, mset_t * a)
{
  mset_internal_t *pr = mset_init_from_manager (man, AP_FUNID_CLOSURE, 0);
  if (!a)
    return NULL;
  mset_t *r = mset_alloc_internal (pr, a->datadim, a->segmdim);
  r->dcons = ap_abstract0_closure (pr->man_dcons, false, a->dcons);
#ifndef MSET_REL
  r->lcons = ap_abstract0_closure (pr->man_lcons, false, a->lcons);
#endif
  r->mscons = ap_abstract0_closure (pr->man_mscons, false, a->mscons);
  if (destructive)
    mset_free_internal (pr, a);
  return r;
}
