/*
 * Cresus EVO - bigdata.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/06/16
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef BIGDATA_H
#define BIGDATA_H

#include "framework/list.h"
#include "framework/indicator.h"
#include "framework/timeline_entry.h"

struct list_indicator {
  /* If it only does that, don't use this struct */
  __inherits_from_list__;
  struct indicator *indicator;
};

struct list_timeline_entry {
  __inherits_from_list__;
  char name[];
  struct timeline_entry *timeline_entry;
};

#endif
