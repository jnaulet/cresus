/*
 * Cresus - plist.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 06/23/2020
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef PLIST_H
#define PLIST_H

#include "framework/alloc.h"

struct plist {
  struct plist *prev, *next;
  void *ptr;
};

static inline int plist_init(struct plist *ctx, void *ptr)
{
  ctx->prev = ctx;
  ctx->next = ctx;
  ctx->ptr = ptr;
  return 0;
}

static inline void plist_release(struct plist *ctx)
{
}

#define plist_alloc(ctx, ptr)				\
  DEFINE_ALLOC(struct plist, ctx, plist_init, ptr)
#define plist_free(ctx)				\
  DEFINE_FREE(ctx, plist_release)
  
static inline void plist_add(struct plist *ctx, struct plist *p)
{
  p->prev = ctx->next->prev;
  p->next = ctx->next;
  ctx->next->prev = p;
  ctx->next = p;
}

#define plist_add_tail(ctx, p)			\
  plist_add((ctx)->prev, (p))

static inline void plist_del(struct plist *ctx)
{
  ctx->next->prev = ctx->prev;
  ctx->prev->next = ctx->next;
}

/* Head */
#define plist_head_t(type) struct plist /* Type is purely indicative */
#define plist_head_init(ctx) ({ plist_init((ctx), NULL); })
#define plist_head_release(ctx) ({ plist_release((ctx)); })

/* Iterate */
#define plist_for_each(head, p)				\
  for(p = (head)->next; (p) != (head); p = (p)->next)
#define plist_for_each_prev(head, p)			\
  for(p = (head)->prev; (p) != (head); p = (p)->prev)
#define plist_for_each_safe(head, p, n)		\
  for(p = (head)->next, n = (p)->next;		\
      (p) != (head); p = (n), n = (p)->next)

/* Abstract layer */

static inline void plist_add_ptr(struct plist *ctx, void *ptr)
{
  struct plist *p;
  if(plist_alloc(p, ptr))
    plist_add(ctx, p);
}

#define plist_add_tail_ptr(ctx, ptr)		\
  plist_add_ptr((ctx)->prev, (ptr))

#endif
