/*
 * Cresus EVO - rs_mansfield.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RS_MANSFIELD_H
#define RS_MANSFIELD_H

/*
 * Relative Strength (Mansfield)
 * Fomula :
 * RSM = ((RSD(today) / mma(RSD(today), n)) - 1) * 100.0
 */

#include "math/average.h"
#include "framework/indicator.h"

struct rs_mansfield {
  /* As always, inherits from indicator */
  __inherits_from_indicator__;
  
  struct average mma;
  struct candle *ref;

  double value;
};

int rs_mansfield_init(struct rs_mansfield *r, int period, struct candle *ref);
void rs_mansfield_free(struct rs_mansfield *r);

/* Indicator-specific */
double rs_mansfield_value(struct rs_mansfield *r);

#endif
