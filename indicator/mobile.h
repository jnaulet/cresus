/*
 * Cresus EVO - mobile.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef MOBILE_H
#define MOBILE_H

/*
 * Mobile Average
 * Exponential & Simple
 */

#include "math/average.h"
#include "engine/candle.h"

#include "framework/alloc.h"
#include "framework/indicator.h"

/* TODO : Is that useful ? */
typedef enum {
  MOBILE_MMA,
  MOBILE_EMA
} mobile_t;

typedef enum {
  MOBILE_DIR_UP,
  MOBILE_DIR_DOWN
} mobile_dir_t;

/* Warning /!\
 * this is position of the mobile avg
 * compared to candle value
 * above means candle is below
 * and below means candle is above
 */
typedef enum {
  MOBILE_POS_ABOVE,
  MOBILE_POS_BELOW
} mobile_pos_t;

/* Indicator-specific events */
#define MOBILE_EVENT_CHDIR_UP     0
#define MOBILE_EVENT_CHDIR_DOWN   1
#define MOBILE_EVENT_CROSSED_DOWN 2
#define MOBILE_EVENT_CROSSED_UP   3

/* Timeline entries object */

struct mobile_entry {
  /* As below */
  __inherits_from_indicator_entry__;
  /* Single value */
  double value;
  /* More info */
  double direction;
  /* Events ? */
};

#define mobile_entry_alloc(entry, parent, value, direction)		\
  DEFINE_ALLOC(struct mobile_entry, entry,				\
	       mobile_entry_init, parent, value, direction)
#define mobile_entry_free(entry)			\
  DEFINE_FREE(entry, mobile_indicator_entry_release)

static inline int mobile_entry_init(struct mobile_entry *entry,
				    struct indicator *parent,
				    double value, double direction){
  __indicator_entry_super__(entry, parent);
  entry->value = value;
  entry->direction = direction;
  return 0;
}

static inline void mobile_entry_release(struct mobile_entry *entry) {
  __indicator_entry_release__(entry);
}

/* Main object */

#define mobile_alloc(m, id, type, period, cvalue)			\
  DEFINE_ALLOC(struct mobile, m, mobile_init, id, type, period, cvalue)
#define mobile_free(m)				\
  DEFINE_FREE(m, mobile_release)

struct mobile {
  /* As always */
  __inherits_from_indicator__;
  /* Basic data */
  mobile_t type;
  candle_value_t cvalue;
  /* Average object */
  struct average avg;
};

int mobile_init(struct mobile *m, indicator_id_t id, mobile_t type,
		int period, candle_value_t cvalue);
void mobile_release(struct mobile *m);

/* indicator-specific */
double mobile_average(struct mobile *m);
double mobile_stddev(struct mobile *m);

#endif
