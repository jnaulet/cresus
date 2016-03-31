//
//  stochastic.h
//  Cresus EVO
//
//  Created by Joachim Naulet on 19/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#ifndef __Cresus_EVO__stochastic__
#define __Cresus_EVO__stochastic__

#include "framework/indicator.h"
#include "math/average.h"

struct stochastic {
  struct indicator parent;
  
  int period, k;
  struct average smooth_k;
  struct average d;
  
  int index;
  struct candle *array;
};

int stochastic_init(struct stochastic *s, int period, int k, int d,
                    const struct candle *seed);

void stochastic_free(struct stochastic *s);

int stochastic_feed(struct indicator *i, const struct candle *candle);

#endif /* defined(__Cresus_EVO__stochastic__) */
