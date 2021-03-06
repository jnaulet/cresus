/*
 * Cresus EVO - rsi.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 04/04/2016
 * Copyright (c) 2016 Joachim Naulet. All rights reserved.
 *
 */

#ifndef RSI_H
#define RSI_H

/*
 * Relative Strength Index
 * Momentum indicator
 * Typical period is 12
 */

#include "math/average.h"
#include "engine/candle.h"

#include "framework/types.h"
#include "framework/indicator.h"

struct rsi {
  /* Inherits from indicator */
  __inherits_from__(struct indicator);
  
  /* https://fr.wikipedia.org/wiki/Relative_strength_index */
  struct average h;
  struct average b;
  struct candle *last;

  double value;
};

int rsi_init(struct rsi *r, unique_id_t id, int period);
void rsi_release(struct rsi *r);

/* Indicator-specific */
double rsi_value(struct rsi *r);

#endif
