/*
 * Cresus EVO - rs_dorsey.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RS_DORSEY_H
#define RS_DORSEY_H

/*
 * Dorsey Relative Strength
 * Very simple formula :
 * RSD = (close / close_index) * 100.0
 */

#include "framework/indicator.h"
#include "framework/timeline_entry.h"

struct rs_dorsey {
  /* As always, inherits from indicator */
  __inherits_from_indicator__;

  double value;
  struct candle *ref;
};

int rs_dorsey_init(struct rs_dorsey *r, struct candle *ref);
void rs_dorsey_free(struct rs_dorsey *r);

/* Indicator-specific */
double rs_dorsey_value(struct rs_dorsey *r);

#endif
