/*
 * ucons_internal.h
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

#ifndef __UCONS_INTERNAL_H_
#define __UCONS_INTERNAL_H_

#include "uthash.h"
#include "shape_options.h"
#include "shape_macros.h"
#include "shape_manager.h"
#include "ucons_fun.h"


/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
	/* *INDENT-ON* */

	/* ********************************************************************** */
	/* Manager */
	/* ********************************************************************** */

	/* ============================================================ */
	/* Internal Representation */
	/* ============================================================ */



typedef enum
{
  pattern_1,			/* y : PI[0]*/
  pattern_2_1,			/* y1 in ? , y2 in ?? : PI[1]*/
  pattern_3_1,			/* y1 in ? , y2 in ?? , y3 in ??? : PI[2] */
  pattern_1_2,		/* y1 < y2  : PI[3] */
  pattern_succ1_3,		/* y3 = y2 + 1 = y1 + 2  */
  pattern_1_l1,			/* y /\ y=1*/
  pattern_1_l			/* y /\ y=l[?]-1 */
} pattern_kind;

typedef struct
{
  size_t size; //number of universal vars on the segment
  bool *reach;
} pattern_var_t;


typedef struct
{
  pattern_kind kind;
  size_t e_seg;			// number of existential segments
  size_t u_seg;			// number of universal segments
  pattern_var_t *uvar;		//sizeof u_seg
  size_t lcons;			// index in LI
  size_t nr_y;			//number of universals on all segments
} pattern_info_t;

	/* manager-local data specific to universal constraints */
struct _ucons_internal_t
{

  /* current function */
  ap_funid_t funid;

  /* local parameters for current function */
  ap_funopt_t *funopt;

//              /* manager of the length domain */
//              ap_manager_t *man_lcons;

  /* manager of the data domain */
  ap_manager_t *man_dcons;

  /* count errors */
  int error_;

  /* back-pointer */
  ap_manager_t *man;

  pattern_info_t *PI;
  /*  list of patterns PR /\ PL */

  size_t PI_size;
  /* number of patterns in the list */


};


	/* ============================================================ */
	/* Basic management. */
	/* ============================================================ */

	/* options for setting basic abstract domains */

#if defined (UCONS_DCONS_OCT_P11)
#define UCONS_DCONS_DOMAIN DOM_OCT_P11
#elif defined (UCONS_DCONS_OCT_P12)
#define UCONS_DCONS_DOMAIN DOM_OCT_P12
#elif defined (UCONS_DCONS_OCT_P21)
#define UCONS_DCONS_DOMAIN DOM_OCT_P21
#elif defined (UCONS_DCONS_POLY_P11)
#define UCONS_DCONS_DOMAIN DOM_POLY_P11
#elif defined (UCONS_DCONS_POLY_P12)
#define UCONS_DCONS_DOMAIN DOM_POLY_P12
#elif defined (UCONS_DCONS_POLY_P21)
#define UCONS_DCONS_DOMAIN DOM_POLY_P21
#else
#define UCONS_DCONS_DOMAIN DOM_BOX
#endif

	/* called by each function to setup and get manager-local data */
static inline ucons_internal_t *
ucons_init_from_manager (ap_manager_t * man, ap_funid_t id, size_t size)
{
  ucons_internal_t *pr = (ucons_internal_t *) man->internal;
  pr->funid = id;
  pr->funopt = man->option.funopt + id;
  man->result.flag_exact = man->result.flag_best = true;

  /* TODO: set other internal data from manager */
  /* DO NOT TOUCH TO THE hgraphs FIELD! */

  return pr;
}


	/* ********************************************************************** */
	/* Universal constraints */
	/* ********************************************************************** */

	/* ============================================================ */
	/* Internal Representation */
	/* ============================================================ */

	/** Universal constraints are parameterized by a set of patterns and
	 * the domains of data and length constraints. It represents
	 * properties of segments of the heap having the form:
	 * lcons /\ forall pattern in segment => /\i (lconsi ==> dconsi)
	 */

	/** Element (lcons ==> dcons) of an universal constraint. */




	/** Data type for patterns.
	 * The current implementation fixes the form of the patterns:
	 * all patterns with at most 3 universals are considered.
	 * The pivot is marqued by *.
	 * Shall be used as an abstract data type.
	 */


typedef struct
{
  size_t type;			// index in PI
  size_t segments[];		//its size is PI[type]->u_seg
} pattern_key_t;


typedef struct
{
  size_t size;			//number of universals?
  pattern_key_t **p;
} pattern_key_set_t;



typedef struct
{
  UT_hash_handle hh;
  ap_abstract0_t *dcons;
  pattern_key_t key;		/* pattern description (reachability + length constraints) */
} pattern_t;


struct _ucons_t
{
  size_t datadim;		// number of program variables of type D
  size_t segmentdim;		//number of segments

  size_t size;

  ap_abstract0_t *econs;

  pattern_key_set_t *n2p;	// size = segmentdim

  pattern_t *udcons;

};

	/* ********************************************************************** */
	/* I. ucons_elm_t */
	/* ********************************************************************** */

	/* see ucons_predicate.c */

//
//      /* ********************************************************************** */
//      /* II. ucons_set_t */
//      /* ********************************************************************** */
//
//      /* see ucons_representation.c */
//
//      void *ucons_set_add_elm (ucons_internal_t * pr, ucons_elm_t * u, ucons_set_t * arr);
//
//      /* see ucons_predicate.c */
//      bool ucons_set_isin (ucons_internal_t * pr, ucons_elm_t * u,
//                      ucons_set_t * arr);
//
//      bool ucons_set_is_top (ucons_internal_t * pr, ucons_set_t * c);
//
//      bool ucons_set_is_bottom (ucons_internal_t * pr, ucons_set_t * c);
//
//
//      /* ********************************************************************** */
//      /* III. pattern_t */see ucons_representation.c
//      /* ********************************************************************** */
//

pattern_key_set_t *pattern_key_set_copy (ucons_internal_t * pr,
					 pattern_key_set_t * a, size_t size);

	//      /* ********************************************************************** */
//      /* IV. ucons_pattern_t */
//      /* ********************************************************************** */
//
//      /* ********************************************************************** */
//      /* V. ucons_t */see ucons_representation.c
//      /* ********************************************************************** */

ucons_t *ucons_alloc_internal (ucons_internal_t * pr, size_t intdim,
			       size_t realdim);
ucons_t *ucons_copy_internal (ucons_internal_t * pr, ucons_t * a);

void ucons_free_internal (ucons_internal_t * pr, ucons_t * a);

	/* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */




#endif /* UCONS_INTERNAL_H_ */
