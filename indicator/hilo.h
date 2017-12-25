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

#include "engine/candle.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct hilo_entry {
  /* As always... */
  __inherits_from_indicator_entry__;
  /* Data */
  double high;
  double low;
};

#define hilo_entry_alloc(entry, parent)					\
  DEFINE_ALLOC(struct hilo_entry, entry, hilo_entry_init, parent)
#define hilo_entry_free(entry)			\
  DEFINE_FREE(entry, hilo_entry_free)

static inline int hilo_entry_init(struct hilo_entry *entry,
				 struct indicator *parent) {
  __indicator_entry_super__(entry, parent);
  return 0;
}

static inline void hilo_entry_release(struct hilo_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Main object */

#define hilo_alloc(hl, id, period, filter)			\
  DEFINE_ALLOC(struct hilo, hl, hilo_init, id, period, filter)
#define hilo_free(hl)				\
  DEFINE_FREE(hl, hilo_release)

struct hilo {
  /* Inherits from indicator */
  __inherits_from_indicator__;
  
  int period;
  int filter;
};

int hilo_init(struct hilo *r, indicator_id_t id, int period, int filter);
void hilo_release(struct hilo *r);

void hilo_reset(struct hilo *r);

#endif
