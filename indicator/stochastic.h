/*
 * Cresus EVO - stochastic.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/19/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__stochastic__
#define __Cresus_EVO__stochastic__

#include "math/average.h"
#include "engine/candle.h"

#include "framework/types.h"
#include "framework/indicator.h"

struct stochastic {
  __inherits_from__(struct indicator);
  
  int period, k;
  struct average smooth_k;
  struct average d;
  
  int index;
  struct candle *array;
};

int stochastic_init(struct stochastic *ctx, unique_id_t id,
		    int period, int k, int d);
void stochastic_release(struct stochastic *ctx);

#endif /* defined(__Cresus_EVO__stochastic__) */
