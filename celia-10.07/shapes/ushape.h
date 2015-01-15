/*
 * shape.h
 *
 * Public definitions.
 * The only file you need to include to use unit elements of shapes.
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

#ifndef __USHAPE_H
#define __USHAPE_H


/* dependencies */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "ap_coeff.h"
#include "ap_dimension.h"
#include "ap_expr0.h"
#include "ap_manager.h"
#include "ap_abstract0.h"

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

ap_manager_t *ushape_manager_alloc (void);
  /* Creates a new manager for the shape library. */


  /* ============================================================ */
  /* Supplementary functions & options, not in the APRON manager. */
  /* ============================================================ */

  /* TODO: add functions not in the APRON manager. */

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */


#endif /* __SUHAPE_H */
