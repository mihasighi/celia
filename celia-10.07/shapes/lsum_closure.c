/*
 * lsum_closure.c
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

#include "lsum.h"
#include "lsum_internal.h"


/* All closures are in-place. */


/* ============================================================ */
/* Full Closure */
/* ============================================================ */


/*
 * TODO: priority 1 What it is here closure? - put in the set form - close
 * all the units
 */

bool
lsum_close (lsum_t * g, size_t dim)
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
lsum_close_incremental (lsum_t * m, size_t dim, size_t v)
{
  return false;
}


/* ============================================================ */
/* Sanity Check */
/* ============================================================ */

/* TODO: priority 1 */
bool
lsum_check_closed (lsum_t * m, size_t dim)
{
  return false;
}


/* ============================================================ */
/* Topological closure operation */
/* ============================================================ */

lsum_t *
lsum_closure (ap_manager_t * man, bool destructive, lsum_t * a)
{
  lsum_internal_t *pr = lsum_init_from_manager (man, AP_FUNID_CLOSURE, 0);
  if (!a)
    return NULL;
  lsum_t *r = lsum_alloc_internal (pr, a->datadim, a->segmdim);
  r->dcons = ap_abstract0_closure (pr->man_dcons, false, a->dcons);
#ifndef LSUM_REL
  r->lcons = ap_abstract0_closure (pr->man_lcons, false, a->lcons);
#endif
  if (destructive)
    lsum_free_internal (pr, a);
  return r;
}
