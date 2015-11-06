/**************************************************************************/
/*                                                                        */
/*  This file is part of SHAD Library.                                    */
/*                                                                        */
/*  Copyright (C) 2011                                                    */
/*    LIAFA (University Paris Diderot and CNRS)                           */
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


#ifndef __SH_MACROS_H_
#define __SH_MACROS_H_

/* *INDENT-OFF* */
#ifdef __cplusplus
extern          "C"
{
#endif
  /* *INDENT-ON* */

  /* invalid argument exception */
#define arg_assert(cond,action)                               \
do { if (!(cond)) {                                           \
    fprintf(stdout, "assertion (%s) failed in %s at %s:%i",   \
             #cond, __func__, __FILE__, __LINE__);            \
    fflush(stdout); \
    action; }                                                  \
} while(0)

#define sh_arg_assert(pr,cond,action)                                         \
do { if (!(cond)) {                                                   \
    char buf_[1024];                                                  \
    snprintf(buf_,sizeof(buf_),                                       \
             "assertion (%s) failed in %s at %s:%i",                  \
             #cond, __func__, __FILE__, __LINE__);            \
    sh_manager_raise_exception(pr->man,AP_EXC_INVALID_ARGUMENT,       \
                               pr->funid,buf_);                       \
    fprintf(stdout,"%s\n",buf_); fflush(stdout); \
    action }                                                          \
} while(0)



  /* internal errors */
#define ERROR(msg,action)                                       \
  do {                                                          \
    fprintf(stdout, "exception (%s) raised in %s at %s:%i",            \
              msg, __func__, __FILE__, __LINE__);               \
    fflush(stdout);  \
    action;                                        \
  } while (0)

  /* malloc with safe-guard */
#define checked_malloc(ptr,t,size,nb,action)                               \
do {                                                                  \
  (ptr) = (t*)malloc(size*(nb));                                 \
  if (!(ptr)) {                                                       \
    fprintf(stdout, "cannot allocate %s[%lu] for %s in %s at %s:%i",         \
             #t, (long unsigned)(nb), #ptr,                           \
             __func__, __FILE__, __LINE__);                           \
    fflush(stdout);  \
    action; }                                                          \
} while(0)


  /* calloc with safe-guard, needed by hash table */
#define checked_calloc(ptr,t,size,nb,action)                               \
do {                                                                  \
  (ptr) = (t*)calloc(size,(nb));                                 \
  if (!(ptr)) {                                                       \
    fprintf(stdout, "cannot allocate %s[%lu] for %s in %s at %s:%i",         \
             #t, (long unsigned)(nb), #ptr,                           \
             __func__, __FILE__, __LINE__);                           \
    fflush(stdout);  \
    action; }                                                          \
} while(0)

#define checked_realloc(ptr,t,size,nb,action)                               \
do {                                                                  \
  (ptr) = (t*)realloc(ptr,size*(nb));                                 \
  if (!(ptr)) {                                                       \
    fprintf(stdout, "cannot allocate %s[%lu] for %s in %s at %s:%i",         \
             #t, (long unsigned)(nb), #ptr,                           \
             __func__, __FILE__, __LINE__);                           \
    fflush(stdout);  \
    action; }                                                          \
} while(0)


  /* *INDENT-OFF* */
#ifdef __cplusplus
}
#endif
/* *INDENT-ON* */


#endif /* SH_MACROS_H_ */
