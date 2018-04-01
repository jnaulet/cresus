/*
 * Cresus EVO - zigzag.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ZIGZAG_H
#define ZIGZAG_H

#include "engine/candle.h"
#include "framework/alloc.h"
#include "framework/indicator.h"

/* Common */

typedef enum {
  ZIGZAG_NONE,
  ZIGZAG_UP,
  ZIGZAG_DOWN,
} zigzag_dir_t;

/* Entries */

#define zigzag_entry_alloc(entry, parent, dir, value, count)	\
  DEFINE_ALLOC(struct zigzag_entry, entry,			\
	       zigzag_entry_init, parent, dir, value, count)
#define zigzag_entry_free(entry)		\
  DEFINE_FREE(entry, zigzag_entry_release)

struct zigzag_entry {
  /* As always */
  __inherits_from_indicator_entry__;
  /* Self */
  double value;
  zigzag_dir_t dir;
  struct candle *ref;
  int n_since_last_ref;
};

static inline int zigzag_entry_init(struct zigzag_entry *entry,
				    struct indicator *parent,
				    zigzag_dir_t dir, double value,
				    int count){
  __indicator_entry_super__(entry, parent);
  entry->dir = dir;
  entry->value = value;
  entry->n_since_last_ref = count;
  return 0;
}

static inline void zigzag_entry_release(struct zigzag_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Indicator */

#define zigzag_alloc(z, id, thres, cvalue)				\
  DEFINE_ALLOC(struct zigzag, z, zigzag_init, id, thres, cvalue)
#define zigzag_free(z)				\
  DEFINE_FREE(z, zigzag_release)

struct zigzag {
  /* As usual */
  __inherits_from_indicator__;
  /* Params */
  double threshold;
  candle_value_t cvalue;
  /* Internals */
  zigzag_dir_t dir;
  /* Some other stuff */
  int ref_count; /* Candles since base ref */
  struct candle *ref, *base_ref;
};

int zigzag_init(struct zigzag *z, indicator_id_t id,
		double threshold, candle_value_t cvalue);
void zigzag_release(struct zigzag *z);

#endif
