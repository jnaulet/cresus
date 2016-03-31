//
//  stochastic.c
//  Cresus EVO
//
//  Created by Joachim Naulet on 19/11/2014.
//  Copyright (c) 2014 Joachim Naulet. All rights reserved.
//

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "stochastic.h"

int stochastic_init(struct stochastic *s, int period, int k, int d,
                    const struct candle *seed) {
  
  /* super() */
  indicator_init(&s->parent, 0, stochastic_feed);
  
  s->k = k;
  s->period = period;
  /* FIXME */
  average_init(&s->d, AVERAGE_MATH, period, 0.0);
  average_init(&s->smooth_k, AVERAGE_MATH, period, 0.0);
  
  if((s->array = malloc(sizeof(*s->array) * period)))
    return -1;
  
  s->index = 1;
  memcpy(&s->array[0], seed, sizeof *seed);
  
  return 0;
}

void stochastic_free(struct stochastic *s) {
  
  indicator_free(&s->parent);
  free(s->array);
}

int stochastic_feed(struct indicator *i, const struct candle *candle) {
  
  struct stochastic *s = (struct stochastic*)i;
  
  memcpy(&s->array[s->index], candle, sizeof *candle);
  s->index = (s->index + 1) % s->period;
  
  /* Find top & bottom */
  double hi = 0.0, lo = DBL_MAX;
  for(int i = 0; i < s->period; i++){
    hi = (s->array[i].high > hi ? s->array[i].high : hi);
    lo = (s->array[i].low < lo ? s->array[i].low : lo);
  }
  
  double pk = (candle->close - lo) / (hi - candle->close) * 100.0;
  double avg = average_update(&s->smooth_k, pk);
  average_update(&s->d, avg);
  
  return 0;
}
