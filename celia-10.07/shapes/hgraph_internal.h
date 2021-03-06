/*
 * hgraph_internal.h
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

#ifndef __HGRAPH_INTERNAL_H_
#define __HGRAPH_INTERNAL_H_

#include "uthash.h"
#include "hgraph_fun.h"
#include "shape_manager.h"
#include "apron2shape.h"


/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

  /* ********************************************************************** */
  /* Heap graphs */
  /* ********************************************************************** */

  /* ============================================================ */
  /* Internal Representation */
  /* ============================================================ */

  /**
   * Heap graphs are represented using their adjacency matrix
   * given by the field {@code info} (and its subfield {@code s}).
   * Each node is labeled by a variable {@code v} representing
   * the ptr variable with the least dimension from which
   * the node is accessible by traversing {@code nn} fields {@code next}.
   * To obtain a canonical form for such graph, nodes are ordered
   * wrt lexicographical order on the pair {@code (nn,v)}.
   *
   * Since a node can be labeld by several variables, the vector v2n
   * gives for each pointer variable, the node labeled.
   *
   * Important note: the {@code ptrdim} corresponds to the number
   * of pointer variables in the program, including variables
   * used for encoding pointer operations or constants, i.e,
   * _data, _free, _len, _next, _new, _null.
   * All these variables are always labeling the null node.
   *
   */
typedef size_t node_t;

#define NODE_T_TOP ((size_t) -1)

struct _node_info_t
{
  node_t s;			/* next successor for this node >= 0 */
  size_t v;			/* least dim of ptr variable labeling the
				 * node or its predecessor cut point */
  size_t nn;			/* nb of next fields from v */
  /*
   * TODO: add more on mapping length and data corresponding to
   * this node
   */
};

typedef struct _node_info_t node_info_t;

struct _hgraph_t
{
  UT_hash_handle hh;		/* make structure hashable keys from field
				 * size--info */
  bool closed;			/* flag if the graph is in the canonical form */

  size_t size;			/* number of nodes in the graph (including
				 * null) */
  size_t datadim;		/* number of non-pointer variables */
  size_t ptrdim;		/* number of ptr vars labeling the nodes,
				 * including null */
  node_info_t info[];		/* array of size+ptrdim node informations
				 * - [0,ptrdim-1] maps vars to nodes by field s
				 * - [ptrdim,ptrdim+size] maps nodes to infos. */
};

#define VAR2NODE(a,v)           (a->info[v].s)
#define NODE_NEXT(a,n)          a->info[a->ptrdim+n].s
#define NODE_VAR(a,n)           a->info[a->ptrdim+n].v
#define NODE_VAR_NEXT(a,n)      a->info[a->ptrdim+n].nn

  /** Array of hgraphs */
typedef struct
{
  hgraph_t **p;
  size_t size;
} hgraph_array_t;


#define hgraph_init_from_manager(man,id,size)  ((hgraph_internal_t*) shape_init_from_manager(man,id,size))


  /* ============================================================ */
  /* Internal Management */
  /* ============================================================ */

  /* see hgraph_representation.c */

hgraph_t *hgraph_alloc_null (hgraph_internal_t * pr, size_t datadim,
			     size_t ptrdim);
  /*
   * Builds the null graph: only one node labeled by null, not in the
   * htable
   */

hgraph_t *hgraph_alloc_internal (hgraph_internal_t * pr, size_t size,
				 size_t datadim, size_t ptrdim);
  /*
   * Builds a graph with size nodes, only null node labeled, not in the
   * htable
   */

hgraph_t *hgraph_make (hgraph_internal_t * pr, size_t code,
		       size_t datadim, size_t ptrdim);
hgraph_t *hgraph_random (hgraph_internal_t * pr, size_t size,
			 size_t datadim, size_t ptrdim);
  /* Builds radomly a hgraph with size nodes and ptrdim variables. */

void hgraph_free_mem (hgraph_internal_t * pr, hgraph_t * a);
void hgraph_free_internal (hgraph_internal_t * pr, hgraph_t * a);
  /* Free a graph if it is not in htable */

