/*
 * Cresus EVO - lowest.h
 *
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 08.05.2018
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef LOWEST_H
#define LOWEST_H

/*
 * Lowest
 * Some support indicator
 */

#include "engine/candle.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

struct lowest_entry {
  /* As always... */
  __inherits_from_indicator_entry__;
  /* Data */
  double value;
};

#define lowest_entry_alloc(entry, parent)				\
  DEFINE_ALLOC(struct lowest_entry, entry, lowest_entry_init, parent)
#define lowest_entry_free(entry)		\
  DEFINE_FREE(entry, lowest_entry_free)

static inline int lowest_entry_init(struct lowest_entry *entry,
					  struct indicator *parent) {
  __indicator_entry_super__(entry, parent);
  return 0;
}

static inline void lowest_entry_release(struct lowest_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Main object */

#define lowest_alloc(ls, id, period)				\
  DEFINE_ALLOC(struct lowest, ls, lowest_init, id, period)
#define lowest_free(ls)			\
  DEFINE_FREE(ls, lowest_release)

struct lowest {
  /* Inherits from indicator */
  __inherits_from_indicator__;
  int period;
};

int lowest_init(struct lowest *ctx, indicator_id_t id, int period);
void lowest_release(struct lowest *ctx);

void lowest_reset(struct lowest *ctx);

#endif
