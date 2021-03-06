/**************************************************************************/
/*                                                                        */
/*  CINV Library / Shape Domain                                           */
/*                                                                        */
/*  Copyright (C) 2009-2011                                               */
/*    LIAFA (University of Paris Diderot and CNRS)                        */
/*                                                                        */
/*                                                                        */
/*  you can redistribute it and/or modify it under the terms of the GNU   */
/*  Lesser General Public License as published by the Free Software       */
/*  Foundation, version 3.                                                */
/*                                                                        */
/*  It is distributed in the hope that it will be useful,                 */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         */
/*  GNU Lesser General Public License for more details.                   */
/*                                                                        */
/*  See the GNU Lesser General Public License version 3.                  */
/*  for more details (enclosed in the file LICENSE).                      */
/*                                                                        */
/**************************************************************************/


#ifndef __USHAPE_H
#define __USHAPE_H


/* dependencies */

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "apron/ap_coeff.h"
#include "apron/ap_dimension.h"
#include "apron/ap_expr0.h"
#include "apron/ap_manager.h"
#include "apron/ap_abstract0.h"

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
