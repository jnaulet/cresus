/*
 * Cresus EVO - position.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04.05.2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef POSITION_H
#define POSITION_H

#include "engine/candle.h"
#include "engine/timeline.h"
#include "framework/list.h"
#include "framework/alloc.h"

typedef enum {
  POSITION_LONG,
  POSITION_SHORT
} position_t;

#define position_alloc(p, timeline, type, n)			\
  DEFINE_ALLOC(struct position, p, position_init, t, type, n)
#define position_free(p)			\
  DEFINE_FREE(p, position_release)

struct position {
  /* slistable */
  __inherits_from_list__;
  /* Some data */
  struct timeline *t;
  
  struct candle *in;
  struct candle *out;
  
  int n;
  position_t type;
};

int position_init(struct position *p, struct timeline *t,
		  position_t type, int n);
void position_release(struct position *p);

typedef int (*position_action_t)(struct position *p);
int position_in(struct position *p); /* position_action_t */
int position_out(struct position *p); /* position_action_t */

double position_value(struct position *p);

#endif
