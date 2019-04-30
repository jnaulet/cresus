/*
 * Cresus EVO - atr.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef ATR_H
#define ATR_H

#include "framework/types.h"
#include "framework/indicator.h"

struct atr {
  /* Parent */
  __inherits_from__(struct indicator);
  
  int period;
  struct candle *ref;
  
  double value;
};

int atr_init(struct atr *ctx, unique_id_t id, int period);
void atr_release(struct atr *ctx);

/* Indicator-specific */
double atr_value(struct atr *ctx);

#endif
