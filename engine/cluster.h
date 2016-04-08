/*
 * Cresus EVO - cluster.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 20/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef CLUSTER_H
#define CLUSTER_H

#include "framework/list.h"
#include "framework/slist.h"
#include "engine/timeline.h"

struct cluster {
  /* First, a cluster is a timeline cause it's an indice computer */
  __inherits_from_timeline__;
  /* Second, a cluster's a list of timelines */
  __slist_head__(struct timeline) slist_timeline;
  /* We need to speed things up finding entries */
  __list_head__(struct timeline_entry) *ref;
  /* Time info */
  granularity_t g;
  time_info_t time, time_max;
};

int cluster_init(struct cluster *c, const char *name, time_info_t time_min, time_info_t time_max);
void cluster_release(struct cluster *c);

int cluster_add_timeline(struct cluster *c, struct timeline *t);
int cluster_step(struct cluster *c);

/* TODO : add functions/macros to manipulate easily indicators/timelines */

#endif
