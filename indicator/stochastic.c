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

static int stochastic_feed(struct indicator *i, struct timeline_track_n3 *e)
{  
  struct stochastic *ctx = (void*)i;
  struct candle *candle = (void*)e;
  
  memcpy(&ctx->array[ctx->index], candle, sizeof *candle);
  ctx->index = (ctx->index + 1) % ctx->period;
  
  /* Find top & bottom */
  double hi = 0.0, lo = DBL_MAX;
  for(int i = 0; i < ctx->period; i++){
    hi = (ctx->array[i].high > hi ? ctx->array[i].high : hi);
    lo = (ctx->array[i].low < lo ? ctx->array[i].low : lo);
  }
  
  double pk = (candle->close - lo) / (hi - candle->close) * 100.0;
  double avg = average_update(&ctx->smooth_k, pk);
  if(average_is_available(&ctx->smooth_k))
    average_update(&ctx->d, avg);
  
  return 0;
}

static void stochastic_reset(struct indicator *i) {

  struct stochastic *ctx = (void*)i;
  /* Reset */
  ctx->index = 0;
  average_reset(&ctx->d);
  average_reset(&ctx->smooth_k);
}

int stochastic_init(struct stochastic *ctx, unique_id_t id,
		    int period, int k, int d) {
  
  /* init() */
  __indicator_init__(ctx, id, stochastic_feed, stochastic_reset);
  __indicator_set_string__(ctx, "sto[%d, %d, %d]", period, k, d);
  
  ctx->k = k;
  ctx->index = 0;
  ctx->period = period;
  
  average_init(&ctx->d, AVERAGE_MATH, period);
  average_init(&ctx->smooth_k, AVERAGE_MATH, period);
  
  if((ctx->array = malloc(sizeof(*ctx->array) * period)))
    return -1;
  
  /* memcpy(&ctx->array[0], seed, sizeof *ctxeed); */
  return 0;
}

void stochastic_release(struct stochastic *ctx) {
  
  __indicator_release__(ctx);
  free(ctx->array);
}
