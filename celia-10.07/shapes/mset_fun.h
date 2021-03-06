/*
 * mset_fun.h
 *
 * Direct access to high level functions bypassing the manager.
 * The only file you need to include to use the multiset domain.
 * The complete semantics of these functions is given in ap_abstract0.h
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

#ifndef __MSET_FUN_H
#define __MSET_FUN_H


/* dependencies */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#define NUMFLT_PRINT_PREC ap_scalar_print_prec

#include "ap_dimension.h"
#include "ap_expr0.h"
#include "ap_manager.h"
#include "mset.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */



  /* ============================================================ */
  /* I.1 Memory */
  /* ============================================================ */

struct _mset_t;
typedef struct _mset_t mset_t;
  /* Abstract data type of msets (defined in mset_internal.h). */

struct _mset_internal_t;
typedef struct _mset_internal_t mset_internal_t;
  /* Abstract data type of library-specific manager options. */

mset_t *mset_copy (ap_manager_t * man, mset_t * a);
  /*
   * Return a copy of an abstract value, on which destructive update
   * does not affect the initial value.
   */

void mset_free (ap_manager_t * man, mset_t * a);
  /* Free all the memory used by the abstract value */

size_t mset_size (ap_manager_t * man, mset_t * a);
  /* Return the abstract size of an abstract value (see ap_manager_t) */


mset_t *mset_of_abstract0 (ap_abstract0_t * a);
ap_abstract0_t *abstract0_of_mset (ap_manager_t * man, mset_t * oct);
  /* Wrapping / unwrapping of mset_t in ap_abstract0_t (no copy) */


  /* ============================================================ */
  /* I.2 Control of the internal representation */
  /* ============================================================ */

void mset_minimize (ap_manager_t * man, mset_t * a);
  /*
   * Minimize the size of the representation of a. This may result in a
   * later recomputation of internal information.
   */

void mset_canonicalize (ap_manager_t * man, mset_t * a);
  /*
   * Put the abstract value in canonical form. (not yet clear
   * definition)
   */

int mset_hash (ap_manager_t * man, mset_t * a);
  /*
   * Return an hash value for the abstract value.  Two abstract values
   * in canonical from (according to @code{ap_abstract0_canonicalize})
   * and considered as equal by the function ap_abstract0_is_eq should
   * be given the same hash value (this implies more or less a
   * canonical form).
   */

void mset_approximate (ap_manager_t * man, mset_t * a, int algorithm);
  /*
   * Perform some transformation on the abstract value, guided by the
   * field algorithm.
   *
   * The transformation may lose information.  The argument "algorithm"
   * overrides the field algorithm of the structure of type ap_funopt_t
   * associated to ap_abstract0_approximate (commodity feature).
   */

bool mset_is_minimal (ap_manager_t * man, mset_t * a);

bool mset_is_canonical (ap_manager_t * man, mset_t * a);


  /* ============================================================ */
  /* I.3 Printing */
  /* ============================================================ */

void mset_fprint (FILE * stream,
		  ap_manager_t * man, mset_t * a, char **name_of_dim);
  /*
   * Print the abstract value in a pretty way, using function
   * name_of_dim to name dimensions
   */

void mset_fprintdiff (FILE * stream,
		      ap_manager_t * man,
		      mset_t * a1, mset_t * a2, char **name_of_dim);
  /*
   * Print the difference between a1 (old value) and a2 (new value),
   * using function name_of_dim to name dimensions. The meaning of
   * difference is library dependent.
   */

void mset_fdump (FILE * stream, ap_manager_t * man, mset_t * a);
  /*
   * Dump the internal representation of an abstract value, for
   * debugging purposes
   */


  /* ============================================================ */
  /* I.4 Serialization */
  /* ============================================================ */

ap_membuf_t mset_serialize_raw (ap_manager_t * man, mset_t * a);
  /*
   * Allocate a memory buffer (with malloc), output the abstract value
   * in raw binary format to it and return a pointer on the memory
   * buffer and the size of bytes written.  It is the user
   * responsability to free the memory afterwards (with free).
   */

mset_t *mset_deserialize_raw (ap_manager_t * man, void *ptr, size_t * size);
  /*
   * Return the abstract value read in raw binary format from the input
   * stream and store in size the number of bytes read
   */


  /* ********************************************************************** */
  /* II. Constructor, accessors, tests and property extraction */
  /* ********************************************************************** */

  /* ============================================================ */
  /* II.1 Basic constructors */
  /* ============================================================ */

  /*
   * We assume that dimensions [0..intdim-1] correspond to integer
   * variables, and dimensions [intdim..intdim+realdim-1] to real
   * variables
   */

mset_t *mset_bottom (ap_manager_t * man, size_t intdim, size_t realdim);
  /* Create a bottom (empty) value */

mset_t *mset_top (ap_manager_t * man, size_t intdim, size_t realdim);
  /* Create a top (universe) value */

