/*
 * Cresus EVO - bollinger.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef BOLLINGER_H
#define BOLLINGER_H

#include "math/average.h"
#include "engine/candle.h"
#include "framework/indicator.h"

struct bollinger_value {
  double mma;
  double hi, lo;
};

struct bollinger {
  /* Inherits from indicator */
  __inherits_from_indicator__;
  
  struct average avg;
  double stddev_factor;
  
  /* Indicator-specific */
  candle_value_t cvalue;
  struct bollinger_value value;
  
  char str[32]; /* FIXME */
};

int bollinger_init(struct bollinger *b, indicator_id_t id, int period,
		   double stddev_factor, candle_value_t cvalue);
void bollinger_release(struct bollinger *b);

/* Indicator-specific */
struct bollinger_value *bollinger_get_value(struct bollinger *b);
const char *bollinger_str(struct bollinger *b);

#endif
