/*
 * Cresus EVO - slist.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef SLIST_H
#define SLIST_H

#include <stdio.h>
#include "framework/types.h"

#define __slist__(x) ((struct slist*)(x))

/* Exports */
#define __slist_init__(ctx) slist_init(__slist__(ctx))
#define __slist_release__(ctx) slist_release(__slist__(ctx))
#define __slist_push__(ctx, n3)                    \
  slist_push(__slist__(ctx), __slist__(n3))
#define __slist_pop__(ctx, n3)			\
  slist_pop(__slist__(ctx), (struct slist**)(n3))
/* Iteration */
#define __slist_for_each__(head, ctx)                           \
  for(struct slist *__ptr__ = __slist__(head)->next;            \
      __ptr__ != NULL && (ctx = (typeof(ctx))(__ptr__));        \
      __ptr__ = __ptr__->next)
  
/* Basic slist object */

#define slist_head_t(type) struct slist /* Type is indicative */
#define slist_head_init(x) slist_init(x)
#define slist_head_release(x) slist_release(x)

struct slist {
  struct slist *next;
};

static inline int slist_init(struct slist *ctx)
{
  ctx->next = NULL;
  return 0;
}

static inline void slist_release(struct slist *ctx)
{
  ctx->next = NULL;
}

/* Warning : only use head as ctx */

static inline void slist_push(struct slist *ctx, struct slist *n3)
{
  n3->next = ctx->next;
  ctx->next = n3;
}

static inline struct slist *slist_pop(struct slist *ctx,
				      struct slist **n3)
{
  *n3 = ctx->next;
  if(*n3) ctx->next = ctx->next->next;
  return *n3;
}

#define slist_is_last(ctx) ((ctx)->next == NULL)
#define slist_is_empty(head) slist_is_last(head)
#define slist_for_each(head, ptr)				\
  for(ptr = (head)->next; (ptr) != NULL; ptr = (ptr)->next)

/*
 * More advanced object : slist_by_uid
 */

#define __slist_by_uid__(x) ((struct slist_by_uid*)(x))

struct slist_by_uid {
  __inherits_from__(struct slist);
  unique_id_t uid;
};

static inline int
slist_by_uid_init(struct slist_by_uid *ctx, unique_id_t uid)
{
  __slist_init__(ctx); /* super() */
  ctx->uid = uid;
  return 0;
}

static inline void
slist_by_uid_release(struct slist_by_uid *ctx)
{
  __slist_release__(ctx);
}

#define __slist_by_uid_init__(ctx, uid)         \
  slist_by_uid_init(__slist_by_uid__(ctx), uid)
#define __slist_by_uid_release__(ctx)		\
  slist_by_uid_release(__slist_by_uid__(ctx))

static inline struct slist_by_uid *
slist_by_uid_find(struct slist_by_uid *ctx, unique_id_t uid)
{
  struct slist_by_uid *ptr;
  __slist_for_each__(ctx, ptr){
    if(ptr->uid == uid)
      return ptr;
  }

  return NULL;
}

#define __slist_by_uid_find__(ctx, uid)         \
  slist_by_uid_find(__slist_by_uid__(ctx), uid)

#endif
