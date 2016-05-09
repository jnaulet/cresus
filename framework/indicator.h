/*
 * Cresus EVO - indicator.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__indicator__
#define __Cresus_EVO__indicator__

#include "slist.h"
#include "timeline_entry.h"

#include <stdlib.h>

/* As it's a superclass, we want macros to manipulate this */
#define __inherits_from_indicator__ struct indicator __parent_indicator__
#define __indicator_is_superclass__ void *__self_indicator__
#define __indicator__(x) (&(x)->__parent_indicator__)
#define __indicator_self__(x) (x)->__self_indicator__
#define __indicator_self_init__(x, self) (x)->__self_indicator__ = self

#define __indicator_super__(self, id, feed, reset)	\
  __indicator_self_init__(__indicator__(self), self);	\
  indicator_init(__indicator__(self), id, feed, reset)
#define __indicator_release__(self)		\
  indicator_release(__indicator__(self))

/* Set methods */
#define __indicator_set_string__(self, fmt, ...)			\
  snprintf(__indicator__(self)->str, INDICATOR_STR_MAX,			\
	   fmt, ##__VA_ARGS__)
#define __indicator_set_event__(self, candle, event)		\
  indicator_set_event(__indicator__(self), candle, event)
#define __indicator_feed__(self, entry)		\
  indicator_feed(__indicator__(self), entry);

/* Internal values get */
#define __indicator_string__(self) __indicator__(self)->str
//#define __indicator_candle_value__(self) __indicator__(self)->value

/* Define types */
struct indicator; /* FIXME : find something more elegant */
typedef unsigned int indicator_id_t;
typedef int (*indicator_feed_ptr)(struct indicator*, struct timeline_entry*);
typedef void (*indicator_reset_ptr)(struct indicator*);

struct indicator {
  /* Inherits from slist */
  __inherits_from_slist__;
  __indicator_is_superclass__;

#define INDICATOR_STR_MAX 64
  /* Fn pointers */
  indicator_feed_ptr feed;
  indicator_reset_ptr reset;
  /* Unique Id & name */
  indicator_id_t id;
  char str[INDICATOR_STR_MAX];
  /* Status */
  int is_empty;
  /* Data / graph */
  list_head_t(struct timeline_entry) list_entry;
};

int indicator_init(struct indicator *i, indicator_id_t id,
		   indicator_feed_ptr feed, indicator_reset_ptr reset);
void indicator_release(struct indicator *i);

int indicator_feed(struct indicator *i, struct timeline_entry *e);
void indicator_reset(struct indicator *i);

/* Indicators generated sub-object */

#define __inherits_from_indicator_entry__		\
  struct indicator_entry __parent_indicator_entry__
#define __indicator_entry_is_superclass__ void *__self_indicator_entry__
#define __indicator_entry__(x) (&(x)->__parent_indicator_entry__)
#define __indicator_entry_self__(x) ((x)->__self_indicator_entry__)
#define __indicator_entry_self_init__(x, self)	\
  (x)->__self_indicator_entry__ = self

#define __indicator_entry_super__(self, indicator)			\
  __indicator_entry_self_init__(__indicator_entry__(self), self);	\
  indicator_entry_init(__indicator_entry__(self), indicator);
#define __indicator_entry_release__(self)			\
  indicator_entry_release(__indicator_entry__(self));

struct indicator_entry {
  __inherits_from_slist__;
  __indicator_entry_is_superclass__;
  /* Say who is parent ? */
  struct indicator *indicator;
  /* Maybe this one's of better use */
  indicator_id_t iid;
};

static inline int indicator_entry_init(struct indicator_entry *entry,
				       struct indicator *parent) {
  __slist_super__(entry);
  entry->indicator = parent;
  entry->iid = parent->id;
  return 0; /* alloc rulz */
}

static inline void indicator_entry_release(struct indicator_entry *entry) {
  __slist_release__(entry);
}

#endif /* defined(__Cresus_EVO__indicator__) */
