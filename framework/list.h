/*
 * Cresus EVO - list.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef LIST_H
#define LIST_H

#include <stdio.h>

#define __list__(x) ((struct list*)(x))

#define __list_init__(ctx) list_init(__list__(ctx))
#define __list_release__(ctx) list_release(__list__(ctx))

#define __list_add__(ctx, n3)                \
  list_add(__list__(ctx), __list__(n3))
#define __list_add_tail__(ctx, n3)           \
  list_add_tail(__list__(ctx), __list__(n3))
#define __list_del__(ctx)			\
  list_del(__list__(ctx))

/* Iteration */
#define __list_for_each__(head, ctx)					\
  for(ctx = (void*)__list__(head)->next;                                \
      (ctx) && !__list_is_head__(ctx);                                  \
      ctx = (void*)__list__(ctx)->next)
#define __list_for_each_prev__(head, ctx)				\
  for(ctx = (void*)__list__(head)->prev;                                \
      (ctx) && !__list_is_head__(ctx);                                  \
      ctx = (void*)__list__(ctx)->prev)
#define __list_for_each_safe__(head, ctx, safe)                         \
  for(ctx = (void*)__list__(head)->next,                                \
	(ctx) && (safe = (typeof(ctx))__list__(ctx)->next);		\
      (ctx) && !__list_is_head__(ctx);                                  \
      ctx = (void*)(safe),                                              \
	(ctx) && (safe = (typeof(ctx))__list__(ctx)->next))
#define __list_for_each_prev_safe__(head, ctx, safe)			\
  for(ctx = (void*)__list__(head)->prev,                                \
        (ctx) && (safe = __list__(ctx)->prev);                          \
      (ctx) && !__list_is_head__(ctx);                                  \
      ctx = (void*)(safe),                                              \
	(ctx) && (safe = __list__(ctx)->prev))
  
/* Relative functions */
#define __list_relative__(ctx, n)                       \
  ((typeof(ctx))list_relative(__list__(ctx), n))
#define __list_prev_n__(ctx, n)                 \
  ((typeof(ctx))list_prev_n(__list__(ctx), n))
#define __list_next_n__(ctx, n)                 \
  ((typeof(ctx))list_next_n(__list__(ctx), n))

/* Basic list object */
  
struct list {
  struct list *prev, *next;
  unsigned int is_head;
};

static inline int list_init(struct list *l)
{
  /* Inside */
  l->next = l;
  l->prev = l;
  l->is_head = 0;
  return 0;
}

static inline void list_release(struct list *l)
{
  l->next = NULL;
  l->prev = NULL;
}

static inline void list_add(struct list *l, struct list *new)
{
  new->prev = l->next->prev;
  new->next = l->next;
  l->next->prev = new;
  l->next = new;
}

static inline void list_add_tail(struct list *l, struct list *n3)
{
  list_add(l->prev, n3);
}

static inline void list_del(struct list *l)
{
  l->next->prev = l->prev;
  l->prev->next = l->next;
}

/* Head */

#define list_head_t(type) struct list /* Type is purely indicative */
#define list_head_init(ctx) ({ list_init((ctx)); (ctx)->is_head = 1; })
#define list_head_release(ctx) list_release((ctx))

#define list_is_head(ctx) ((ctx)->is_head)
#define __list_is_head__(ctx) list_is_head(__list__(ctx))

/* Iteration */

#define list_for_each(head, l)				\
  for(l = (head)->next; (l) != (head); l = (l)->next)
#define list_for_each_prev(head, l)			\
  for(l = (head)->prev; (l) != (head); l = (l)->prev)
#define list_for_each_safe(head, l, n)		\
  for(l = (head)->next, n = (l)->next;		\
      (l) != (head); l = (n), n = (l)->next)

/* Relative functions */

static inline struct list *list_prev_n(struct list *l, int n)
{
  while(!list_is_head(l) && n--) l = l->prev;
  return l;
}

static inline struct list *list_next_n(struct list *l, int n)
{
  while(!list_is_head(l) && n--) l = l->next;
  return l;
}

#define list_relative(list, n)			\
  (((n) < 0) ? list_prev_n((list), -(n)) :	\
   list_next_n((list), (n)))

#endif
