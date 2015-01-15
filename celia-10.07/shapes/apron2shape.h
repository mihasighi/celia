/*
 * apron2shape.h
 *
 * Translation of the Apron constraints/expressions to Shape ones.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef __APRON2SHAPE_H_
#define __APRON2SHAPE_H_

#include "shape_manager.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

  /* ============================================================ */
  /* Dimensions */
  /* ============================================================ */

  /* dimension of pre-defined pointer variables */
extern size_t var_ptr_data;
extern size_t var_ptr_free;
extern size_t var_ptr_length;
extern size_t var_ptr_new;
extern size_t var_ptr_next;
extern size_t var_ptr_null;

  /* number of length prg variables */
extern size_t lendim;

#define PTR2REAL_DIM(pr,d)   d
#define REAL2PTR_DIM(pr,d)   d
#define DIM2DATA(d,datadim)  d
#define DIM2PTR(d,datadim)   (d-datadim)

#define RANDOM_PTRVAR(pr,intdim,realdim) (intdim+var_ptr_null+1+lrand48()%(realdim - var_ptr_null -1))

#define RANDOM_PTRVAR_OR_NULL(pr,intdim,realdim) (intdim+var_ptr_null+lrand48()%(realdim - var_ptr_null))

#define PTRVAR_NULL(pr,datadim,ptrdim) (var_ptr_null)

#define IS_INTDIM(d,datadim,ptrdim) (d < datadim)
#define IS_PTRDIM(d,datadim,ptrdim) (datadim <= d && (d-datadim) < ptrdim)

#define IS_PTRVAR(pr,d,datadim,ptrdim) (var_ptr_null <= (d-datadim) \
					 && (d-datadim) < ptrdim)
#define IS_DATAVAR(pr,d,datadim,ptrdim) (var_ptr_data == (d-datadim))
#define IS_FREEVAR(pr,d,datadim,ptrdim) (var_ptr_free == (d-datadim))
#define IS_NEWVAR(pr,d,datadim,ptrdim)  (var_ptr_new  == (d-datadim))
#define IS_NEXTVAR(pr,d,datadim,ptrdim) (var_ptr_next == (d-datadim))
#define IS_NULLVAR(pr,d,datadim,ptrdim) (var_ptr_null == (d-datadim))

#define LEN_DIM(intdim,realdim) lendim
#define DATA_DIM(intdim,realdim) (intdim - lendim)


  /* ============================================================ */
  /* Expressions and constraints */
  /* ============================================================ */

pcons0_t *shape_pcons_of_lincons (shape_internal_t * pr, ap_lincons0_t * c,
				  size_t intdim, size_t realdim);
pcons0_t *shape_pcons_of_tcons (shape_internal_t * pr, ap_tcons0_t * c,
				size_t intdim, size_t realdim);
pcons0_array_t *shape_pcons_array_of_lincons_array (shape_internal_t * pr,
						    ap_lincons0_array_t * c,
						    size_t intdim,
						    size_t realdim);
pcons0_array_t *shape_pcons_array_of_tcons_array (shape_internal_t * pr,
						  ap_tcons0_array_t * c,
						  size_t intdim,
						  size_t realdim);
  /*
   * Convert Apron constraints to pointer constraints. In arrays, constraints
   * are sorted as follows: constraints involving NULL < EQ_CONS <
   * REACH*_CONS < NE_CONS < DATA_CONS
   */

ap_lincons0_t shape_lincons_of_node (shape_internal_t * pr,
				     ap_lincons0_t * c,
				     size_t * v2n,
				     size_t size,
				     size_t intdim, size_t realdim);
  /*
   * Convert a linear constraint on ptr vars and data (intdim+realdim) to a
   * linear expression on nodes and data (size + datadim) using array v2n of
   * size (ptrdim)
   */

  /* ============================================================ */
  /* Assignments */
  /* ============================================================ */

passign0_t *shape_passign_of_linexpr (shape_internal_t * pr,
				      ap_dim_t lhs, ap_linexpr0_t * rhs,
				      size_t intdim, size_t realdim);
passign0_array_t *shape_passign_of_linexpr_array (shape_internal_t * pr,
						  ap_dim_t * lhs,
						  ap_linexpr0_t ** rhs,
						  size_t size, size_t intdim,
						  size_t realdim);
