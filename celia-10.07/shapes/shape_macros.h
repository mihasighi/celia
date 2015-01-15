/*
 * shape_macros.h
 *
 * Useful macros.
 *
 * APRON Library / Shape Domain
 *
 * Copyright (C) LIAFA 2009
 */

/*
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef __SHAPE_MACROS_H_
#define __SHAPE_MACROS_H_

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

  /* invalid argument exception */
#define arg_assert(cond,action)                                         \
do { if (!(cond)) {                                                   \
    char buf_[1024];                                                  \
    snprintf(buf_,sizeof(buf_),                                       \
             "assertion (%s) failed in %s at %s:%i",                  \
             #cond, __func__, __FILE__, __LINE__);            \
    ap_manager_raise_exception(pr->man,AP_EXC_INVALID_ARGUMENT,       \
                               pr->funid,buf_);                       \
    action }                                                          \
} while(0)

  /* internal errors */
#define ERROR(msg,action)                                       \
  do {                                                  \
    fprintf(stderr,"\n%s\n",msg);                       \
    action                                      \
    pr->error_++;                                               \
  } while (0)

  /* malloc with safe-guard */
#define checked_malloc(ptr,t,size,nb,action)                               \
do {                                                                  \
  (ptr) = (t*)malloc(size*(nb));                                 \
  if (!(ptr)) {                                                       \
    char buf_[1024];                                                  \
    snprintf(buf_,sizeof(buf_),                                       \
             "cannot allocate %s[%lu] for %s in %s at %s:%i",         \
             #t, (long unsigned)(nb), #ptr,                           \
             __func__, __FILE__, __LINE__);                           \
    ap_manager_raise_exception(pr->man,AP_EXC_OUT_OF_SPACE,           \
                               pr->funid,buf_);                       \
    action }                                                          \
} while(0)


  /* calloc with safe-guard, needed by hash table */
#define checked_calloc(ptr,t,size,nb,action)                               \
do {                                                                  \
  (ptr) = (t*)calloc(size,(nb));                                 \
  if (!(ptr)) {                                                       \
    char buf_[1024];                                                  \
    snprintf(buf_,sizeof(buf_),                                       \
             "cannot allocate %s[%lu] for %s in %s at %s:%i",         \
             #t, (long unsigned)(nb), #ptr,                           \
             __func__, __FILE__, __LINE__);                           \
    ap_manager_raise_exception(pr->man,AP_EXC_OUT_OF_SPACE,           \
                               pr->funid,buf_);                       \
    action }                                                          \
} while(0)

#define checked_realloc(ptr,t,size,nb,action)                               \
do {                                                                  \
  (ptr) = (t*)realloc(ptr,size*(nb));                                 \
  if (!(ptr)) {                                                       \
    char buf_[1024];                                                  \
    snprintf(buf_,sizeof(buf_),                                       \
             "cannot allocate %s[%lu] for %s in %s at %s:%i",         \
             #t, (long unsigned)(nb), #ptr,                           \
             __func__, __FILE__, __LINE__);                           \
    ap_manager_raise_exception(pr->man,AP_EXC_OUT_OF_SPACE,           \
                               pr->funid,buf_);                       \
    action }                                                          \
} while(0)


  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */


#endif /* SHAPE_MACROS_H_ */