mset_t *mset_of_box (ap_manager_t * man,
		     size_t intdim, size_t realdim,
		     ap_interval_t ** tinterval);
  /*
   * Abstract an hypercube defined by the array of intervals of size
   * intdim+realdim
   */

mset_t *mset_of_lincons_array (ap_manager_t * man,
			       size_t intdim, size_t realdim,
			       ap_lincons0_array_t * array);
  /* Abstract a conjunction of linear constraints */

mset_t *mset_of_tcons_array (ap_manager_t * man,
			     size_t intdim, size_t realdim,
			     ap_tcons0_array_t * array);
  /* Abstract a conjunction of tree expressions constraints */


  /* ============================================================ */
  /* II.2 Accessors */
  /* ============================================================ */

ap_dimension_t mset_dimension (ap_manager_t * man, mset_t * a);
  /* Return the total number of dimensions of the abstract values */


  /* ============================================================ */
  /* II.3 Tests */
  /* ============================================================ */

  /*
   * In abstract tests,
   *
   * - true means that the predicate is certainly true.
   *
   * - false means by default don't know (an exception has occurred, or
   * the exact computation was considered too expensive to be
   * performed).
   *
   * However, if the flag exact in the manager is true, then false means
   * really that the predicate is false.
   */

bool mset_is_bottom (ap_manager_t * man, mset_t * a);
bool mset_is_top (ap_manager_t * man, mset_t * a);

bool mset_is_leq (ap_manager_t * man, mset_t * a1, mset_t * a2);
  /* inclusion check */

bool mset_is_eq (ap_manager_t * man, mset_t * a1, mset_t * a2);
  /* equality check */

bool mset_sat_lincons (ap_manager_t * man, mset_t * a, ap_lincons0_t * cons);
  /* does the abstract value satisfy the linear constraint ? */

bool mset_sat_tcons (ap_manager_t * man, mset_t * a, ap_tcons0_t * cons);
  /* does the abstract value satisfy the tree expression constraint ? */

bool mset_sat_interval (ap_manager_t * man, mset_t * a,
			ap_dim_t dim, ap_interval_t * interval);
  /* is the dimension included in the interval in the abstract value ? */

bool mset_is_dimension_unconstrained (ap_manager_t * man, mset_t * a,
				      ap_dim_t dim);
  /* is the dimension unconstrained ? */

  /* ============================================================ */
  /* II.4 Extraction of properties */
  /* ============================================================ */

ap_interval_t *mset_bound_linexpr (ap_manager_t * man,
				   mset_t * a, ap_linexpr0_t * expr);
  /*
   * Returns the interval taken by a linear expression over the
   * abstract value
   */

ap_interval_t *mset_bound_texpr (ap_manager_t * man,
				 mset_t * a, ap_texpr0_t * expr);
  /*
   * Returns the interval taken by a tree expression over the abstract
   * value
   */

ap_interval_t *mset_bound_dimension (ap_manager_t * man,
				     mset_t * a, ap_dim_t dim);
  /*
   * Returns the interval taken by the dimension over the abstract
   * value
   */

ap_lincons0_array_t mset_to_lincons_array (ap_manager_t * man, mset_t * a);
  /*
   * Converts an abstract value to a polyhedra (conjunction of linear
   * constraints).
   */

ap_tcons0_array_t mset_to_tcons_array (ap_manager_t * man, mset_t * a);
  /*
   * Converts an abstract value to a conjunction of tree expressions
   * constraints
   */

ap_interval_t **mset_to_box (ap_manager_t * man, mset_t * a);
  /*
   * Converts an abstract value to an interval/hypercube. The size of
   * the resulting array is mset_dimension(man,a).  This function can
   * be reimplemented by using mset_bound_linexpr
   */

ap_generator0_array_t mset_to_generator_array (ap_manager_t * man,
					       mset_t * a);
  /* Converts an abstract value to a system of generators. */


  /* ********************************************************************** */
  /* III. Operations */
  /* ********************************************************************** */

  /* ============================================================ */
  /* III.1 Meet and Join */
  /* ============================================================ */

mset_t *mset_meet (ap_manager_t * man, bool destructive, mset_t * a1,
		   mset_t * a2);
mset_t *mset_join (ap_manager_t * man, bool destructive, mset_t * a1,
		   mset_t * a2);
  /* Meet and Join of 2 abstract values */

mset_t *mset_meet_array (ap_manager_t * man, mset_t ** tab, size_t size);
mset_t *mset_join_array (ap_manager_t * man, mset_t ** tab, size_t size);
  /*
   * Meet and Join of an array of abstract values. Raises an
   * [[exc_invalid_argument]] exception if [[size==0]] (no way to
   * define the dimensionality of the result in such a case
   */

mset_t *mset_meet_lincons_array (ap_manager_t * man,
				 bool destructive, mset_t * a,
				 ap_lincons0_array_t * array);
  /*
   * Meet of an abstract value with a set of constraints (generalize
   * mset_of_lincons_array)
   */

