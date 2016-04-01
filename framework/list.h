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

/* Superclass */

#define __inherits_from_list__ struct list __parent_list__
#define __list_is_superclass__ void *__self_list__
#define __list__(x) (&(x)->__parent_list__)
#define __list_self__(x) ((struct list*)(x)->__self_list__)
#define __list_self_init__(x, self) (x)->__self_list__ = self

#define __list_super__(self)			\
  __list_self_init__(__list__(self), self);	\
  list_init(__list__(self));
#define __list_free__(self) list_free(__list__(self));

#define __list_add__(list, entry)		\
  list_add((list), __list__(entry))
#define __list_add_tail__(list, entry)		\
  list_add_tail((list), __list__(entry))
#define __list_del__(entry)			\
  list_del(&__list__(entry))
/* Iteration */
#define __list_for_each__(list, self)			\
  for(self = __list_self__((list)->next);		\
      __list__(self) != (list);				\
      self = __list_self__(__list__(self)->next))
#define __list_for_each_prev__(list, self)		\
  for(self = __list_self__((list)->prev);		\
      __list__(self) != (list);				\
      self = __list_self__(__list__(self)->prev))

/* Basic list object */

struct list {
  __list_is_superclass__;
  struct list *prev, *next;
};

static inline int list_init(struct list *l) {
  l->next = l;
  l->prev = l;
}

static inline void list_free(struct list *l) {
  l->next = NULL;
  l->prev = NULL;
}

static inline void list_add(struct list *l, struct list *new) {
  l->next->prev = new;
  new->next = l->next;
  new->prev = l->prev;
  l->prev->next = new;
}

static inline void list_add_tail(struct list *l, struct list *entry) {
  list_add(l->prev, entry);
}

static inline void list_del(struct list *l) {
  l->next->prev = l->prev;
  l->prev->next = l->next;
}

#endif
