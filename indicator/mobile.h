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
#include "framework/indicator.h"

typedef enum {
  MOBILE_MMA,
  MOBILE_EMA
} mobile_t;

typedef enum {
  MOBILE_DIR_UP,
  MOBILE_DIR_DOWN
} mobile_dir_t;

/* Warning
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

struct mobile {
  /* As always */
  __inherits_from_indicator__;
  
  mobile_t type;
  mobile_dir_t dir;
  mobile_pos_t pos;
  candle_value_t cvalue;

  /* Average object */
  struct average avg;
};

int mobile_init(struct mobile *m, mobile_t type,
		int period, candle_value_t cvalue);
void mobile_free(struct mobile *m);

/* indicator-specific */
double mobile_average(struct mobile *m);
double mobile_stddev(struct mobile *m);

#endif