char hgraph_check (hgraph_internal_t * pr, hgraph_t * a);
  /* Check integrity of the canonical representation of hgraphs */

int hgraph_cmp_internal (hgraph_internal_t * pr, hgraph_t * a, hgraph_t * b);
hgraph_t *hgraph_copy_mem (hgraph_internal_t * pr, hgraph_t * a);
hgraph_t *hgraph_copy_internal (hgraph_internal_t * pr, hgraph_t * a);
void hgraph_fdump_internal (FILE * stream, hgraph_internal_t * pr,
			    hgraph_t * a);

hgraph_array_t *hgraph_array_make (hgraph_internal_t * pr, size_t size);
void hgraph_array_clear (hgraph_internal_t * pr, hgraph_array_t * a);
int hgraph_array_resize (hgraph_internal_t * pr, hgraph_array_t * a,
			 size_t size);
hgraph_array_t *hgraph_array_copy (hgraph_internal_t * pr,
				   hgraph_array_t * a);
int hgraph_array_add (hgraph_internal_t * pr, hgraph_array_t * dst,
		      bool docopy, bool destructive, hgraph_t * a);
hgraph_array_t *hgraph_array_merge (hgraph_internal_t * pr, bool destructive,
				    hgraph_array_t * a, hgraph_array_t * b);
void hgraph_array_fdump (FILE * stream, ap_manager_t * man,
			 hgraph_array_t * a);
  /* Manipulation of arrays of hgraphs  */

  /* ============================================================ */
  /* Direct Management of Nodes: Use with care! */
  /* ============================================================ */

  /* see hgraph_representation.c */

#define hgraph_node_forall(a,n) for(n=0; n < a->size; n++)
  /* Iterate n among nodes in a */

#define hgraph_node_get_null(a)  ((node_t) 0)
node_t hgraph_node_get_succ (hgraph_t * a, node_t n);
node_t hgraph_node_get_succ_cut (hgraph_t * a, node_t n, int *anon);
  /*
   * Get the cut successor of n in a or NODE_T_TOP; annon counts the
   * anonymous nodes
   */
node_t hgraph_node_get_nsucc (hgraph_t * a, node_t n, size_t nsucc);
  /* Get the nsucc successor of n in a or NODE_T_TOP */
size_t *hgraph_node_get_prev (hgraph_t * a);
  /* Compute the number of predecessors for each node */

bool hgraph_node_is_cut (hgraph_t * a, node_t n);
bool hgraph_node_is_used (hgraph_t * a, node_t n);
bool hgraph_node_is_lt (node_info_t * a, node_info_t * b);
  /* Comparison used to sort nodes */
bool hgraph_node_is_reachable (hgraph_t * a, node_t init, node_t target);
  /* True if target is reachable from init in a */
size_t hgraph_node_is_sorted (hgraph_t * a);
  /* Return first node for which a->info is not sorted */

hgraph_t *hgraph_node_add (hgraph_internal_t * pr, hgraph_t * a, node_t s,
			   size_t v, size_t nn, node_t * n,
			   ap_dimperm_t * perm);
  /*
   * Add node with info (s,v,nn) to a; produce permutation of OLD nodes in
   * perm
   */

void hgraph_node_sort (hgraph_t * a, size_t i, ap_dimperm_t * perm);
   /* Reorder nodes stating from i and compose with perm */

void hgraph_node_set_var (hgraph_t * a, node_t n, size_t v,
			  ap_dimperm_t * perm);
  /* Set var of n to be v; produce permutation of nodes in perm */
void hgraph_node_set_succ (hgraph_t * a, node_t n, node_t nn,
			   ap_dimperm_t * perm);
  /* Set successor of n to be nn; produce permutation of nodes in perm */


node_t hgraph_get_var_node (hgraph_t * a, size_t v);
  /* Getter for the node labeled by v in a */
size_t *hgraph_get_var2node (hgraph_t * a);
/* Getter for the mapping (allocated) var to nodes */

