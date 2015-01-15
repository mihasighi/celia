/*
 * shape_resize.c
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

#include "shape.h"
#include "shape_internal.h"


/* ============================================================ */
/* Projections */
/* ============================================================ */

/* TODO: priority 0 */
/*
 * not used because we suppose that all ptr variables are declared from the
 * beginning
 */
shape_t *
shape_forget_array (ap_manager_t * man,
		    bool destructive, shape_t * a,
		    ap_dim_t * tdim, size_t size, bool project)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_FORGET_ARRAY, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Change and permutation of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
shape_t *
shape_add_dimensions (ap_manager_t * man,
		      bool destructive, shape_t * a,
		      ap_dimchange_t * dimchange, bool project)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
shape_t *
shape_remove_dimensions (ap_manager_t * man,
			 bool destructive, shape_t * a,
			 ap_dimchange_t * dimchange)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_REMOVE_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
shape_t *
shape_permute_dimensions (ap_manager_t * man,
			  bool destructive, shape_t * a,
			  ap_dimperm_t * permutation)
{
  shape_internal_t *pr =
    shape_init_from_manager (man, AP_FUNID_ADD_DIMENSIONS, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}


/* ============================================================ */
/* Expansion and folding of dimensions */
/* ============================================================ */

/* TODO: priority 0 */
shape_t *
shape_expand (ap_manager_t * man,
	      bool destructive, shape_t * a, ap_dim_t dim, size_t n)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_EXPAND, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}

/* TODO: priority 0 */
shape_t *
shape_fold (ap_manager_t * man,
	    bool destructive, shape_t * a, ap_dim_t * tdim, size_t size)
{
  shape_internal_t *pr = shape_init_from_manager (man, AP_FUNID_FOLD, 0);
  ap_manager_raise_exception (man, AP_EXC_NOT_IMPLEMENTED, pr->funid,
			      "not implemented");
  return a;
}
