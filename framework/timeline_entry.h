/*
 * Cresus EVO - timeline_entry.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/05/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TIMELINE_ENTRY_H
#define TIMELINE_ENTRY_H

#include <time.h>
#include "list.h"

/* Define furtherly required types */
typedef time_t granularity_t;

#define GRANULARITY_SEC   1
#define GRANULARITY_MIN   60
#define GRANULARITY_HOUR  3600
#define GRANULARITY_DAY   86400
#define GRANULARITY_WEEK  432000
#define GRANULARITY_MONTH 1728000
#define GRANULARITY_YEAR  20736000

/* This is a superclass */

#define __inherits_from_timeline_entry__		\
  struct timeline_entry __parent_timeline_entry__
#define __timeline_entry_is_superclass__ void *__self_timeline_entry__
#define __timeline_entry__(x) (&(x)->__parent_timeline_entry__)
#define __timeline_entry_self__(x) (x)->__self_timeline_entry__
#define __timeline_entry_self_init__(x, self)	\
  __timeline_entry_self__(x) = self

#define __timeline_entry_super__(self, time, granularity)		\
  __timeline_entry_self_init__(__timeline_entry__(self), self);		\
  timeline_entry_init(__timeline_entry__(self), time, granularity)
#define __timeline_entry_free__(self)			\
  timeline_entry_free(__timeline_entry__(self))

#define __timeline_entry_timecmp__(self, time)			\
  timeline_entry_timecmp(__timeline_entry__(self), time)
#define __timeline_entry_find__(self, time)		\
  timeline_entry_find(__timeline_entry__(self), time)
#define __timeline_entry_localtime_str__(self, buf, len)		\
  timeline_entry_localtime_str(__timeline_entry__(self), buf, len)

struct timeline_entry {
  /* parent */
  __inherits_from_list__;
  __timeline_entry_is_superclass__;
  
  /* Time/Date management */
  time_t time; /* Epoch */
  granularity_t granularity; /* epoch-format, too */
};

int timeline_entry_init(struct timeline_entry *e, time_t time, granularity_t g);
void timeline_entry_free(struct timeline_entry *e);

/* Methods */
time_t timeline_entry_timecmp(struct timeline_entry *e, time_t time);
struct timeline_entry *timeline_entry_find(struct timeline_entry *t, time_t time);
const char *timeline_entry_localtime_str(struct timeline_entry *e, char *buf, size_t len);

#endif
