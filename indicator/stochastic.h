/*
 * Cresus EVO - stochastic.h 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/19/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#ifndef __Cresus_EVO__stochastic__
#define __Cresus_EVO__stochastic__

#include "framework/indicator.h"
#include "math/average.h"

struct stochastic {
  __inherits_from_indicator__;
  
  int period, k;
  struct average smooth_k;
  struct average d;
  
  int index;
  struct candle *array;
};

int stochastic_init(struct stochastic *s, int period, int k, int d);
void stochastic_free(struct stochastic *s);

#endif /* defined(__Cresus_EVO__stochastic__) */
