/*
 * Cresus EVO - engine.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 10/21/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__engine__
#define __Cresus_EVO__engine__

#include "engine/order.h"
#include "engine/position.h"
#include "engine/timeline.h"

struct engine {
  struct timeline *timeline;
  /* Order fifo */
  list_head_t(struct order) list_order;
  /* Position management. OBSOLETE */
  list_head_t(struct position) list_position_to_open;
  list_head_t(struct position) list_position_opened;
  list_head_t(struct position) list_position_to_close;
  list_head_t(struct position) list_position_closed;
  /* Money management */
  double npos;
  double amount; /* FIXME: find another name */
  double earnings; /* Find another name */
  /* Last close value (FIXME) */
  double close;
};

/* Extrenal pointer to plugin */
typedef int (*engine_feed_ptr)(struct engine*, struct timeline*, struct timeline_entry*);

int engine_init(struct engine *e, struct timeline *t);
void engine_release(struct engine *e);

void engine_run(struct engine *e, engine_feed_ptr feed);

int engine_place_order(struct engine *ctx, order_t type, order_by_t by,
		       double value);

#endif /* defined(__Cresus_EVO__engine__) */
