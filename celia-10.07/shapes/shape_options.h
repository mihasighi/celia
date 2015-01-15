/*
 * shape_options.h
 *
 * Options for the implementation of the shape domain.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef __SHAPE_OPTIONS_H_
#define __SHAPE_OPTIONS_H_

/* Options for the selection of domains */

/* constant names for domains */
typedef enum
{
  DOM_BOX = 0, DOM_OCT, DOM_OCT_P11, DOM_OCT_P12, DOM_OCT_P21, DOM_POLY, DOM_POLY_P11, DOM_POLY_P12, DOM_POLY_P21
} dom_t;


/* the domain for existentials (prg variables and heap nodes) */
#if defined(SHAPE_DCONS_BOX)
#define SHAPE_DCONS_DOMAIN  DOM_BOX
#elif defined (SHAPE_DCONS_OCT)
#define SHAPE_DCONS_DOMAIN  DOM_OCT
#elif defined (SHAPE_DCONS_OCT_P11)
#define SHAPE_DCONS_DOMAIN  DOM_OCT_P11
#elif defined (SHAPE_DCONS_OCT_P12)
#define SHAPE_DCONS_DOMAIN  DOM_OCT_P12
#elif defined (SHAPE_DCONS_OCT_P21)
#define SHAPE_DCONS_DOMAIN  DOM_OCT_P21
#elif defined (SHAPE_DCONS_POLY_P11)
#define SHAPE_DCONS_DOMAIN  DOM_POLY_P11
#elif defined (SHAPE_DCONS_POLY_P12)
#define SHAPE_DCONS_DOMAIN  DOM_POLY_P12
#elif defined (SHAPE_DCONS_POLY_P21)
#define SHAPE_DCONS_DOMAIN  DOM_POLY_P21
#else
#define SHAPE_DCONS_DOMAIN  DOM_POLY
#endif

/* the domains for segments (working in parallel) */
/* for lengths */
#if defined(SHAPE_SCONS_LEN)
#define SHAPE_SCONS_LEN 1
#if defined(SHAPE_SCONS_LEN_BOX)
#define SHAPE_SCONS_LEN_DOMAIN DOM_BOX
#elif defined(SHAPE_SCONS_LEN_OCT)
#define SHAPE_SCONS_LEN_DOMAIN DOM_OCT
#else
#define SHAPE_SCONS_LEN_DOMAIN DOM_POLY
#endif
#else
#define SHAPE_SCONS_LEN 0
#endif

/*
 * for universal constraints on data (options are set inside the domain, see
 * below)
 */
#if defined(SHAPE_SCONS_UCONS)
#define SHAPE_SCONS_UCONS 1
#else
#define SHAPE_SCONS_UCONS 0
#endif

/* TODO: for multiset constraints on data */
#define SHAPE_SCONS_MSET 0

/*
 * for sum constraints
 */
#if defined(SHAPE_SCONS_LSUM)
#define SHAPE_SCONS_LSUM 1
#else
#define SHAPE_SCONS_LSUM 0
#endif


#endif /* __SHAPE_OPTIONS_H_ */