hgraph_t *hgraph_resize (hgraph_internal_t * pr, hgraph_t * a, size_t size);
  /* Copy a but with size nodes; nodes added not filled. */

  /* ============================================================ */
  /* Tests */
  /* ============================================================ */

  /* see hgraph_predicate.c */

bool hgraph_is_equal (hgraph_t * a, hgraph_t * b);
bool hgraph_is_lt (hgraph_t * a, hgraph_t * b);
int hgraph_cmp (hgraph_t * a, hgraph_t * b);
bool hgraph_is_closed (hgraph_internal_t * pr, hgraph_t * a);
bool hgraph_sat_pcons (hgraph_internal_t * pr, hgraph_t * a, pcons0_t * c);
  /* Version for sat with pointer constraints */

  /* ============================================================ */
  /* Meet and Join */
  /* ============================================================ */

  /* see hgraph_nary.c */

hgraph_array_t *hgraph_meet_pcons_array (hgraph_internal_t * pr,
					 bool destructive, hgraph_t * a,
					 pcons0_array_t * c);
  /* Version for meet returning a set of hgraphs from shape constraints */

/* ============================================================ */
/* Assignment */
/* ============================================================ */

typedef struct
{
  ap_dim_t *p;
  size_t size;
} ap_dim_array_t;

typedef struct
{
  ap_dim_array_t *p;
  size_t size;
} ap_dim_array2_t;

  /* see hgraph_transfer.c */
void hgraph_node_anon_array_clear (hgraph_internal_t * pr,
				   ap_dim_array2_t * anon);
hgraph_t *hgraph_assign_x_y (hgraph_internal_t * pr, hgraph_t * a, size_t vx,
			     size_t vy, ap_dimperm_t * perm);
hgraph_t *hgraph_assign_x_y_next (hgraph_internal_t * pr, bool conservative,
				  hgraph_t * a, size_t vx, size_t vy,
				  ap_dimperm_t * perm);
hgraph_t *hgraph_assign_x_null (hgraph_internal_t * pr, hgraph_t * a,
				size_t vx, ap_dimperm_t * perm,
				ap_dim_array2_t * anon);
hgraph_t *hgraph_assign_x_new (hgraph_internal_t * pr, hgraph_t * a,
			       size_t vx, ap_dimperm_t * perm);
hgraph_t *hgraph_assign_x_free (hgraph_internal_t * pr, hgraph_t * a,
				size_t vx, ap_dimperm_t * perm,
				ap_dim_array2_t * anon);
hgraph_t *hgraph_assign_x_next_null (hgraph_internal_t * pr, hgraph_t * a,
				     size_t vx, ap_dimperm_t * perm,
				     ap_dim_array2_t * anon);
hgraph_t *hgraph_assign_x_next_free (hgraph_internal_t * pr, hgraph_t * a,
				     size_t vx, ap_dimperm_t * perm,
				     ap_dim_array2_t * anon);
hgraph_t *hgraph_assign_x_next_y (hgraph_internal_t * pr, hgraph_t * a,
				  size_t vx, size_t vy, ap_dimperm_t * perm);
hgraph_t *hgraph_assign_passign (hgraph_internal_t * pr, bool conservative,
				 hgraph_t * a, passign0_t * op);
  /* Basic version for assignment, where garbage and anonymous nodes are returned */

  /* ============================================================ */
  /* Closure Algorithms */
  /* ============================================================ */

  /* see hgraph_closure.c */
size_t hgraph_close_garbage (hgraph_internal_t * pr, hgraph_t * a,
			     ap_dimperm_t * perm);
hgraph_t *hgraph_close_anonymous (hgraph_internal_t * pr, hgraph_t * a,
				  ap_dimperm_t * perm, ap_dim_array2_t * anon,
				  size_t ngarbage);
hgraph_t *hgraph_close (hgraph_internal_t * pr, hgraph_t * a,
			ap_dimperm_t * perm, ap_dim_array2_t * anon);
  /* In place closure */

  /* TODO: other functions related to the internal representation */

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __HGRAPH_INTERNAL_H_ */
