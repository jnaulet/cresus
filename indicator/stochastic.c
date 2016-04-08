/*
 * Cresus EVO - stochastic.c 
 * 
 * Created by Joachim Naulet <jnaulet@rdinnovation.fr> on 11/19/2014
 * Copyright (c) 2014 Joachim Naulet. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <float.h>

#include "stochastic.h"

static int stochastic_feed(struct indicator *i, struct timeline_entry *e) {
  
  struct stochastic *s = __indicator_self__(i);
  struct candle *candle = __timeline_entry_self__(e);
  
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
  if(average_is_available(&s->smooth_k))
    average_update(&s->d, avg);
  
  return 0;
}


int stochastic_init(struct stochastic *s, indicator_id_t id,
		    int period, int k, int d) {
  
  /* super() */
  __indicator_super__(s, id, stochastic_feed);
  __indicator_set_string__(s, "sto[%d, %d, %d]", period, k, d);
  
  s->k = k;
  s->index = 0;
  s->period = period;

  average_init(&s->d, AVERAGE_MATH, period);
  average_init(&s->smooth_k, AVERAGE_MATH, period);
  
  if((s->array = malloc(sizeof(*s->array) * period)))
    return -1;
  
  /* memcpy(&s->array[0], seed, sizeof *seed); */
  return 0;
}

void stochastic_release(struct stochastic *s) {
  
  __indicator_release__(s);
  free(s->array);
}
