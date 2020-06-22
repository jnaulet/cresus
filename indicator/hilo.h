/*
 * Cresus EVO - hilo.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 12.25.2017
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef HILO_H
#define HILO_H

/*
 * High / Low
 * Support / Resistance indicator
 * Typical period is 30
 */

#include "framework/types.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct hilo_n3 {
  /* Inherits from indicator_n3 */
  struct indicator_n3 indicator_n3;
  /* Data */
  double high;
  double low;
};

#define hilo_n3_alloc(n3, parent)					\
  DEFINE_ALLOC(struct hilo_n3, n3, hilo_n3_init, parent)
#define hilo_n3_free(n3)			\
  DEFINE_FREE(n3, hilo_n3_free)

static inline int hilo_n3_init(struct hilo_n3 *n3,
			       struct indicator *parent)
{
  indicator_n3_init(&n3->indicator_n3, parent); /* super() */
  return 0;
}

static inline void hilo_n3_release(struct hilo_n3 *n3)
{
  indicator_n3_release(&n3->indicator_n3);
}

/* Main object */

#define hilo_alloc(hl, id, period, filter)			\
  DEFINE_ALLOC(struct hilo, hl, hilo_init, id, period, filter)
#define hilo_free(hl)				\
  DEFINE_FREE(hl, hilo_release)

struct hilo {
  /* Inherits from indicator */
  struct indicator indicator;
  /* Internals */
  int period;
  int filter;
};

int hilo_init(struct hilo *r, unique_id_t id, int period, int filter);
void hilo_release(struct hilo *r);

void hilo_reset(struct hilo *r);

#endif