passign0_t *shape_passign_of_texpr (shape_internal_t * pr, ap_dim_t lhs,
				    ap_texpr0_t * rhs, size_t intdim,
				    size_t realdim);
passign0_array_t *shape_passign_of_texpr_array (shape_internal_t * pr,
						ap_dim_t * lhs,
						ap_texpr0_t ** rhs,
						size_t size, size_t intdim,
						size_t realdim);
  /* Convert Apron assignments to pointer assignments */

  /* ============================================================ */
  /* Apron expressions for Shapes */
  /* ============================================================ */

typedef enum
{
  expr_ptr,			/* x */
  expr_reach,			/* x - y */
  expr_reachl,			/* x - y - l */
  expr_lindata,			/* data only */
  expr_next,			/* x * next */
  expr_deref,			/* x / next, x*next / next, */
  expr_data,			/* v + x*data */
  expr_deref_data,		/* expr_data / data */
} exprmode_t;

extern const char *exprname[];

ap_linexpr0_t *shape_linexpr_random (shape_internal_t * pr, exprmode_t mode,
				     size_t datadim, size_t ptrdim);
ap_texpr0_t *shape_texpr_random (shape_internal_t * pr, exprmode_t mode,
				 size_t datadim, size_t ptrdim);
  /* Build radomly Apron expressions useful for Shapes */

ap_lincons0_t shape_lincons_x_cst (ap_constyp_t op, size_t x, int cst,
				   size_t datadim, size_t ptrdim);
ap_linexpr0_t *shape_linexpr_x_y_l (int cx, size_t x, int cy, size_t y,
				    int cl, size_t l, size_t datadim,
				    size_t ptrdim);
ap_lincons0_t shape_lincons_x_y_l (ap_constyp_t op, int cx, size_t x, int cy,
				   size_t y, int cl, size_t l, size_t datadim,
				   size_t ptrdim);
ap_lincons0_t shape_lincons_x_y_v_cst (ap_constyp_t op, size_t code, int cx,
				       size_t x, int cy, size_t y, int cv,
				       size_t v, int cst, size_t datadim,
				       size_t ptrdim);
ap_texpr0_t *shape_texpr_x_y_v_cst_data (int cx, size_t x, int cy, size_t y,
					 int cv, size_t v, int cst, bool rhs,
					 size_t datadim, size_t ptrdim);
ap_tcons0_t shape_tcons_x_y (ap_constyp_t op, size_t x, bool nextx, size_t y,
			     bool nexty, size_t datadim, size_t ptrdim);
ap_tcons0_t shape_tcons_x_cst (ap_constyp_t op, size_t x, bool nextx, int cst,
			       size_t datadim, size_t ptrdim);
  /* Build Apron expressions useful for testing Shapes */

ap_texpr0_t *shape_texpr_x_next (size_t x, int nextx, size_t datadim,
				 size_t ptrdim);
ap_texpr0_t *shape_texpr_x_cst (size_t x, bool nextx, int cst, size_t datadim,
				size_t ptrdim);
ap_texpr0_t *shape_texpr_singleton (int n);
ap_texpr0_t *shape_texpr_split (size_t src, size_t new);
ap_texpr0_t **shape_texpr_merge (size_t ** nodes, size_t size, size_t max);
  /* Build Apron expressions on nodes for the segment abstract domain */

ap_linexpr0_t *shape_linexpr_of_node (shape_internal_t * pr,
				      ap_linexpr0_t * c,
				      size_t * v2n,
				      size_t size,
				      size_t intdim, size_t realdim);
  /*
   * Convert a linear expression on ptr vars and data (intdim+realdim) to a
   * linear expression on nodes and data (size + datadim) using array v2n of
   * size (ptrdim)
   */

void shape_dimperm_copy (ap_dimperm_t * dst, size_t begin,
			 ap_dimperm_t * src);
  /* Put the permutation of nodes from src in the dst starting with begin */
void shape_dimchange_sort (ap_dimchange_t * p);
  /* Do insertion sort for dimensions */

/* ============================================================ */
/* Printing Apron names for Shapes */
/* ============================================================ */

#define MAX_SEGM_DIM  20
extern char **shape_name_of_dim;
void shape_init_name_of_dim (size_t datadim);

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* SHAPE_HGRAPH_H_ */
