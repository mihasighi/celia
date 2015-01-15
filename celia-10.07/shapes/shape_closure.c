/*
 * shape_closure.c
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

#include "shape.h"
#include "shape_internal.h"


/* All closures are in-place. */


/* ============================================================ */
/* Full Closure */
/* ============================================================ */


/*
 * TODO: priority 1 What it is here closure? - put in the set form - close
 * all the units
 */

bool
shape_close (shape_internal_t * pr, shape_t * a)
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
shape_close_incremental (shape_t * m, size_t dim, size_t v)
{
  return false;
}


/* ============================================================ */
/* Sanity Check */
/* ============================================================ */

/* TODO: priority 1 */
bool
shape_check_closed (shape_t * m, size_t dim)
{
  return false;
}


/* ============================================================ */
/* Topological closure operation */
/* ============================================================ */

/* TODO: priority 3 */
/* Eliminate anonymous nodes */
shape_t *
shape_closure (ap_manager_t * man, bool destructive, shape_t * a)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_CLOSURE, 0);
  if (destructive)
    return a;
  else
    return shape_copy_internal (pr, a);
}