mset_t *mset_meet_tcons_array (ap_manager_t * man,
			       bool destructive, mset_t * a,
			       ap_tcons0_array_t * array);
  /*
   * Meet of an abstract value with a set of tree expressions
   * constraints. (generalize mset_of_tcons_array)
   */

mset_t *mset_add_ray_array (ap_manager_t * man,
			    bool destructive, mset_t * a,
			    ap_generator0_array_t * array);
  /*
   * Generalized time elapse operator. Note: this is not like adding
   * arbitrary generators because: - mset_add_ray_array is strict -
   * array can only contain rays and lines, not vertices
   */

  /* ============================================================ */
  /* III.2 Assignement and Substitutions */
  /* ============================================================ */

mset_t *mset_assign_linexpr_array (ap_manager_t * man,
				   bool destructive, mset_t * a,
				   ap_dim_t * tdim,
				   ap_linexpr0_t ** texpr,
				   size_t size, mset_t * dest);
mset_t *mset_substitute_linexpr_array (ap_manager_t * man,
				       bool destructive, mset_t * a,
				       ap_dim_t * tdim,
				       ap_linexpr0_t ** texpr,
				       size_t size, mset_t * dest);
mset_t *mset_assign_texpr_array (ap_manager_t * man,
				 bool destructive, mset_t * a,
				 ap_dim_t * tdim,
				 ap_texpr0_t ** texpr,
				 size_t size, mset_t * dest);
mset_t *mset_substitute_texpr_array (ap_manager_t * man,
				     bool destructive, mset_t * a,
				     ap_dim_t * tdim,
				     ap_texpr0_t ** texpr,
				     size_t size, mset_t * dest);
  /*
   * Parallel Assignement and Substitution of several dimensions by
   * expressions in abstract value a.
   *
   * dest is an optional argument. If not NULL, semantically speaking, the
   * result of the transformation is intersected with dest. This is
   * useful for precise backward transformations in lattices like
   * intervals or octagons.
   */


  /* ============================================================ */
  /* III.3 Projections */
  /* ============================================================ */

mset_t *mset_forget_array (ap_manager_t * man,
			   bool destructive, mset_t * a,
			   ap_dim_t * tdim, size_t size, bool project);


  /* ============================================================ */
  /* III.4 Change and permutation of dimensions */
  /* ============================================================ */

mset_t *mset_add_dimensions (ap_manager_t * man,
			     bool destructive, mset_t * a,
			     ap_dimchange_t * dimchange, bool project);

mset_t *mset_remove_dimensions (ap_manager_t * man,
				bool destructive, mset_t * a,
				ap_dimchange_t * dimchange);

mset_t *mset_permute_dimensions (ap_manager_t * man,
				 bool destructive,
				 mset_t * a, ap_dimperm_t * permutation);
  /*
   * Size of the permutation is supposed to be equal to the dimension
   * of the abstract value
   */


  /* ============================================================ */
  /* III.5 Expansion and folding of dimensions */
  /* ============================================================ */

mset_t *mset_expand (ap_manager_t * man,
		     bool destructive, mset_t * a, ap_dim_t dim, size_t n);
  /*
   * Expand the dimension dim into itself + n additional dimensions. It
   * results in (n+1) unrelated dimensions having same relations with
   * other dimensions. The (n+1) dimensions are put as follows:
   *
   * - original dimension dim
   *
   * - if the dimension is integer, the n additional dimensions are put at
   * the end of integer dimensions; if it is real, at the end of the
   * real dimensions.
   */

mset_t *mset_fold (ap_manager_t * man,
		   bool destructive, mset_t * a,
		   ap_dim_t * tdim, size_t size);
  /*
   * Fold the dimensions in the array tdim of size n>=1 and put the
   * result in the first dimension in the array. The other dimensions
   * of the array are then removed.
   */


  /* ============================================================ */
  /* III.6 Widening, Narrowing */
  /* ============================================================ */

mset_t *mset_widening (ap_manager_t * man, mset_t * a1, mset_t * a2);
  /* Standard widening: set unstable constraints to +oo */

mset_t *mset_widening_thresholds (ap_manager_t * man,
				  mset_t * a1, mset_t * a2,
				  ap_scalar_t ** array, size_t nb);
  /*
   * Widening with threshold. array is assumed to contain nb
   * thresholds, sorted in increasing order.
   */

mset_t *mset_narrowing (ap_manager_t * man, mset_t * a1, mset_t * a2);
  /* Standard narrowing: refine only +oo constraint */


  /* ============================================================ */
  /* III.7 Topological closure operation */
  /* ============================================================ */

mset_t *mset_closure (ap_manager_t * man, bool destructive, mset_t * a);
  /*
   * Returns the topological closure of a possibly opened abstract
   * value
   */


  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __MSET_FUN_H */
