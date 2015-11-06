/* ************************************************************************* */
/* sh_result.h: global management of results in all domains */
/* ************************************************************************* */

/* This file is part of the SHAD Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _SH_EXCEPTION_H_
#define _SH_EXCEPTION_H_

#include <stdlib.h>
#include <stdio.h>
#include "ap_config.h"

#ifdef __cplusplus
extern "C" {
#endif


/* ********************************************************************** */
/* I. Types */
/* ********************************************************************** */

/* Domains (public type) */
typedef enum sh_dom_t {
  SH_DOM_HGRAPH,
  SH_DOM_DWORD,
  SH_DOM_SHAPE,
  SH_DOM_SIZE
} sh_dom_t;

extern const char* sh_name_of_domain[SH_DOM_SIZE];

/* Exceptions (public type) */
typedef enum sh_exc_t {
  SH_EXC_NONE,             /* no exception detected */
  SH_EXC_TIMEOUT,          /* timeout detected */
  SH_EXC_OUT_OF_SPACE,     /* out of space detected */
  SH_EXC_OVERFLOW,         /* magnitude overflow detected */
  SH_EXC_MEMORY_ERROR,     /* error while memory access */
  SH_EXC_INVALID_ARGUMENT, /* invalid arguments */
  SH_EXC_NOT_IMPLEMENTED,  /* not implemented */
  SH_EXC_SIZE
} sh_exc_t;

extern const char* sh_name_of_exception[SH_EXC_SIZE];

/* Exception log */
typedef struct sh_exclog_t {
  sh_exc_t exn;
  sh_dom_t dom;                /* abstract domain raising it */
  size_t funid;
  char* msg;                   /* dynamically allocated */
  struct sh_exclog_t* tail;
} sh_exclog_t;

/* Exceptions and other indications (out) (opaque type) */
typedef struct sh_result_t {
  sh_exclog_t* exclog; /* history of exceptions */
  sh_exc_t exn;        /* exception for the last called function */
  bool flag_exact;  /* result is mathematically exact or don't know */
  bool flag_best;   /* result is best correct approximation or don't know */
} sh_result_t;

/* ====================================================================== */
/* I.2 Options */
/* ====================================================================== */

/* Option associated to each function (public type) */
typedef struct sh_funopt_t {
  int algorithm;
  /* Algorithm selection:
     - 0 is default algorithm;
     - otherwise, no accuracy or speed meaning
  */
  size_t timeout; /* unit !? */
  /* Above the given computation time, the function may abort with the
     exception flag flag_time_out on.
  */
  size_t max_object_size; /* in abstract object size unit. */
  /* If during the computation, the size of some object reach this limit, the
     function may abort with the exception flag flag_out_of_space on.
  */
} sh_funopt_t;

/* Options (in) (opaque type) */
typedef struct sh_option_t {
  sh_funopt_t* funopt;            /* dynamically allocated in each manager */
  size_t funsize;
  bool abort_if_exception[SH_EXC_SIZE];
} sh_option_t;

/* ====================================================================== */
/* I.3 Internal options */
/* ====================================================================== */

/* for pretty printer */
extern size_t ushape_number;


/* ********************************************************************** */
/* II. User Functions */
/* ********************************************************************** */

/* Settings fields */
void sh_funopt_init(sh_funopt_t* fopt);

/* ********************************************************************** */
/* III. Implementor Functions */
/* ********************************************************************** */

sh_exclog_t* sh_exc_cons(sh_exc_t exn,
                         sh_dom_t dom,
			 size_t funid, const char* msg,
			 sh_exclog_t* tail);
void sh_exclog_free(sh_exclog_t* head);

void sh_result_add_exception(sh_result_t* result, 
		sh_exc_t exn, sh_dom_t dom,
                size_t funid, const char* msg);

void sh_result_init(sh_result_t* result);

void sh_result_clear(sh_result_t* result);

#ifdef __cplusplus
}
#endif

#endif
