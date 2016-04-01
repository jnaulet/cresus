/*
 * Cresus EVO - timeline.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/06/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef TIMELINE_H
#define TIMELINE_H

#include "framework/input.h"
#include "framework/indicator.h"
#include "framework/timeline_entry.h"

#define TIMELINE_NAME_MAX 256

struct timeline {
  char name[TIMELINE_NAME_MAX];
  /* Main data / graph */
  struct timeline_entry list_entry;
  struct timeline_entry *cache;
  /* Secondary graphs */
  struct indicator slist_indicator;
};

int timeline_init(struct timeline *t);
void timeline_free(struct timeline *t);

int timeline_add_indicator(struct timeline *t, struct indicator *i);
int timeline_step(struct timeline *t, struct input *in);
int timeline_load(struct timeline *t, struct input *in);

#define timeline_entries(x)				\
  __timeline_entry_self__(&(x)->list_entry)

#endif
