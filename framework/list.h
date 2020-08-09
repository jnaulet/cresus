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

static inline struct list *list_pop(struct list *l)
{
  struct list *n = l->next;
  list_del(n);
  return n;
}

static inline struct list *list_prev_safe(struct list *l)
{
  return (!l->prev->is_head ? l->prev : l);
}

/* Head */

#define list_head_t(type) struct list /* Type is purely indicative */
#define list_head_init(ctx) ({ list_init((ctx)); (ctx)->is_head = 1; })
#define list_head_release(ctx) list_release((ctx))
#define list_is_head(ctx) ((ctx)->is_head)

/* Iteration */

#define list_for_each(head, l)				\
  for(l = (head)->next; !(l)->is_head; l = (l)->next)
#define list_for_each_prev(head, l)			\
  for(l = (head)->prev; !(l)->is_head; l = (l)->prev)
#define list_for_each_safe(head, l, n)		\
  for(l = (head)->next, n = (l)->next;		\
      (l) != (head); l = (n), n = (l)->next)

#endif
