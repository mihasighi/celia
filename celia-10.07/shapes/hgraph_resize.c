/*
 * hgraph_resize.c
 *
 * Projection, changes of dimension, variable permutation.
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

#include "hgraph.h"
#include "hgraph_internal.h"


/* ============================================================ */
/* Projections */
/* ============================================================ */

/* TODO: priority 0 */
/*
 * not used because we suppose that all ptr variables are declared from the
 * beginning
 */
hgraph_t *
hgraph_forget_array (ap_manager_t * man,
		     bool destructive, hgraph_t * a,
		     ap_dim_t * tdim, size_t size, bool project)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_FORGET_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Change and permutation of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
hgraph_t *
hgraph_add_dimensions (ap_manager_t * man,
		       bool destructive, hgraph_t * a,
		       ap_dimchange_t * dimchange, bool project)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
hgraph_t *
hgraph_remove_dimensions (ap_manager_t * man,
			  bool destructive, hgraph_t * a,
			  ap_dimchange_t * dimchange)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_REMOVE_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
hgraph_t *
hgraph_permute_dimensions (ap_manager_t * man,
			   bool destructive, hgraph_t * a,
			   ap_dimperm_t * permutation)
{
  hgraph_internal_t *pr =
    hgraph_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Expansion and folding of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
hgraph_t *
hgraph_expand (ap_manager_t * man,
	       bool destructive, hgraph_t * a, ap_dim_t dim, size_t n)
{
  hgraph_internal_t *pr = hgraph_init_from_manager (man, AP_FUNID_EXPAND, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
hgraph_t *
hgraph_fold (ap_manager_t * man,
	     bool destructive, hgraph_t * a, ap_dim_t * tdim, size_t size)
{
  hgraph_internal_t *pr = hgraph_init_from_manager (man, AP_FUNID_FOLD, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}
