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

/* Inheritance system */

#define __inherits_from_slist__ struct slist __parent_slist__
#define __slist_is_superclass__ void *__self_slist__
#define __slist__(x) (&(x)->__parent_slist__)
#define __slist_self__(x) (x)->__self_slist__
#define __slist_self_init__(x, self) (x)->__self_slist__ = self

#define __slist_super__(self)			\
  __slist_self_init__(__slist__(self), self);	\
  slist_init(__slist__(self))
#define __slist_release__(self) slist_release(__slist__(self))

#define __slist_insert__(slist, entry)		\
  slist_insert((slist), __slist__(entry))
#define __slist_del__(slist)			\
  slist_del(__slist__(slist))
/* Iteration */
#define __slist_for_each__(head, self)			\
  for(struct slist *ptr = (head)->next;			\
      ptr != NULL && (self = __slist_self__(ptr));	\
      ptr = ptr->next)

/* Basic slist object */

#define slist_head_t(type) struct slist /* Type is indicative */
#define slist_head_init(x) slist_init(x)
#define slist_head_release(x) slist_release(x)

struct slist {
  __slist_is_superclass__;
  struct slist *next;
};

static inline int slist_init(struct slist *s) {
  s->next = NULL;
  return 0;
}

static inline void slist_release(struct slist *s) {
  s->next = NULL;
}

static inline void slist_insert(struct slist *s, struct slist *entry) {
  entry->next = s->next;
  s->next = entry;
}

static inline void slist_del(struct slist *s) {
  s->next = s->next->next;
}

#define slist_is_empty(head) ((head)->next == NULL)
#define slist_for_each(head, ptr)				\
  for(ptr = (head)->next; (ptr) != NULL; ptr = (ptr)->next)

#endif
