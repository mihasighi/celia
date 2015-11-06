/* ************************************************************************* */
/* hg_manager.h: global manager for heap graphs passed to all functions */
/* ************************************************************************* */

/* This file is part of the SHAD Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _HG_MANAGER_H_
#define _HG_MANAGER_H_

#include <stdlib.h>
#include <stdio.h>
#include "sh_result.h"

#ifdef __cplusplus
extern "C" {
#endif


    /* ********************************************************************** */
    /* I. Types */
    /* ********************************************************************** */

    /* ====================================================================== */
    /* I.1 Identifying functions */

    /* ====================================================================== */

    typedef enum hg_funid_t {
        HG_FUNID_UNKNOWN = 0,
        HG_FUNID_COPY,
        HG_FUNID_FREE,
        HG_FUNID_SIZE,
        HG_FUNID_ADD_EDGE,
        HG_FUNID_ADD_NODE,
        HG_FUNID_ADD_LABEL,
        HG_FUNID_EXPAND_EDGE,
        HG_FUNID_GET_EDGE,
        HG_FUNID_GET_NULL,
        HG_FUNID_GET_NODES,
        HG_FUNID_FPRINT,
        HG_FUNID_FPRINTDIFF,
        HG_FUNID_FDUMP,
        HG_FUNID_SERIALIZE_RAW,
        HG_FUNID_DESERIALIZE_RAW,
        HG_FUNID_BOTTOM,
        HG_FUNID_TOP,
        HG_FUNID_DIMENSION,
        HG_FUNID_IS_BOTTOM,
        HG_FUNID_IS_TOP,
        HG_FUNID_IS_LEQ,
        HG_FUNID_IS_EQ,
        HG_FUNID_IS_DIMENSION_UNCONSTRAINED,
        HG_FUNID_IS_REACHABLE,
        HG_FUNID_SAT_PCONS,
        HG_FUNID_TO_PCONS_ARRAY,
        HG_FUNID_MEET,
        HG_FUNID_JOIN,
        HG_FUNID_MEET_PCONS_ARRAY,
        HG_FUNID_ASSIGN_PEXPR,
        HG_FUNID_SUBSTITUTE_PEXPR,
        HG_FUNID_SUBSTITUTE_DIMENSIONS,
        HG_FUNID_UNIFY,
        HG_FUNID_ADD_DIMENSIONS,
        HG_FUNID_DUP_DIMENSIONS,
        HG_FUNID_REMOVE_DIMENSIONS,
        HG_FUNID_PERMUTE_DIMENSIONS,
        HG_FUNID_CLOSURE,
        HG_FUNID_IS_CLOSED,
        HG_FUNID_SIZE2 /* For the size of the array below */
    } hg_funid_t;

    extern const char* hg_name_of_funid[HG_FUNID_SIZE2];
    /* give the name of a function identifier */


    /* ====================================================================== */
    /* I.2 Manager */
    /* ====================================================================== */

    /* Manager (opaque type) */
    typedef struct hg_manager_t {
        char* library; /* name of the effective library */
        char* version; /* version of the effective library */
        void* internal; /* library dependent,
				    should be different for each thread
				    (working space) */
        void* funptr[HG_FUNID_SIZE2]; /* Array of function pointers,
				    initialized by the effective library */
        sh_option_t option; /* Options (in) */
        sh_result_t result; /* Exceptions and other indications (out) */
        void (*internal_free)(void*); /* deallocation function for internal */
        size_t count; /* reference counter */
    } hg_manager_t;

    /* ********************************************************************** */
    /* II. User Functions */
    /* ********************************************************************** */

    void hg_manager_clear_exclog(hg_manager_t* man);
    /* erase the current log of exception */
    void hg_manager_free(hg_manager_t* man);
    /* dereference the counter,
       and possibly free internal field if it is not yet put to NULL */

    /* Reading fields */
    const char* hg_manager_get_library(hg_manager_t* man);
    const char* hg_manager_get_version(hg_manager_t* man);

    sh_funopt_t hg_manager_get_funopt(hg_manager_t* man, hg_funid_t funid);
    bool hg_manager_get_abort_if_exception(hg_manager_t* man, sh_exc_t exn);

    sh_exc_t hg_manager_get_exception(hg_manager_t* man);
    /* Get the last exception raised */
    sh_exclog_t* hg_manager_get_exclog(hg_manager_t* man);
    /* Get the full log of exception */

    /* Settings fields */
    void hg_funopt_init(sh_funopt_t* fopt);
    void hg_manager_set_funopt(hg_manager_t* man, hg_funid_t funid, sh_funopt_t* funopt);
    void hg_manager_set_abort_if_exception(hg_manager_t* man, sh_exc_t exn, bool flag);

    /* ********************************************************************** */
    /* II. Implementor Functions */
    /* ********************************************************************** */

    hg_manager_t* hg_manager_alloc(char* library, char* version,
            void* internal,
            void (*internal_free)(void*));
    static inline
    hg_manager_t* hg_manager_copy(hg_manager_t* man);
    /* Increment the reference counter and return its argument */
    void hg_manager_raise_exception(hg_manager_t* man,
            sh_exc_t exn, hg_funid_t funid, const char* msg);
    /* raise an exception */


    /* ********************************************************************** */
    /* III. Definition of previously declared inline functions */
    /* ********************************************************************** */

    static inline
    hg_manager_t* hg_manager_copy(hg_manager_t* man) {
        man->count++;
        return man;
    }
#ifdef __cplusplus
}
#endif

#endif
