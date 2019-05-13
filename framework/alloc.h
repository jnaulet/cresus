/*
 * Cresus EVO - alloc.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 22/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>

/*
 * Object init MUST always return 0 on success & -1 on failure
 */
#define DEFINE_ALLOC(type, ptr, init, ...)                      \
  ((ptr = malloc(sizeof(type))) ?                               \
   (init(ptr, ##__VA_ARGS__) != -1 ? ptr : free_null(ptr)) :    \
   NULL)
#define DEFINE_FREE(ptr, release)		\
  ({ release(ptr); free(ptr); })

static inline void *free_null(void *ptr)
{
  free(ptr);
  return NULL;
}

static inline void release_dummy(void *ptr)
{
}

#endif
