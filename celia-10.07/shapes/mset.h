/*
 * mset.h
 *
 * Public declarations for the multiset abstract domain on list segments.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef __MSET_H_
#define __MSET_H_

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

ap_manager_t *mset_manager_alloc (void);
  /* Creates a new manager for the mset library. */

  /* ============================================================ */
  /* Supplementary functions & options, not in the APRON manager. */
  /* ============================================================ */

  /* TODO: add functions not in the APRON manager. */

  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */

#endif /* __MSET_H_ */
