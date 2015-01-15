/*
 * shape_manager.h
 *
 * Private definition of the library part (internal) part of the manager.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef SHAPE_MANAGER_H_
#define SHAPE_MANAGER_H_

#include "hgraph_fun.h"
#include "shape_fun.h"
#include "ap_pcons0.h"
#include "ap_passign0.h"
#include "ap_manager.h"

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

  /* manager-local data specific to shapes */
struct _shape_internal_t
{

  /* current function */
  ap_funid_t funid;

  /* local parameters for current function */
  ap_funopt_t *funopt;

  /* hash-tables */
  hgraph_t *hgraphs;		/* heap graphs */
  pcons0_t *pcons;		/* ptr constraints */
  passign0_t *passigns;		/* ptr assignments */

  /* manager of the segment domains */
  size_t size_scons;
  ap_manager_t **man_scons;	/* array of scons_size managers */

  /* max number for anonymous nodes for closure */
  size_t max_anon;
  size_t segm_anon;

  /* count errors and files */
  int error_;
  long int filenum;

  /* default dimensions */
  size_t intdim;
  size_t realdim;

  /* TODO: other data */

  /* back-pointer */
  ap_manager_t *man;
};

typedef struct _shape_internal_t shape_internal_t;
  /* Abstract data type of library-specific manager options. */

  /* ============================================================ */
  /* Basic management. */
  /* ============================================================ */

  /* called by each function to setup and get manager-local data */
static inline shape_internal_t *
shape_init_from_manager (ap_manager_t * man, ap_funid_t id, size_t size)
{
  shape_internal_t *pr = (shape_internal_t *) man->internal;
  pr->funid = id;
  pr->funopt = man->option.funopt + id;
  man->result.flag_exact = man->result.flag_best = true;

  /* TODO: set other internal data from manager */
  /* DO NOT TOUCH TO THE hgraphs FIELD! */

  return pr;
}

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */


#endif /* SHAPE_MANAGER_H_ */
